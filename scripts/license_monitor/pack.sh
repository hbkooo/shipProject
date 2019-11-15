#!/bin/bash

set -e

PROJECT_DIR=$(pwd)
RELEASE_DIR="${PROJECT_DIR}/release"
EXECUTABLE_DIR="${PROJECT_DIR}/build/bin"
EXECUTABLE_FILE="${EXECUTABLE_DIR}/verifierMonitor"
TARGET_DIR="${RELEASE_DIR}/verifierMonitor"
SCRIPT_PATH="${PROJECT_DIR}/scripts/license_monitor"
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

	if [ -e "verifierMonitor.tar.gz" ]; then
		rm zmqClient.tar.gz
	fi 

	if [ ! -e $EXECUTABLE_FILE ]; then
		echo "ERROR - can not find executable file ${EXECUTABLE_FILE}!";
		exit 1;
	fi

	if [ ! -e "${SCRIPT_PATH}/install.sh" ]; then
		echo "ERROR - can not find install scripts!";
		exit 1;
	fi

	if [ ! -e "${SCRIPT_PATH}/monitor.sh" ]; then
		echo "ERROR - can not find monitor.sh scripts!";
		exit 1;
	fi

	echo "Check files done!"
}

make_dir()
{
	mkdir $TARGET_DIR/scripts
	echo "Gennerate directories done!"
}

copy_files()
{
	cp $EXECUTABLE_FILE $TARGET_DIR
	cp ${SCRIPT_PATH}/monitor.sh $TARGET_DIR/scripts
	cp ${SCRIPT_PATH}/install.sh ${RELEASE_DIR}
}

compress()
{
	cd ${RELEASE_DIR}
	tar zcvf verifierMonitor.tar.gz verifierMonitor
	echo "Compress done!"
}


gen_md5()
{
	md5=$(md5sum verifierMonitor.tar.gz)
	md5=${md5:0:32}
	sed -i "s/MD5=0/MD5=${md5}/" install.sh
	echo "Generate md5 done!"
}
get_version()
{
	VERSION_MAJOR=0
	VERSION_MINOR=0
	VERSION_PATCH=1
}

gen_prog()
{
	echo "===Binary files===" >> install.sh
	base64 verifierMonitor.tar.gz >> install.sh
	PROGRAM_NAME="verifierMonitor_v${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}"
	mv install.sh ${PROGRAM_NAME}
	chmod u+x ${PROGRAM_NAME}

	echo "Generate ${PROGRAM_NAME} done!"
}

clean()
{
	if [ -e "${RELEASE_DIR}/verifierMonitor.tar.gz" ]; then
		rm -rf ${TARGET_DIR}
		rm $RELEASE_DIR/verifierMonitor.tar.gz
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
	copy_files
	compress
	gen_md5
	get_version		
	gen_prog
	clean
}

main $# $@
