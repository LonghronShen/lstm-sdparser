# if(UNIX)
#     set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" CACHE STRING "CMAKE_FIND_LIBRARY_SUFFIXES" FORCE)
# endif()

find_package(Threads REQUIRED)

if(UNIX)
    find_package(DL REQUIRED)
endif()

if(WIN32)
    set(Boost_USE_STATIC_LIBS ON CACHE STRING "Boost_USE_STATIC_LIBS" FORCE)
    set(Boost_USE_STATIC_RUNTIME ON CACHE STRING "Boost_USE_STATIC_RUNTIME" FORCE)
endif()

# find a boost install with the libraries unit_test_framework
find_package(Boost 1.65.1 REQUIRED COMPONENTS serialization thread log log_setup system program_options filesystem coroutine locale regex unit_test_framework)
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

# look for Eigen
add_subdirectory(external/eigen)
add_subdirectory(external/dynet)