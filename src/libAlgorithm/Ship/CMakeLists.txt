file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")


IF(BUILD_OPENCV2)
    find_package(OpenCV 2 REQUIRED)
ELSE()
    find_package(OpenCV REQUIRED)
ENDIF()

list(REMOVE_ITEM OpenCV_LIBS opencv_freetype)

add_library(ship_algorithm STATIC ${SRC_LIST} ${HEADERS} )

# caffe and other library
target_link_libraries(ship_algorithm ${OpenCV_LIBS} ${CAFFE_LIBRARY} glog protobuf)
target_include_directories(ship_algorithm PRIVATE ${BOOST_INCLUDE_DIR} ${CAFFE_INCLUDE_DIR})

#add_dependencies(ship_algorithm Caffe_ship)

# roatation path
target_link_libraries(ship_algorithm ${ROTATION_LIBRARY})   # donnot add this code
target_include_directories(ship_algorithm PRIVATE ${ROTATION_INCLUDE_DIR})

# python path
target_include_directories(ship_algorithm PRIVATE ${PYTHON_INCLUDE_DIR})
target_link_libraries(ship_algorithm ${PYTHON_LIBRARY})

# other python layers lib
#target_include_directories(ship_algorithm PRIVATE ${PYTHONPATH})
include_directories(BEFORE ${PYTHONPATH})


###################################
##########    rrpn   ##############
###################################
# 1. caffe include path and library
# 2. rotation model
# 3. python path
# 4. python layers
# 4. caffe model of python
###################################




