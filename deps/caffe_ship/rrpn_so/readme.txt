py-RFCN�У��Ͳ�����caffe��ܣ�������һ����layer��caffe�в�û�У�����rpn��roi�㣬���Ծ�����pythonʵ������Щ�㣬����py-RFCN�ļ����µ�lib�⡣
�����Ҫ��װpy-RFCN�Ļ����������õ��Ͳ��caffe�����õ���pythonд��lib�⣬������ʱ��Ҫ��Ҫ��pythonд��lib��������������û�취�����

���Խ�rrpn_demo.cpp��Ϊͨ�õ�caffe��C++��ⲽ�衣

���ڴ˷����У�rrpn_demo.cpp�У�Ҳ��ֱ��ʹ��caffe��C++�ӿ������м�⣬������������ִ�м��Ĺ����У�һЩ�㻹ʹ����python����������Ȼ������libcaffe.so�⣬����������ʱ��Ҫ����pythonģ���·��������pythonд�Ĳ��ģ�飺
export PYTHONPATH=$PYTHONPATH:/home/hbk/netModel/RRPN/caffe/python    ## ����caffe�������ɵ�python�ӿڣ������ᱨ��import caffe�Ҳ���
export PYTHONPATH=$PYTHONPATH:/home/hbk/netModel/RRPN/lib             ## ����python����д������Ĳ��ģ��


ִ�� ./rrpn_demo ʱ��ĥҪ���Ĵ���
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

����ԭ��
����caffe��make rrpn_demo.cppʱ���л�����һ����
�������ȱ���caffeʱ�õ�local python2������CMakeLists.txt��python·����������/usr/include/python2.7/������ϵͳ��python·����������Ҫ���˸�Ϊ/home/hbk/miniconda3/envs/python2/include/python2.7/���������caffeʱһ����local python2��



�޸���Ϻ��ֻᱨ������һ������protobuf�汾���⣬˵���д˳�����Ҫ�߰汾3.6.1�Ļ���������ǰ���е�protobuf�汾Ϊ2.6.1����Ҫ���¿��ܻ�������У�
����ԭ��ǧ��Ҫ���£���������ԭ������Ϊ����caffeʱ��protobuf������rrpn_demo.cpp��protobuf�汾��һ�µ�ԭ��
    ��caffe���������Ǳ���ͳһProtoBuffer�İ汾�ſ��Ա�����ֲ����Ų�Ĵ��󣡣�����
    �鿴local python2�е�pip list�������protoc�İ汾Ϊ3.6.1�����ڱ���caffeʱ����ʹ�õ�local python2������������./rrpn_demoʱ�鿴protoc -      -versionΪ2.6.1������������ʱ�ᾯ��˵Ҫ���protobuf�İ汾��
�����������python2�е�protobufж�أ�pip uninstall protobuf��Ȼ����������2.6.1��protobuf��pip install protobuf==2.6.1��Ȼ�����±���caffe���ɡ�





