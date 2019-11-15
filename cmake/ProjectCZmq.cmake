include(ExternalProject)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
    set(CZMQ_CMAKE_COMMAND emcmake cmake)
else()
    set(CZMQ_CMAKE_COMMAND ${CMAKE_COMMAND})
endif()

ExternalProject_Add(czmq
        PREFIX ${CMAKE_BINARY_DIR}/deps
        SOURCE_DIR ${CMAKE_BINARY_DIR}/deps/src/czmq
        DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/deps
        DOWNLOAD_NO_PROGRESS 1
        DOWNLOAD_NAME czmq.tar.gz
        URL https://github.com/zeromq/czmq/archive/master.zip
        URL_HASH SHA256=27a2747a5289fbc46406cc39ba9e723f1aafc4d5935e0e6010cee10761c577b1
        BUILD_IN_SOURCE 1

        # GIT_REPOSITORY https://github.com/zeromq/czmq.git
        # GIT_TAG 1.0
        # UPDATE_COMMAND ""
        CMAKE_COMMAND ${CZMQ_CMAKE_COMMAND}
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DBUILD_SHARED_LIBS=OFF
        -DCMAKE_POSITION_INDEPENDENT_CODE=${DBUILD_SHARED_LIBS}
        -DCMAKE_BUILD_TYPE=Release
        #CONFIGURE_COMMAND ./autogen.sh COMMAND ./configure --prefix=<INSTALL_DIR> --with-libzmq=${CMAKE_BINARY_DIR}/deps
        #BUILD_COMMAND  make
        #INSTALL_COMMAND ""
        #CONFIGURE_COMMAND ./autogen.sh COMMAND ./configure --prefix=<INSTALL_DIR> --with-libzmq=${CMAKE_BINARY_DIR}/deps
        #BUILD_COMMAND  make
        #INSTALL_COMMAND ""
        LOG_DOWNLOAD 1
        LOG_CONFIGURE 1
        LOG_BUILD 1
        LOG_INSTALL 1
        )
        
# message("hbk0 ===> ${INSTALL_DIR}")   # result is null

ExternalProject_Get_Property(czmq INSTALL_DIR)

# message("hbk1 ===> ${INSTALL_DIR}")   # result is not null  :  build/deps/

add_library(CZmq STATIC IMPORTED)
set(CZMQ_LIBRARY ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}czmq${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CZMQ_INCLUDE_DIR ${INSTALL_DIR}/include)
file(MAKE_DIRECTORY ${CZMQ_INCLUDE_DIR})  # Must exist.
set_property(TARGET CZmq PROPERTY IMPORTED_LOCATION ${CZMQ_LIBRARY})
set_property(TARGET CZmq PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CZMQ_INCLUDE_DIR})
add_dependencies(CZmq czmq)
add_dependencies(czmq zmq)
unset(INSTALL_DIR)
unset(BINARY_DIR)

# message("hbk2 ===> ${INSTALL_DIR}") # result is null
