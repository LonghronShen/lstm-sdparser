cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

if(TARGET lstmsdparser-lib)
    return()
endif()

# include(CMakeDetermineSystem)

# if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
#     if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "^[Xx]86$")
#         set(VCPKG_TARGET_TRIPLET "x86-windows-static" CACHE STRING "VCPKG_TARGET_TRIPLET" FORCE)
#     elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "^[Aa][Mm][Dd]64$")
#         set(VCPKG_TARGET_TRIPLET "x64-windows-static" CACHE STRING "VCPKG_TARGET_TRIPLET" FORCE)
#     elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "^[Aa][Rr][Mm]$")
#         set(VCPKG_TARGET_TRIPLET "arm-windows-static" CACHE STRING "VCPKG_TARGET_TRIPLET" FORCE)
#     elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "^[Aa][Rr][Mm]64$")
#         set(VCPKG_TARGET_TRIPLET "arm64-windows-static" CACHE STRING "VCPKG_TARGET_TRIPLET" FORCE)
#     else()
#         message(AUTHOR_WARNING "Failed to determine the system arch.")
#     endif()
#     message(STATUS "Using vcpkg for ${VCPKG_TARGET_TRIPLET}, ${CMAKE_HOST_SYSTEM_PROCESSOR}")
#     set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "CMAKE_TOOLCHAIN_FILE" FORCE)
# endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

set(project_name cnn)

project(${project_name} C CXX)

set(CMAKE_CXX_STANDARD 14 CACHE STRING "Set to 11 or 14 to enable C++11 or C++14 builds, respectively." FORCE)
set(CMAKE_C_STANDARD 11 CACHE STRING "Set to 99 or 11 to enable C99 or C11 builds, respectively." FORCE)
set_property(GLOBAL PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY CXX_EXTENSIONS OFF)

add_compile_definitions("__STDC_WANT_LIB_EXT1__=1")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${CMAKE_BINARY_DIR})

if(WIN32 OR WIN64)
    add_compile_definitions("WIN32_LEAN_AND_MEAN" "_CRT_SECURE_NO_WARNINGS" "NOMINMAX")
    if(MSVC)
        add_link_options("/LIBPATH:${CMAKE_BINARY_DIR}/bin")
        add_compile_options("/source-charset:utf-8" "/EHsc" "/Zc:threadSafeInit-")

        if(NOT CMAKE_BUILD_TYPE)
            add_compile_options("/MT")
        else()
            add_compile_options("/MTd")
        endif()

        set(CompilerFlags
            CMAKE_CXX_FLAGS
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL
            CMAKE_CXX_FLAGS_RELWITHDEBINFO
            CMAKE_C_FLAGS
            CMAKE_C_FLAGS_DEBUG
            CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_MINSIZEREL
            CMAKE_C_FLAGS_RELWITHDEBINFO)
        foreach(CompilerFlag ${CompilerFlags})
            string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
            set(${CompilerFlag} "${${CompilerFlag}}" CACHE STRING "msvc compiler flags" FORCE)
            message("MSVC flags: ${CompilerFlag}:${${CompilerFlag}}")
        endforeach()
    endif()
else()
    if(UNIX)
        if(APPLE)
            add_compile_options("-m64" "-fPIC" "-march=native")
            set(CMAKE_MACOSX_RPATH 1 CACHE STRING "CMAKE_MACOSX_RPATH" FORCE)
        else()
            add_compile_options("-fPIC" )
            if(CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
                add_compile_options("-m64" "-march=westmere")
            endif()
        endif()
    endif()
endif()

# set(BACKEND "eigen" CACHE STRING "BACKEND" FORCE)

include(external/InitializeExternalComponents.cmake)

add_subdirectory(src)
