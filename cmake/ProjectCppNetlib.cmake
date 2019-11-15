# /*********************************************************
# *   Copyright (C) 2017 All rights reserved.
# *
# * File Name: ProjectCppNetlib.cmake
# * Purpose: N/A
# * Creation Date: 2018-01-13 09:48:09 +0800
# * Created By: mumumumuxiansheng@gmail.com
# *********************************************************/

include(ExternalProject)

ExternalProject_Add(cpp-netlib
        PREFIX ${CMAKE_BINARY_DIR}/deps
        DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/deps
        # SOURCE_DIR ${CMAKE_SOURCE_DIR}/deps/cpp-netlib

        # GIT_REPOSITORY https://github.com/cpp-netlib/cpp-netlib.git
        # GIT_TAG 1.0
        # GIT_SUBMODULES ""

        # DOWNLOAD_NAME cpp-netlib.tar.gz
        DOWNLOAD_NO_PROGRESS 1
        URL http://downloads.cpp-netlib.org/0.13.0/cpp-netlib-0.13.0-rc1.tar.gz
        URL_HASH MD5=075981ef6f1c4f9d7674ae47aacd203a
        BUILD_IN_SOURCE 1

        CMAKE_COMMAND ""
        # LOG_CONFIGURE 1
        # BINARY_DIR "buildcodecp
        BUILD_COMMAND make
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DBOOST_ROOT=<INSTALL_DIR>
        -DBOOST_INCLUDEDIR=${BOOST_INCLUDE_DIR}
        -DBOOST_LIBRARYDIR=${BOOST_LIB_DIR}
        -DBoost_NO_SYSTEM_PATHS=ON
        -DBoost_ADDITIONAL_VERSIONS=1.65.1
        -DCPP-NETLIB_BUILD_EXAMPLES=OFF
        # -DUri_BUILD_TESTS=OFF
        -DCPP-NETLIB_BUILD_TESTS=OFF
        -DCPP-NETLIB_ENABLE_HTTPS=OFF
        BUILD_COMMAND ""
        # INSTALL_COMMAND ""
        LOG_CONFIGURE 1
        LOG_BUILD 1
        LOG_DOWNLOAD 1
        LOG_INSTALL 1
        )

# Create imported library
ExternalProject_Get_Property(cpp-netlib INSTALL_DIR)
add_library(CppNetlib-url STATIC IMPORTED)
add_library(CppNetlib-client STATIC IMPORTED)
add_library(CppNetlib-server STATIC IMPORTED)

set(CPPNETLIB_URL_LIBRARY ${INSTALL_DIR}/lib/libcppnetlib-uri.a) 
set(CPPNETLIB_CLIENT_LIBRARY ${INSTALL_DIR}/lib/libcppnetlib-client-connections.a)
set(CPPNETLIB_SERVER_LIBRARY ${INSTALL_DIR}/lib/libcppnetlib-server-parsers.a)

set(CPPNETLIB_INCLUDE_DIR ${INSTALL_DIR}/include)

file(MAKE_DIRECTORY ${CPPNETLIB_INCLUDE_DIR})  # Must exist.

set_property(TARGET CppNetlib-url PROPERTY IMPORTED_LOCATION ${CPPNETLIB_URL_LIBRARY})
set_property(TARGET CppNetlib-url PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CPPNETLIB_INCLUDE_DIR})
add_dependencies(CppNetlib-url cpp-netlib)

set_property(TARGET CppNetlib-client PROPERTY IMPORTED_LOCATION ${CPPNETLIB_CLIENT_LIBRARY})
set_property(TARGET CppNetlib-client PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CPPNETLIB_INCLUDE_DIR})
add_dependencies(CppNetlib-client cpp-netlib)

set_property(TARGET CppNetlib-server PROPERTY IMPORTED_LOCATION ${CPPNETLIB_SERVER_LIBRARY})
set_property(TARGET CppNetlib-server PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CPPNETLIB_INCLUDE_DIR})
add_dependencies(CppNetlib-server cpp-netlib)
add_dependencies(cpp-netlib boost)
unset(INSTALL_DIR)

