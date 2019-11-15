#!/bin/bash

PROGRAM_NAME="verifier"
PACKAGE_NAME="${PROGRAM_NAME}.tar.gz"
MD5=0

check()
{

	if [ ! -e $PACKAGE_NAME ]; then
		echo "ERROR - can not find package file ${PACKAGE_NAME} !";
		exit 1;
	fi
	md5=$(md5sum $PACKAGE_NAME)
	md5=${md5:0:32}
	if [ "$MD5" != "$md5" ];then
		echo "check md5 failed! exit now!";
		exit 1;
	fi
}

extract()
{	
	echo "INFO - extracting ...."
	line=`awk '/===Binary files===/{print NR}' $0 | tail -n1` 
	line=`expr $line + 1`
	tail -n +$line $0 | base64 -d >${PACKAGE_NAME}

	check

	tar zxf $PACKAGE_NAME
	cd $PROGRAM_NAME
	chmod a+x ./*
	echo "INFO - unpack finished!"
}

install()
{

	echo "INFO - installing ..."

	sudo cp lib/libcaffe.so.1.0.0 /usr/lib

	echo "INFO - copy caffe lib finished!"

	sh ./scripts/install_deps.sh

	echo "INFO - install dependence finished!" 

	sh ./scripts/install_cuda.sh
	sh ./scripts/install_runtime.sh

}

post_install()
{
	rm -rf ./deps
	rm -rf ./scripts
	rm -rf ./lib
	rm ../$PACKAGE_NAME

	if [ -e "cuda-repo-ubuntu1404_8.0.61-1_amd64.deb" ]; then
		rm cuda-repo-ubuntu1404_8.0.61-1_amd64.deb	
	fi

	echo "INFO - Install runtime finished!"

	echo "INFO - Installation finished!"
	echo "INFO - Before start the program, please insert the Encryption Key provided by sofetware Suppliers."
}

setup_monitor()
{
	cp ./scripts/monitor.sh ./
	MONITOR="monitor"
	if [ ! -e ${MONITOR} ];then
		touch ${MONITOR}
	fi
	echo "* * * * * /bin/bash $(pwd)/monitor.sh $(pwd)" > ${MONITOR}
	crontab ${MONITOR}
	echo "INFO - Setup monitor done!"
}

main()
{
	extract
	install
	setup_monitor
	post_install
}

main
exit 0
