cmake_minimum_required(VERSION 3.16)

# Set the project name
project(lstmsdparser-lib C CXX)

file(GLOB_RECURSE src
    ${CMAKE_CURRENT_LIST_DIR}/*.h
    ${CMAKE_CURRENT_LIST_DIR}/*.hpp

    ${CMAKE_CURRENT_LIST_DIR}/*.c
	${CMAKE_CURRENT_LIST_DIR}/*.cc
	${CMAKE_CURRENT_LIST_DIR}/*.cpp
)

file(GLOB_RECURSE public_headers
    ${CMAKE_CURRENT_LIST_DIR}/include/*.h
    ${CMAKE_CURRENT_LIST_DIR}/include/*.hpp
)

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/include
)

set(lstm_sdparser_VERSION "0.0.1")

add_library(lstmsdparser-lib
    STATIC ${src}
)

if(BACKEND MATCHES "^eigen$")
    set(DYNET_LIB "dynet")
elseif(BACKEND MATCHES "^cuda$")
    set(DYNET_LIB "gdynet")
else()
    message(SEND_ERROR "BACKEND must be eigen or cuda")
endif()

target_link_libraries(lstmsdparser-lib
    PUBLIC ${CMAKE_THREAD_LIBS_INIT}
    PUBLIC ${Boost_LIBRARIES}
    PUBLIC ${DL_LIBRARY}
    PUBLIC ${DYNET_LIB}
)

target_include_directories(lstmsdparser-lib
    PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include/"
    PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../../external/rapidjson/include"
    PUBLIC ${Boost_INCLUDE_DIRS}
    PUBLIC ${EIGEN3_INCLUDE_DIR}
    PUBLIC ${TH_INCLUDE_DIR}
)