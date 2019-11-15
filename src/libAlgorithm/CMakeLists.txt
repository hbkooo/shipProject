file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

if (NOT BUILD_SAR)
    # remove the source of SAR
    file(GLOB SAR_SRC sar_detect.cpp)
    file(GLOB SAR_HEADERS sar_detect.h)
    list(REMOVE_ITEM SRC_LIST ${SAR_SRC} )
    list(REMOVE_ITEM HEADERS ${SAR_HEADERS} )
endif ()

if (NOT BUILD_YOLO)
    # remove the source of YOLO
    file(GLOB YOLO_SRC yolo_detect.cpp)
    file(GLOB YOLO_HEADERS yolo_detect.h darknet.h)
    list(REMOVE_ITEM SRC_LIST ${YOLO_SRC})
    list(REMOVE_ITEM HEADERS ${YOLO_HEADERS})
endif ()

find_library(CAFFE_LIBRARY caffe ${CMAKE_BINARY_DIR}/deps/lib)

IF(BUILD_CAFFE_CPU)
    add_definitions(-DCPU_ONLY)
ENDIF()

IF(BUILD_OPENCV2)
    find_package(OpenCV 2 REQUIRED)
ELSE()
    find_package(OpenCV REQUIRED)
ENDIF()

list(REMOVE_ITEM OpenCV_LIBS opencv_freetype)

if (BUILD_SHIP)
    add_subdirectory(Ship)
endif ()

add_library(algorithm STATIC ${SRC_LIST} ${HEADERS} )
target_link_libraries(algorithm ${OpenCV_LIBS} ${CAFFE_LIBRARY} glog protobuf)
target_include_directories(algorithm PRIVATE ${BOOST_INCLUDE_DIR} ${CAFFE_INCLUDE_DIR} .. ${OCEAN_AI_INCLUDE_DIR})

if (BUILD_YOLO)
    target_link_libraries(algorithm  ${DARKNET_LIBRARY})
    target_include_directories(algorithm PRIVATE ${DARKNET_INCLUDE_DIR})
endif ()

