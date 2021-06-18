conda activate imgMan

1, make polyiou:
	swig -c++ -python polyiou.i
	python setup.py build_ext --inplace


2, 测试通用检测：
	python darknet.py
	
3, 测试SAR检测:
	python sar.py
	
4, 测试遥感舰船检测：
	python ship.py

注：（默认不用修改）
	在运行上面的测试脚本之前，首先需要把测试数据列表准备好（在testData中不同种类的数据中的test.txt文件）
	然后并且需要修改在这三个脚本文件中开始位置的模型路径和数据路径。
