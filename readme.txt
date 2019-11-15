
source activate python2   # 开启虚拟环境

# 编译caffe
1, cd SHIP_SERVER/deps/caffe_ship/caffe_faster_rcnn : 
    in Makefile.config : 
        修改anaconda_home 的路径：70行, 直接将miniconda3修改为anaconda
    
    然后直接 make -j8  
             make pycaffe
        
        如果出错 make error : /usr/include/boost/python/detail/wrap_python.hpp:50:23: fatal error: pyconfig.h: No such file or directory  
                solution : ==> export CPLUS_INCLUDE_PATH=/home/hbk/miniconda3/envs/python2/include/python2.7:$CPLUS_INCLUDE_PATH   # 修改为你的anaconda安装的python2的位置

    cd ../lib
    rm */*.so
    make
    cd rotation/
    cp rotate_polygon_nms.so librotate_polygon_nms.so
    
3, 编译 ship_server

    in cmake/ProjectCaffeShip.cmake file : 修改python路径到安装的anaconda虚拟环境
    
    cd SHIP_SERVER/
    mkdir build
    cd build
    cmake ..
    make -j8
    
    
4, run :  in build/
    export LD_LIBRARY_PATH=/path/to/SHIP_SERVER/deps/caffe_ship/lib:$LD_LIBRARY_PATH
    export LD_LIBRARY_PATH=/path/to/SHIP_SERVER/deps/caffe_ship/caffe_fast_rcnn/python:$LD_LIBRARY_PATH
    ./bin/verifier   # 开启一个终端
    
    ./bin/router     # 开启另一个终端
    
    
5, make :
    /usr/bin/ld.gold: error: cannot find -lprotobuf
      sudo ln -s /usr/local/lib/libprotobuf.so /usr/lib/libprotobuf.so
    
    usr/local/lib/libopencv_freetype.so.3.2.0: error: undefined reference to 'FT_Set_Pixel_Sizes'
      in file : zmqServer/CMakeLIsts.txt    libTaskModel/CMakeLIsts.txt    libAlgorithm/CMakeLIsts.txt    libAlgorithm/Ship/CMakeLIsts.txt
        list(REMOVE_ITEM OpenCV_LIBS opencv_freetype) 
    
    
    
    
    
    uuid boost opencv glog protobuf 
    
    
    
    
    
    
    
    
    