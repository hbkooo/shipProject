cmake_minimum_required(VERSION 3.5)
project(rsa)

set(CMAKE_CXX_STANDARD 11)

include_directories(${LIBCRYPTO_INCLUDE_DIRS}
        ${BOOST_INCLUDE_DIR})
find_library(LIBCRYPTO_LIBRARIES crypto ${CMAKE_BINARY_DIR}/deps/lib)

add_executable(rsa main.cpp)
#target_link_libraries(rsa crypto)
target_link_libraries(rsa auth common ${LIBCRYPTO_LIBRARIES} dl Boost::Filesystem)
target_include_directories(rsa PRIVATE ${CMAKE_SOURCE_DIR}/src ${BOOST_INCLUDE_DIR})
add_dependencies(rsa auth)
add_dependencies(rsa common)
add_dependencies(rsa Openssl)
add_dependencies(rsa Boost::Filesystem)
