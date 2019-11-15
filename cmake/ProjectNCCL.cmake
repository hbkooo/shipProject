include(ExternalProject)
include(GNUInstallDirs)

ExternalProject_Add(nccl
        PREFIX ${CMAKE_BINARY_DIR}/deps
        DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/deps
        DOWNLOAD_NO_PROGRESS 1
        DOWNLOAD_NAME nccl.zip
        URL https://github.com/NVIDIA/nccl/archive/master.zip
        URL_HASH SHA256=c2d51e9a178d7b3ad42b2b6d1196ac56d0d831fe573913733ec7561893b80a37

        # GIT_REPOSITORY https://github.com/NVIDIA/nccl.git
        # UPDATE_COMMAND ""

        BUILD_IN_SOURCE 1
        # CMAKE_COMMAND ${NCCL_CMAKE_COMMAND}
        # CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        # Build shared lib but suitable to be included in a static lib.
        # -DBUILD_SHARED_LIBS=ON
        # -DCMAKE_POSITION_INDEPENDENT_CODE=${BUILD_SHARED_LIBS}
        # -DCMAKE_BUILD_TYPE=Release
        CONFIGURE_COMMAND ""
        BUILD_COMMAND make BUILDDIR=<INSTALL_DIR>
        INSTALL_COMMAND ""

        LOG_CONFIGURE 1
        LOG_BUILD 1
        LOG_INSTALL 1
        LOG_DOWNLOAD 1
        )

# Create nccl imported library
ExternalProject_Get_Property(nccl INSTALL_DIR)
add_library(Nccl STATIC IMPORTED)
set(NCCL_LIBRARY ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}nccl_static${CMAKE_STATIC_LIBRARY_SUFFIX})
set(NCCL_INCLUDE_DIR ${INSTALL_DIR}/include)
file(MAKE_DIRECTORY ${NCCL_INCLUDE_DIR})  # Must exist.
set_property(TARGET Nccl PROPERTY IMPORTED_LOCATION ${NCCL_LIBRARY})
set_property(TARGET Nccl PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${NCCL_INCLUDE_DIR})
add_dependencies(Nccl nccl)
unset(INSTALL_DIR)

