#!/bin/bash

set -e

PROJECT_DIR=$(pwd)
RELEASE_DIR="${PROJECT_DIR}/release"
EXECUTABLE_DIR="${PROJECT_DIR}/build/bin"
EXECUTABLE_FILE="${EXECUTABLE_DIR}/verifier"
ENCRYPT_PROGRAM="${EXECUTABLE_DIR}/modelEncrypt"
MODEL_PATH="${PROJECT_DIR}/model"
ENCRYPT_MODEL_PATH="${RELEASE_DIR}/model"
TARGET_DIR="${RELEASE_DIR}/verifier"
SCRIPT_PATH="${PROJECT_DIR}/scripts"
CAFFE_LIB="${PROJECT_DIR}/build/deps/lib/libcaffe.so.*"
RUNTIME_FILE="${PROJECT_DIR}/deps/aksusbd_7.54-1_i386.deb"
CUDNN_FILE="${PROJECT_DIR}/deps/cudnn-8.0-linux-x64-v5.1.tar"
ENVELOPE_DIR="/home/sh/workspace/sentinel/Linux/VendorTools/Envelope"
BUILD_TYPE=0 # 0 is debug, 1 is release
VERSION_MAJOR=0
VERSION_MINOR=0
VERSION_PATCH=0

pre_check()
{

	if [ -e ${RELEASE_DIR} ]; then
		rm -rf ${RELEASE_DIR}
	fi

	if [ ! -d ${TARGET_DIR} ]; then
		mkdir -p ${TARGET_DIR}
	fi

	cd ${TARGET_DIR}

	if [ -e "verifier.tar.gz" ]; then
		rm verifier.tar.gz
	fi 

	if [ ! -e $EXECUTABLE_FILE ]; then
		echo "ERROR - can not find executable file ${EXECUTABLE_FILE}!";
		exit 1;
	fi

	if [ $BUILD_TYPE == 1 -a ! -e $ENCRYPT_PROGRAM ]; then
		echo "ERROR - can not find executable file ${ENCRYPT_PROGRAM}!";
		exit 1;
	fi
	
	if [ -f $MODEL_PATH ]; then
		echo "ERROR - can not find model files!";
		exit 1;
	fi

	if [ ! -e $CAFFE_LIB ]; then
		echo "ERROR - can not find caffe libarary!";
		exit 1;
	fi

	if [ ! -e "${SCRIPT_PATH}/install_deps.sh" ]; then
		echo "ERROR - can not find install dependence scripts!";
		exit 1;
	fi

	if [ ! -e "${SCRIPT_PATH}/install_cuda.sh" ]; then
		echo "ERROR - can not find install cuda scripts!";
		exit 1;
	fi

	if [ ! -e "${SCRIPT_PATH}/install.sh" ]; then
		echo "ERROR - can not find install scripts!";
		exit 1;
	fi

	if [ ! -e "${SCRIPT_PATH}/install_runtime.sh" ]; then
		echo "ERROR - can not find install runtime scripts!";
		exit 1;
	fi

	if [ ! -e "${RUNTIME_FILE}" ]; then
		echo "ERROR - can not find runtime file ${RUNTIME_FILE}!";
		exit 1;
	fi

	if [ ! -e "${CUDNN_FILE}" ]; then
		echo "ERROR - can not find cudnn file ${CUDNN_FILE}!";
		exit 1;
	fi

	echo "Check files done!"
}

make_dir()
{
	mkdir ${ENCRYPT_MODEL_PATH}
	mkdir $TARGET_DIR/lib
	mkdir $TARGET_DIR/deps
	mkdir $TARGET_DIR/scripts
	echo "Gennerate directories done!"
}

encrypt_model()
{
	cd ${PROJECT_DIR}
	for model_file in ${MODEL_PATH}/*.caffemodel
	do 
		./build/bin/modelEncrypt $model_file ${ENCRYPT_MODEL_PATH}
	done
	echo "Encrypt model done!"	
}

encrypt_program()
{
	if [ -f ${ENVELOPE_DIR} ];
	then
		echo "INFO - Cannot find encrypt , skip encrypt."
	else
		echo "======Start Encrypt======"
		cd ${ENVELOPE_DIR}
		./linuxenv -c:envconfig.cfgx ${EXECUTABLE_FILE} ${EXECUTABLE_FILE} &&
		if [ $? != 0 -a $? != 252 ];then
			echo "Encryption failed ! Exit now."
			exit 1;
		fi
		cd ${PROJECT_DIR} 
		echo "======End Encrypt======"
	fi

}

copy_files()
{
	cp $EXECUTABLE_FILE $TARGET_DIR
	cp $CAFFE_LIB $TARGET_DIR/lib
	cp -R $MODEL_PATH $TARGET_DIR
	if [ $BUILD_TYPE == 1 ]; then
		cp -R $ENCRYPT_MODEL_PATH $TARGET_DIR 
	fi

	cp ${SCRIPT_PATH}/install_deps.sh $TARGET_DIR/scripts
	cp ${SCRIPT_PATH}/install_cuda.sh $TARGET_DIR/scripts
	cp ${SCRIPT_PATH}/install_runtime.sh $TARGET_DIR/scripts
	cp ${SCRIPT_PATH}/monitor.sh $TARGET_DIR/scripts

	cp ${RUNTIME_FILE} ${TARGET_DIR}/deps
	cp ${CUDNN_FILE} ${TARGET_DIR}/deps
	cp ${SCRIPT_PATH}/install.sh ${RELEASE_DIR}
}

compress()
{
	cd ${RELEASE_DIR}
	tar zcvf verifier.tar.gz verifier
	echo "Compress done!"
}


gen_md5()
{
	md5=$(md5sum verifier.tar.gz)
	md5=${md5:0:32}
	sed -i "s/MD5=0/MD5=${md5}/" install.sh
	echo "Generate md5 done!"
}
get_version()
{
	CMKAELITS_PATH="../CMakeLists.txt"
	VERSION_MAJOR=$(cat ${CMKAELITS_PATH} | grep "set(VERSION_MAJOR" | awk '{print $2}')
	VERSION_MINOR=$(cat ${CMKAELITS_PATH} | grep "set(VERSION_MINOR" | awk '{print $2}')
	VERSION_PATCH=$(cat ${CMKAELITS_PATH} | grep "set(VERSION_PATCH" | awk '{print $2}')
}

gen_prog()
{
	echo "===Binary files===" >> install.sh
	base64 verifier.tar.gz >> install.sh
	PROGRAM_NAME="verifier_v${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}"
	mv install.sh ${PROGRAM_NAME}
	chmod u+x ${PROGRAM_NAME}

	echo "Generate ${PROGRAM_NAME} done!"
}

clean()
{
	if [ -e "${RELEASE_DIR}/verifier.tar.gz" ]; then
		rm -rf ${TARGET_DIR}
		rm $RELEASE_DIR/verifier.tar.gz
	fi
	
	if [ -d ${ENCRYPT_MODEL_PATH} ];then
		rm -r ${ENCRYPT_MODEL_PATH}
	fi
}

main()
{
	if [ $1 != 0 ];then
		BUILD_TYPE=1
		ENVELOPE_DIR=$2
	fi
	if [ $BUILD_TYPE == 1 ];
	then
		echo "Build type : release."
	else
		echo "Build type : debug"
	fi

	pre_check
	make_dir
	if [ $BUILD_TYPE == 1 ];then
		encrypt_program
		encrypt_model
	fi
	copy_files
	compress
	gen_md5
	get_version		
	gen_prog
	clean
}

main $# $@
