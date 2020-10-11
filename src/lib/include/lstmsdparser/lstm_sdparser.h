#ifndef __LSTMSDPARSER_PARSER_H__
#define __LSTMSDPARSER_PARSER_H__

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>
#include <sstream>
#include <time.h>
#include <vector>


#include <queue>
#include <unordered_map>
#include <unordered_set>


#ifndef _WIN32
#include <execinfo.h>
#endif

#include <signal.h>
#include <unistd.h>


#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/program_options.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>


#include "dynet/dynet.h"
#include "dynet/expr.h"
#include "dynet/lstm.h"
#include "dynet/nodes.h"
#include "dynet/rnn.h"
#include "dynet/training.h"
#include "lstmsdparser/bilstm.h"
#include "lstmsdparser/c2.h"
#include "lstmsdparser/treelstm.h"


namespace lstmsdparser {

using namespace dynet::expr;
using namespace dynet;
using namespace std;
namespace po = boost::program_options;

constexpr const char *ROOT_SYMBOL = "ROOT";
const std::string REL_NULL = "__NULL__";
const std::string REL_EXIST = "__EXIST__";

// std::string StrToLower(const std::string s);

typedef struct Sizes {
  unsigned kROOT_SYMBOL;
  unsigned ACTION_SIZE;
  unsigned VOCAB_SIZE;
  unsigned POS_SIZE;
} Sizes;

typedef struct Options {
  unsigned LAYERS;            // 2
  unsigned INPUT_DIM;         // 100
  unsigned HIDDEN_DIM;        // 200
  unsigned ACTION_DIM;        // 50
  unsigned PRETRAINED_DIM;    // 100
  unsigned LSTM_INPUT_DIM;    // 200
  unsigned POS_DIM;           // 50
  unsigned REL_DIM;           // 50
  unsigned BILSTM_HIDDEN_DIM; // 100
  unsigned max_itr;           // 10000
  std::string optimizer;
  std::string transition_system; // "list"
  std::string corpus;            // "sem16"
  std::string dynet_seed;
  std::string dynet_mem;
  bool USE_POS;         // true
  bool USE_BILSTM;      // false
  bool USE_TREELSTM;    // false
  bool UPDATE_ANCESTOR; // false
  bool USE_ATTENTION;   // false
  bool POST_PROCESS;    // true
  bool SDP_OUTPUT;      // false
  bool HAS_HEAD;        // false
  bool USE_2MODEL;      // false
  bool USE_3MODEL;      // false
} Options;

typedef struct Parameters {
  LSTMBuilder stack_lstm; // (layers, input, hidden, trainer)
  LSTMBuilder buffer_lstm;
  LSTMBuilder pass_lstm; // lstm for pass buffer
  LSTMBuilder action_lstm;
  BidirectionalLSTMLayer buffer_bilstm; //[bilstm] bilstm for buffer
  TheirTreeLSTMBuilder tree_lstm;       // [treelstm] for subtree
  LookupParameter p_w;                  // word embeddings
  LookupParameter p_t; // pretrained word embeddings (not updated)
  LookupParameter p_a; // input action embeddings
  LookupParameter p_r; // relation embeddings
  LookupParameter p_p; // pos tag embeddings
  Parameter p_pbias;   // parser state bias
  Parameter p_A;       // action lstm to parser state
  Parameter p_B;       // buffer lstm to parser state
  // Parameter p_fwB; // [bilstm] buffer forward lstm to parser state
  // Parameter p_bwB; // [bilstm] buffer backward lstm to parser state
  Parameter p_biB; // [bilstm] buffer concatenate fw and bw lstm to parser state
  Parameter p_P;   // pass lstm to parser state
  Parameter p_S;   // stack lstm to parser state
  Parameter p_H;   // head matrix for composition function
  Parameter p_D;   // dependency matrix for composition function
  Parameter p_R;   // relation matrix for composition function
  Parameter p_w2l; // word to LSTM input
  Parameter p_p2l; // POS to LSTM input
  Parameter p_t2l; // pretrained word embeddings to LSTM input
  Parameter p_ib;  // LSTM input bias
  Parameter p_cbias;        // composition function bias
  Parameter p_p2a;          // parser state to action
  Parameter p_action_start; // action bias
  Parameter p_abias;        // action bias
  Parameter p_buffer_guard; // end of buffer
  Parameter p_stack_guard;  // end of stack
  Parameter p_pass_guard;   // end of pass buffer
  Parameter p_W_satb;       // [attention] weight for stack top attending buffer
  Parameter p_bias_satb;    // [attention] bias for stack top attending buffer
} Parameters;

typedef struct Exprs {
  Expression biB;
  Expression W_satb;
  Expression bias_satb;
  Expression pbias;
  Expression H;
  Expression D;
  Expression R;
  Expression cbias;
  Expression S;
  Expression B;
  Expression P;
  Expression A;
  Expression ib;
  Expression w2l;
  Expression p2l;
  Expression t2l;
  Expression p2a;
  Expression abias;
  Expression action_start;
  vector<Expression> buffer;
  vector<Expression> pass;
  vector<Expression> stack;
  vector<Expression> word_emb;
  std::vector<BidirectionalLSTMLayer::Output> bilstm_outputs;
} Exprs;

static volatile bool requested_stop;

class LSTMParser {
public:
  bool DEBUG = false;
  cpyp::Corpus corpus;
  vector<unsigned> possible_actions;
  unordered_map<unsigned, vector<float>> pretrained;
  Options Opt;
  Sizes System_size;
  std::string transition_system;
  Model model, model_2nd;
  vector<Model> model_;

  bool use_pretrained; // True if use pretraiend word embedding

  unsigned kUNK;
  set<unsigned> training_vocab; // words available in the training corpus
  set<unsigned> singletons;

  Parameters params, params_2nd;
  vector<Parameters> params_;

  explicit LSTMParser();
  ~LSTMParser();

  // uni-model
  bool save_model(string model_file);
  bool load_model(string model_file, string dev_data_file = "");
  void set_options(Options opts);
  bool load(string model_file, string training_data_file,
            string word_embedding_file, string dev_data_file = "");
  bool load_2nd_model(string model_file);

  bool load_model(string model_file, Model &model, Parameters &params);
  bool load_n_models(vector<string> model_files);

  void get_dynamic_infos();
  bool init_dynet();
  bool setup_dynet(Model &model, Parameters &params);

  // bool has_path_to(int w1, int w2, const std::vector<bool>  dir_graph []);
  bool has_path_to(int w1, int w2, const std::vector<std::vector<bool>> &graph);

  bool has_path_to(int w1, int w2,
                   const std::vector<std::vector<string>> &graph);

  vector<unsigned> get_children(unsigned id, const vector<vector<bool>> graph);
  vector<unsigned> get_heads(unsigned id, const vector<vector<bool>> graph);
  vector<unsigned> get_update_order(unsigned id,
                                    const vector<vector<bool>> graph);

  bool update_ancestors(unsigned id, const vector<vector<bool>> graph,
                        TheirTreeLSTMBuilder &tree_lstm,
                        const vector<Expression> &word_emb,
                        LSTMBuilder &stack_lstm, vector<Expression> &stack,
                        const vector<int> &stacki, LSTMBuilder &pass_lstm,
                        vector<Expression> &pass, const vector<int> &passi,
                        LSTMBuilder &buffer_lstm, vector<Expression> &buffer,
                        const vector<int> &bufferi);

  bool IsActionForbidden(
      const string &a, unsigned bsize, unsigned ssize, unsigned root,
      const std::vector<std::vector<bool>>
          dir_graph, // const std::vector<bool>  dir_graph [],
      const std::vector<int> &stacki, const std::vector<int> &bufferi);
  std::vector<std::vector<string>>
  compute_heads(const std::vector<unsigned> &sent,
                const std::vector<unsigned> &actions);

  // for swap-based algorithm
  void apply_action(ComputationGraph *hg, LSTMBuilder &stack_lstm,
                    LSTMBuilder &buffer_lstm, LSTMBuilder &action_lstm,
                    TheirTreeLSTMBuilder &tree_lstm, vector<Expression> &buffer,
                    vector<int> &bufferi, vector<Expression> &stack,
                    vector<int> &stacki, vector<unsigned> &results,
                    unsigned action, const vector<string> &setOfActions,
                    const vector<unsigned> &sent, // sent with oovs replaced
                    const map<unsigned, std::string> &intToWords,
                    const Expression &cbias, const Expression &H,
                    const Expression &D, const Expression &R, string *rootword,
                    vector<vector<bool>> &graph,
                    const vector<Expression> &word_emb);

  void apply_action_2nd(ComputationGraph *hg,
                        // LSTMBuilder& stack_lstm,
                        // LSTMBuilder& buffer_lstm,
                        // LSTMBuilder& action_lstm,
                        // TheirTreeLSTMBuilder& tree_lstm,
                        vector<Expression> &buffer, const vector<int> &bufferi,
                        vector<Expression> &stack, const vector<int> &stacki,
                        // vector<unsigned>& results,
                        unsigned action, const vector<string> &setOfActions,
                        const vector<unsigned> &sent, // sent with oovs replaced
                        const map<unsigned, std::string> &intToWords,
                        const Expression &cbias, const Expression &H,
                        const Expression &D, const Expression &R,
                        string *rootword, const vector<vector<bool>> &graph,
                        const vector<Expression> &word_emb, Parameters &params);

  // for list-based algorithms
  void apply_action(ComputationGraph *hg, LSTMBuilder &stack_lstm,
                    LSTMBuilder &buffer_lstm, LSTMBuilder &action_lstm,
                    TheirTreeLSTMBuilder &tree_lstm, vector<Expression> &buffer,
                    vector<int> &bufferi, vector<Expression> &stack,
                    vector<int> &stacki, vector<unsigned> &results,
                    unsigned action, const vector<string> &setOfActions,
                    const vector<unsigned> &sent, // sent with oovs replaced
                    const map<unsigned, std::string> &intToWords,
                    const Expression &cbias, const Expression &H,
                    const Expression &D, const Expression &R, string *rootword,
                    vector<vector<bool>> &graph,
                    const vector<Expression> &word_emb, LSTMBuilder &pass_lstm,
                    vector<Expression> &pass, vector<int> &passi);

  void apply_action_2nd(ComputationGraph *hg,
                        // LSTMBuilder& stack_lstm,
                        // LSTMBuilder& buffer_lstm,
                        // LSTMBuilder& action_lstm,
                        // TheirTreeLSTMBuilder& tree_lstm,
                        vector<Expression> &buffer, const vector<int> &bufferi,
                        vector<Expression> &stack, const vector<int> &stacki,
                        // vector<unsigned>& results,
                        unsigned action, const vector<string> &setOfActions,
                        const vector<unsigned> &sent, // sent with oovs replaced
                        const map<unsigned, std::string> &intToWords,
                        const Expression &cbias, const Expression &H,
                        const Expression &D, const Expression &R,
                        string *rootword, const vector<vector<bool>> &graph,
                        const vector<Expression> &word_emb,
                        // LSTMBuilder& pass_lstm,
                        vector<Expression> &pass, const vector<int> &passi,
                        Parameters &params);

  std::vector<unsigned>
  log_prob_parser(ComputationGraph *hg,
                  const std::vector<unsigned> &raw_sent, // raw sentence
                  const std::vector<unsigned> &sent, // sent with oovs replaced
                  const std::vector<unsigned> &sentPos,
                  const std::vector<unsigned> &correct_actions,
                  const std::vector<string> &setOfActions,
                  const map<unsigned, std::string> &intToWords, double *right,
                  std::vector<std::vector<string>> &cand);

  std::vector<unsigned> log_prob_parser_ensemble(
      ComputationGraph *hg,
      const std::vector<unsigned> &raw_sent, // raw sentence
      const std::vector<unsigned> &sent,     // sent with oovs replaced
      const std::vector<unsigned> &sentPos,
      const std::vector<unsigned> &correct_actions,
      const std::vector<string> &setOfActions,
      const map<unsigned, std::string> &intToWords, double *right,
      std::vector<std::vector<string>> &cand);

  bool load_params(ComputationGraph *hg, Exprs &expr, Parameters &params,
                   const vector<unsigned> &raw_sent, // raw sentence
                   const vector<unsigned> &sent,     // sent with oovs replaced
                   const vector<unsigned> &sentPos);

  Expression score(Exprs &exprs, Parameters &params);

  std::vector<unsigned> log_prob_parser_ensemble_n(
      ComputationGraph *hg,
      const std::vector<unsigned> &raw_sent, // raw sentence
      const std::vector<unsigned> &sent,     // sent with oovs replaced
      const std::vector<unsigned> &sentPos,
      const std::vector<unsigned> &correct_actions,
      const std::vector<string> &setOfActions,
      const map<unsigned, std::string> &intToWords, double *right,
      std::vector<std::vector<string>> &cand, unsigned model_number);

  bool IsActionForbidden2(const string &a, unsigned bsize, unsigned ssize,
                          vector<int> stacki);

  int process_headless(std::vector<std::vector<string>> &hyp,
                       std::vector<std::vector<string>> &cand,
                       const std::vector<unsigned> &sent,
                       const std::vector<unsigned> &sentPos);

  void process_headless_search_all(const std::vector<unsigned> &sent,
                                   const std::vector<unsigned> &sentPos,
                                   const std::vector<string> &setOfActions,
                                   int n, int sent_len, int dir,
                                   map<int, double> *scores,
                                   map<int, string> *rels);

  void get_best_label(const std::vector<unsigned> &sent,
                      const std::vector<unsigned> &sentPos,
                      ComputationGraph *hg,
                      const std::vector<string> &setOfActions, int s0, int b0,
                      int sent_size, int dir, double *score, string *rel);

  static void signal_callback_handler(int /* signum */);

  void train(const std::string fname, const unsigned unk_strategy,
             const double unk_prob);

  void test(std::string test_data_file);

  void predict_dev();

  void predict(std::vector<std::vector<string>> &hyp,
               const std::vector<std::string> &words,
               const std::vector<std::string> &postags);

  void output_sdp(const vector<unsigned> &sentence, const vector<unsigned> &pos,
                  const vector<string> &sentenceUnkStrings,
                  const vector<vector<string>> &hyp);

  void output_sdp(const vector<unsigned> &sentence, const vector<unsigned> &pos,
                  const vector<string> &sentenceUnkStrings,
                  const vector<vector<string>> &hyp, const int nsent);

  void output_conll(const vector<unsigned> &sentence,
                    const vector<unsigned> &pos,
                    const vector<string> &sentenceUnkStrings,
                    // const map<unsigned, string>& intToWords,
                    // const map<unsigned, string>& intToPos,
                    const vector<vector<string>> &hyp);

  void output_conll(const vector<unsigned> &sentence,
                    const vector<unsigned> &pos,
                    const vector<string> &sentenceUnkStrings,
                    const vector<vector<string>> &hyp, const int nsent);

  map<string, double>
  evaluate(const std::vector<std::vector<std::vector<string>>> &refs,
           const std::vector<std::vector<std::vector<string>>> &hyps);
};

} //  namespace lstmsdparser

#endif //  end for __LSTMSDPARSER_PARSER_H__
