IF(BUILD_OPENCV2)
    find_package(OpenCV 2 REQUIRED)
ELSE()
    find_package(OpenCV REQUIRED)
ENDIF()

add_executable(lockerTool lockerTool.cpp)
target_link_libraries(lockerTool common auth)
target_include_directories(lockerTool PRIVATE ${CMAKE_SOURCE_DIR}/src)

