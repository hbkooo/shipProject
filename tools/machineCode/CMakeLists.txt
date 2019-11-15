cmake_minimum_required(VERSION 3.5)
project(machineCode)

set(CMAKE_CXX_STANDARD 11)

include_directories(${LIBCRYPTO_INCLUDE_DIRS}
        ${BOOST_INCLUDE_DIR})
find_library(LIBCRYPTO_LIBRARIES crypto ${CMAKE_BINARY_DIR}/deps/lib)

#find_package(Threads REQUIRED)
#if(THREADS_HAVE_PTHREAD_ARG)
#    set_property(TARGET DetectLib PROPERTY COMPILE_OPTIONS "-pthread")
#    set_property(TARGET DetectLib PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
#endif()

add_executable(machineCode main.cpp)

target_link_libraries(machineCode auth common ${OpenCV_LIBS} ${LIBCRYPTO_LIBRARIES} Boost::Filesystem -pthread -ldl)
target_include_directories(machineCode PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_dependencies(machineCode auth)
add_dependencies(machineCode common)
add_dependencies(machineCode Openssl)
add_dependencies(machineCode Boost::Filesystem)
#if(CMAKE_THREAD_LIBS_INIT)
#    target_link_libraries(machineCode ${CMAKE_THREAD_LIBS_INIT})
#endif()
