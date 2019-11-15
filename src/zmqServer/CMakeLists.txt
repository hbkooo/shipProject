file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

find_library(CAFFE_LIBRARY caffe ${CMAKE_BINARY_DIR}/deps)
find_library(CZMQ_LIBRARY czmq ${CMAKE_BINARY_DIR}/deps/lib)
find_library(UUID_LIBRARY uuid)

IF(BUILD_OPENCV2)
    find_package(OpenCV 2 REQUIRED)
ELSE()
    find_package(OpenCV REQUIRED)
ENDIF()

list(REMOVE_ITEM OpenCV_LIBS opencv_freetype)

find_package(BLAS REQUIRED)
find_package(LAPACK REQUIRED)
add_library(zmqserver STATIC ${SRC_LIST} ${HEADERS})
target_include_directories(zmqserver PRIVATE ${BOOST_INCLUDE_DIR} .. ${CAFFE_INCLUDE_DIR} ${OCEAN_AI_INCLUDE_DIR})
target_link_libraries(zmqserver algorithm ${OpenCV_LIBS} ${CAFFE_LIBRARY} ${CZMQ_LIBRARY} glog protobuf
        ${LIBZMQ_LIBRARIES} ${UUID_LIBRARY} ${BLAS_LIBRARIES} ${LAPACK_LIBRARIES})
add_dependencies(zmqserver algorithm)
add_dependencies(zmqserver CZmq)

if (BUILD_SHIP)
    target_link_libraries(zmqserver ship_algorithm)
    add_dependencies(zmqserver ship_algorithm)
endif ()

add_executable(verifier ZmqDealer.cpp)
target_include_directories(verifier PRIVATE ./ ${OCEAN_AI_INCLUDE_DIR}
        ${LIBCRYPTO_INCLUDE_DIRS})
target_link_libraries(verifier zmqserver  common taskmodel
        Boost::Filesystem) # auth
add_dependencies(verifier zmqserver)


add_executable(router ZmqRouter.cpp)
include_directories(${CZMQ_INCLUDE_DIR})
target_link_libraries(router ${CZMQ_LIBRARY} ${LIBZMQ_LIBRARIES})
target_link_libraries(router  common Boost::Filesystem) # auth
add_dependencies(router CZmq)


