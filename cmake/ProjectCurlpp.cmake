include(ExternalProject)
include(GNUInstallDirs)
if (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
    set(CURLPP_CMAKE_COMMAND emcmake cmake)
else()
    set(CURLPP_CMAKE_COMMAND ${CMAKE_COMMAND})
endif()
ExternalProject_Add(curlpp
        PREFIX ${CMAKE_BINARY_DIR}/deps
        DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/deps
        DOWNLOAD_NO_PROGRESS 1
        DOWNLOAD_NAME curlpp-v0.8.1.tar.gz
        URL https://codeload.github.com/jpbarrette/curlpp/tar.gz/v0.8.1
        URL_HASH SHA256=97e3819bdcffc3e4047b6ac57ca14e04af85380bd93afe314bee9dd5c7f46a0a

        # GIT_REPOSITORY https://github.com/NVIDIA/nccl.git
        # UPDATE_COMMAND ""

        BUILD_IN_SOURCE 1
        # CMAKE_COMMAND ${NCCL_CMAKE_COMMAND}
        # CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        # Build shared lib but suitable to be included in a static lib.
        -DBUILD_SHARED_LIBS=OFF
        -DCMAKE_POSITION_INDEPENDENT_CODE=${BUILD_SHARED_LIBS}
        # -DCMAKE_BUILD_TYPE=Release
        CMAKE_COMMAND ${CURLPP_CMAKE_COMMAND}
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        # -DBUILD_SHARED_LIBS=OFF
        # -DCMAKE_POSITION_INDEPENDENT_CODE=${DBUILD_SHARED_LIBS}
        # BUILD_COMMAND make BUILDDIR=<INSTALL_DIR>
        # INSTALL_COMMAND ""

        LOG_CONFIGURE 1
        LOG_BUILD 1
        LOG_INSTALL 1
        LOG_DOWNLOAD 1
        )

# Create nccl imported library
ExternalProject_Get_Property(curlpp INSTALL_DIR)
add_library(Curlpp STATIC IMPORTED)
set(CURLPP_LIBRARY ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}curlpp${CMAKE_STATIC_LIBRARY_SUFFIX})
message(STATUS "CURLPP_LIBRARY:  ${CURLPP_LIBRARY}")
set(CURLPP_INCLUDE_DIR ${INSTALL_DIR}/include/curlpp)
file(MAKE_DIRECTORY ${CURLPP_INCLUDE_DIR})  # Must exist.
message(STATUS "CURLPP_INCLUDE_DIR:  ${CURLPP_INCLUDE_DIR}")
set_property(TARGET Curlpp PROPERTY IMPORTED_LOCATION ${CURLPP_LIBRARY})
set_property(TARGET Curlpp PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CURLPP_INCLUDE_DIR})
add_dependencies(Curlpp curlpp)
unset(INSTALL_DIR)

