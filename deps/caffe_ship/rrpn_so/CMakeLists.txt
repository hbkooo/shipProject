#This part is used for compile faster_rcnn_demo.cpp
#这里是版本要求，根据自己项目而定，我用了默认的

cmake_minimum_required (VERSION 2.8)

#我们的工程的名字

project (rrpn_demo)

#添加我们要生成的可执行文件的名字，以及相应的源码文件

add_executable(rrpn_demo rrpn_demo.cpp)

find_library(TEST rotate_polygon_nms ${CMAKE_SOURCE_DIR}/../lib/rotation)
message(${TEST})


#这里添加这个faster_rcnn_demo.cpp所依赖的头文件路径
#首先是Caffe目录的include
#其次是用了gpu_nms.cu，所以也要添加相应的头文件gpu_nms.hpp在py-faster-rcnn根目录下的lib/nms中
#下面就是几个Caffe的依赖项，包括Python
#值得注意的是boost/python。hpp的头文件路径也要加入
#还有opencv的路径，cuda路径，线性代数库路径相应的都要添加
include_directories ( ${CMAKE_SOURCE_DIR}/../caffe_fast_rcnn/include
    ${CMAKE_SOURCE_DIR}/../caffe_fast_rcnn/cmake
    ${CMAKE_SOURCE_DIR}/../lib/rotation
    ${CMAKE_SOURCE_DIR}/../lib/nms
    #/usr/local/include 
    /usr/local/include/opencv2/
    /usr/local/cuda/include 
    /usr/include/python2.7/      # wrap_python.hpp找不到问题
    #/home/hbk/miniconda3/envs/python2/include/python2.7/  
    # /usr/include
    )
    
    
#在这里值得说一下，target_link_libraries 语句中 生成的目标是可执行文件 后面紧跟的得是动态库的完整路径，否则会出错
#我一开始用的是Link_directoreis然后在后面直接加入了动态库的路径，结果他一直报错，提示找不到库，ORZ真是跪了，
#所以要么在这里直接加入完整路径或者同通过另一条语句find_library()，这种方式也比较好，直接去指定路径查找，返回相应的绝对路径也可避免直接添加地址的问题
#gpu_nms.so 在py-faster-rcnn根目录下的lib\nms中，直接make就会生成这个so文件，不过不使用这个，因为这个是相当于水平边界框的nms，我们要使用的是
# 有角度的边界框，RRPN中自己写了一个rotate_polygon_nms，所以我们要使用rotate_polygon_nms.so文件

#LINK_DIRECTORIES(/home/hbk/netModel/RRPN_new/lib/rotation)

target_link_libraries(rrpn_demo ${CMAKE_SOURCE_DIR}/../caffe_fast_rcnn/build/lib/libcaffe.so
    ${CMAKE_SOURCE_DIR}/../lib/rotation/librotate_polygon_nms.so
    ${CMAKE_SOURCE_DIR}/../lib/nms/gpu_nms.so 
    /usr/local/lib/libopencv_highgui.so 
    /usr/local/lib/libopencv_core.so 
    /usr/local/lib/libopencv_imgproc.so 
    /usr/local/lib/libopencv_imgcodecs.so
    /usr/local/lib/libglog.so 
    /usr/lib/x86_64-linux-gnu/libboost_system.so    
    
    #/usr/lib/x86_64-linux-gnu/libboost_python-py27.so
    
    /usr/lib/x86_64-linux-gnu/libpython2.7.so       # wrap_python.hpp找不到问题
    #/home/hbk/miniconda3/envs/python2/lib/libpython2.7.so
    )
    
#SET (SRC_LIST rrpn_demo.cpp)    
#add_library (rrpn_demo SHARED ${SRC_LIST})    

    
    
    
