if(POLICY CMP0072)
  cmake_policy(SET CMP0072 NEW)
endif()

find_package(Threads REQUIRED)

if(UNIX)
    find_package(DL REQUIRED)
endif()

if(WIN32)
    set(Boost_USE_STATIC_LIBS ON CACHE STRING "Boost_USE_STATIC_LIBS" FORCE)
    set(Boost_USE_STATIC_RUNTIME ON CACHE STRING "Boost_USE_STATIC_RUNTIME" FORCE)
    set(BOOST_PYTHON_STATIC_LIB ON CACHE STRING "BOOST_PYTHON_STATIC_LIB" FORCE)
endif()

# find a boost install with the libraries unit_test_framework
find_package(Boost 1.58 REQUIRED COMPONENTS serialization thread log log_setup system program_options filesystem coroutine locale regex unit_test_framework)
if(Boost_FOUND)
    message(STATUS "** Boost Include: ${Boost_INCLUDE_DIR}")
    message(STATUS "** Boost Libraries Directory: ${Boost_LIBRARY_DIRS}")
    message(STATUS "** Boost Libraries: ${Boost_LIBRARIES}")
    include_directories(${Boost_INCLUDE_DIRS})
else()
    if(WIN32)
        message(FATAL_ERROR "Plase check your vcpkg settings or global environment variables for the boost library.")
    else()
        add_subdirectory(external/boost-cmake)
    endif()
endif()

# looking for Eigen
find_package(Eigen3 QUIET)
if(NOT EIGEN3_FOUND)
    set(EIGEN_TEST_CXX11 ON CACHE STRING "EIGEN_TEST_CXX11" FORCE)
    add_subdirectory(external/eigen EXCLUDE_FROM_ALL)
    set(EIGEN3_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/eigen" CACHE STRING "EIGEN3_INCLUDE_DIR" FORCE)
endif()

find_package(DyNet QUIET)
if(NOT TH_INCLUDE_DIR)
    add_subdirectory(external/dynet EXCLUDE_FROM_ALL)
    set(TH_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/dynet" CACHE STRING "TH_INCLUDE_DIR" FORCE)
endif()

if(WIN32)
    include_directories(external/unistd_h)
endif()