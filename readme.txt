
source activate python2   # �������⻷��

# ����caffe
1, cd SHIP_SERVER/deps/caffe_ship/caffe_faster_rcnn : 
    in Makefile.config : 
        �޸�anaconda_home ��·����70��, ֱ�ӽ�miniconda3�޸�Ϊanaconda
    
    Ȼ��ֱ�� make -j8  
             make pycaffe
        
        ������� make error : /usr/include/boost/python/detail/wrap_python.hpp:50:23: fatal error: pyconfig.h: No such file or directory  
                solution : ==> export CPLUS_INCLUDE_PATH=/home/hbk/miniconda3/envs/python2/include/python2.7:$CPLUS_INCLUDE_PATH   # �޸�Ϊ���anaconda��װ��python2��λ��

    cd ../lib
    rm */*.so
    make
    cd rotation/
    cp rotate_polygon_nms.so librotate_polygon_nms.so
    
3, ���� ship_server

    in cmake/ProjectCaffeShip.cmake file : �޸�python·������װ��anaconda���⻷��
    
    cd SHIP_SERVER/
    mkdir build
    cd build
    cmake ..
    make -j8
    
    
4, run :  in build/
    export LD_LIBRARY_PATH=/path/to/SHIP_SERVER/deps/caffe_ship/lib:$LD_LIBRARY_PATH
    export LD_LIBRARY_PATH=/path/to/SHIP_SERVER/deps/caffe_ship/caffe_fast_rcnn/python:$LD_LIBRARY_PATH
    ./bin/verifier   # ����һ���ն�
    
    ./bin/router     # ������һ���ն�
    
    
5, make :
    /usr/bin/ld.gold: error: cannot find -lprotobuf
      sudo ln -s /usr/local/lib/libprotobuf.so /usr/lib/libprotobuf.so
    
    usr/local/lib/libopencv_freetype.so.3.2.0: error: undefined reference to 'FT_Set_Pixel_Sizes'
      in file : zmqServer/CMakeLIsts.txt    libTaskModel/CMakeLIsts.txt    libAlgorithm/CMakeLIsts.txt    libAlgorithm/Ship/CMakeLIsts.txt
        list(REMOVE_ITEM OpenCV_LIBS opencv_freetype) 
    
    
    
    
    
    uuid boost opencv glog protobuf 
    
    
    
    
    
    
    
    
    