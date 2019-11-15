include(ExternalProject)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
    set(ZMQ_CMAKE_COMMAND emcmake cmake)
else()
    set(ZMQ_CMAKE_COMMAND ${CMAKE_COMMAND})
endif()

ExternalProject_Add(zmq
        PREFIX ${CMAKE_BINARY_DIR}/deps
        SOURCE_DIR ${CMAKE_BINARY_DIR}/deps/src/zmq
        DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/deps
        DOWNLOAD_NO_PROGRESS 1
        DOWNLOAD_NAME libzmq.zip
        URL https://github.com/zeromq/libzmq/archive/master.zip
        URL_HASH SHA256=bcc7e82b6cac91be0ce21baf82ad5fb80e06f17f84cf854df43c848126a1388d
        BUILD_IN_SOURCE 1

        # GIT_REPOSITORY https://github.com/zeromq/libzmq.git
        # GIT_TAG 1.0
        # UPDATE_COMMAND ""
        #CMAKE_COMMAND ${ZMQ_CMAKE_COMMAND} -Wno-dev
        #CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        #-DBUILD_SHARED_LIBS=OFF
        #-DCMAKE_POSITION_INDEPENDENT_CODE=${DBUILD_SHARED_LIBS}
        #-DCMAKE_BUILD_TYPE=Release

        CONFIGURE_COMMAND ./autogen.sh COMMAND ./configure --prefix=<INSTALL_DIR> --without-libsodium --without-documentation
        BUILD_COMMAND make
        INSTALL_COMMAND make install
        LOG_DOWNLOAD 1
        LOG_CONFIGURE 1
        LOG_BUILD 1
        LOG_INSTALL 1
        )

ExternalProject_Get_Property(zmq INSTALL_DIR)
add_library(Zmq STATIC IMPORTED)
set(LIBZMQ_LIBRARIES ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}zmq${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LIBZMQ_INCLUDE_DIRS ${INSTALL_DIR}/include)
file(MAKE_DIRECTORY ${LIBZMQ_INCLUDE_DIRS})  # Must exist.
set_property(TARGET Zmq PROPERTY IMPORTED_LOCATION ${LIBZMQ_LIBRARIES})
set_property(TARGET Zmq PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${LIBZMQ_INCLUDE_DIRS})
add_dependencies(Zmq zmq)
unset(INSTALL_DIR)
unset(BINARY_DIR)

