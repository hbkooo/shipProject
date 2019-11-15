py-RFCN中，低层利用caffe框架，但是有一部分layer层caffe中并没有，比如rpn、roi层，所以就利用python实现了这些层，就是py-RFCN文件夹下的lib库。
而如果要封装py-RFCN的话，不仅有用到低层的caffe，还用到了python写的lib库，所以这时就要需要把python写的lib库打包，不过好像没办法打包。

可以将rrpn_demo.cpp作为通用的caffe的C++检测步骤。

而在此方法中，rrpn_demo.cpp中，也是直接使用caffe的C++接口来进行检测，不过，在网络执行检测的过程中，一些层还使用了python程序，所以虽然包含了libcaffe.so库，但是在运行时还要包括python模块的路径，即用python写的层的模块：
export PYTHONPATH=$PYTHONPATH:/home/hbk/netModel/RRPN/caffe/python    ## 包含caffe编译生成的python接口，不含会报错：import caffe找不到
export PYTHONPATH=$PYTHONPATH:/home/hbk/netModel/RRPN/lib             ## 包含python程序写的网络的层的模块


执行 ./rrpn_demo 时折磨要死的错误：
File "/home/hbk/RRPN/lib/rotation/rproposal_layer.py", line 8, in <module>
    import caffe
  File "/home/hbk/RRPN/caffe-fast-rcnn/python/caffe/__init__.py", line 1, in <module>
    from .pycaffe import Net, SGDSolver, NesterovSolver, AdaGradSolver, RMSPropSolver, AdaDeltaSolver, AdamSolver
  File "/home/hbk/RRPN/caffe-fast-rcnn/python/caffe/pycaffe.py", line 11, in <module>
    import numpy as np
  File "/home/hbk/miniconda3/envs/python2/lib/python2.7/site-packages/numpy/__init__.py", line 142, in <module>
    from . import add_newdocs
  File "/home/hbk/miniconda3/envs/python2/lib/python2.7/site-packages/numpy/add_newdocs.py", line 13, in <module>
    from numpy.lib import add_newdoc
  File "/home/hbk/miniconda3/envs/python2/lib/python2.7/site-packages/numpy/lib/__init__.py", line 8, in <module>
    from .type_check import *
  File "/home/hbk/miniconda3/envs/python2/lib/python2.7/site-packages/numpy/lib/type_check.py", line 11, in <module>
    import numpy.core.numeric as _nx
  File "/home/hbk/miniconda3/envs/python2/lib/python2.7/site-packages/numpy/core/__init__.py", line 103, in <module>
    from numpy.testing._private.pytesttester import PytestTester
  File "/home/hbk/miniconda3/envs/python2/lib/python2.7/site-packages/numpy/testing/__init__.py", line 10, in <module>
    from unittest import TestCase
  File "/home/hbk/miniconda3/envs/python2/lib/python2.7/unittest/__init__.py", line 64, in <module>
    from .main import TestProgram, main
  File "/home/hbk/miniconda3/envs/python2/lib/python2.7/unittest/main.py", line 7, in <module>
    from . import loader, runner
  File "/home/hbk/miniconda3/envs/python2/lib/python2.7/unittest/runner.py", line 7, in <module>
    from .signals import registerResult
  File "/home/hbk/miniconda3/envs/python2/lib/python2.7/unittest/signals.py", line 2, in <module>
    import weakref
  File "/home/hbk/miniconda3/envs/python2/lib/python2.7/weakref.py", line 14, in <module>
    from _weakref import (
ImportError: cannot import name _remove_dead_weakref
terminate called after throwing an instance of 'boost::python::error_already_set'

错误原因：
编译caffe和make rrpn_demo.cpp时运行环境不一样：
错误：首先编译caffe时用的local python2，而在CMakeLists.txt中python路径包含的是/usr/include/python2.7/，这是系统的python路径，所以需要将此改为/home/hbk/miniconda3/envs/python2/include/python2.7/，即与编译caffe时一样的local python2。



修改完毕后又会报了另外一个错误，protobuf版本问题，说运行此程序需要高版本3.6.1的环境，而当前运行的protobuf版本为2.6.1，需要更新可能会可以运行：
错误原因：千万不要更新，这个错误的原因是因为编译caffe时的protobuf和运行rrpn_demo.cpp的protobuf版本不一致的原因。
    在caffe环境中我们必须统一ProtoBuffer的版本才可以避免各种不易排查的错误！！！！
    查看local python2中的pip list命令，看到protoc的版本为3.6.1，而在编译caffe时就是使用的local python2环境。在运行./rrpn_demo时查看protoc -      -version为2.6.1，所以在运行时会警告说要提高protobuf的版本。
解决方法：将python2中的protobuf卸载：pip uninstall protobuf，然后重新下载2.6.1的protobuf：pip install protobuf==2.6.1，然后重新编译caffe即可。





