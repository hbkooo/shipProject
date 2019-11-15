#!/bin/bash

PROGRAM_NAME="verifierMonitor"
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

}

post_install()
{
	rm -rf ./scripts
	rm ../$PACKAGE_NAME
	echo "INFO - Installation finished!"
}

setup_monitor()
{
	cp ./scripts/monitor.sh ./
	# cp ./scripts/crontabMonitor.sh ./
	# CRONTAB="crontabMonitor"
	# if [ ! -e ${CRONTAB} ];then
	# 	touch ${CRONTAB}
	# fi
	MONITOR="monitor"
	if [ ! -e ${MONITOR} ];then
		touch ${MONITOR}
	fi
	# echo "* * * * * /bin/bash $(pwd)/crontabMonitor.sh" > ${CRONTAB}
	# 备份其他定时任务
	crontab -l |grep -v "verifierMonitor" > ${MONITOR}
	echo "# the follow task is verfierMonitor crontab task, don't modify it please if you don't understand it" >> ${MONITOR}
	echo "# ************************************************************************************" >> ${MONITOR}
	step=2
	for (( i = 0; i < 60; i=(i+step) )); 
	do 
		echo "* * * * * sleep $i;/bin/bash $(pwd)/monitor.sh $(pwd)" >> ${MONITOR}
	done
	# /bin/bash $(pwd)/crontabMonitor.sh
	# crontab ${CRONTAB}
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
