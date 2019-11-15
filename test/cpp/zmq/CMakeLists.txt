file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

add_executable(zmqClient ${SRC_LIST} ${HEADERS} result.cpp)

find_library(CZMQ_LIBRARY czmq ${CMAKE_BINARY_DIR}/deps/lib)
find_library(UUID_LIBRARY uuid)

message(STATUS TEST : )
message(STATUS CZMQ_LIBRARY : ${CZMQ_LIBRARY})
message(STATUS UUID_LIBRARY : ${UUID_LIBRARY})
message(STATUS LIBZMQ_LIBRARIES : ${LIBZMQ_LIBRARIES})
message(STATUS common : ${common})
message(STATUS CZMQ_INCLUDE_DIR : ${CZMQ_INCLUDE_DIR})

include_directories(${CZMQ_INCLUDE_DIR})
target_link_libraries(zmqClient ${CZMQ_LIBRARY} ${LIBZMQ_LIBRARIES} ${UUID_LIBRARY})
target_link_libraries(zmqClient common)
#add_dependencies(zmqClient CZmq)

#message("hbk in test/cpp/zmq/CMakeLists.txt   CZMQ_INCLUDE_DIR :  ==> ${CZMQ_INCLUDE_DIR}")
