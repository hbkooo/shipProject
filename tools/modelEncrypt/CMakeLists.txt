IF(BUILD_OPENCV2)
    find_package(OpenCV 2 REQUIRED)
ELSE()
    find_package(OpenCV REQUIRED)
ENDIF()

add_executable(modelEncrypt modelEncrypt.cpp)
target_link_libraries(modelEncrypt ${OpenCV_LIBS} auth)
target_include_directories(modelEncrypt PRIVATE ${CMAKE_SOURCE_DIR}/src ${BOOST_INCLUDE_DIR})
