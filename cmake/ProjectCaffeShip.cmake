# caffe path
find_library(CAFFE_LIBRARY caffe ${CMAKE_SOURCE_DIR}/deps/caffe_ship/caffe_fast_rcnn/build/lib)
set(CAFFE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/deps/caffe_ship/caffe_fast_rcnn/include)

# python path
set(PYTHON_LIBRARY /home/hbk/miniconda3/envs/python2/lib/libpython2.7.so)
set(PYTHON_INCLUDE_DIR /home/hbk/miniconda3/envs/python2/include/python2.7)

# user-defined python layers and caffe python module
set(PYTHONPATH ${CMAKE_SOURCE_DIR}/deps/caffe_ship/lib ${CMAKE_SOURCE_DIR}/deps/caffe_ship/caffe_fast_rcnn/python)

# rotation
set(ROTATION_LIBRARY ${CMAKE_SOURCE_DIR}/deps/caffe_ship/lib/rotation/librotate_polygon_nms.so )
#set(ROTATION_LIBRARY ${CMAKE_SOURCE_DIR}/deps/caffe_ship/lib/nms/gpu_nms.so)
set(ROTATION_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/deps/caffe_ship/lib/rotation )
#        ${CMAKE_SOURCE_DIR}/deps/caffe_ship/lib/nms)


message(STATUS "CAFFE_LIBRARY ${CAFFE_LIBRARY}")
message(STATUS "CAFFE_INCLUDE_DIR ${CAFFE_INCLUDE_DIR}")
message(STATUS "PYTHON_LIBRARY ${PYTHON_LIBRARY}")
message(STATUS "PYTHON_INCLUDE_DIR ${PYTHON_INCLUDE_DIR}")
message(STATUS "PYTHONPATH ${PYTHONPATH}")
message(STATUS "ROTATION_LIBRARY ${ROTATION_LIBRARY}")
message(STATUS "ROTATION_INCLUDE_DIR ${ROTATION_INCLUDE_DIR}")
message(STATUS "CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR}")
message(STATUS "OCEAN_AI_INCLUDE_DIR ${OCEAN_AI_INCLUDE_DIR}")


