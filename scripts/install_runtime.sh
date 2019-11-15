#!/bin/bash

PROGRAM_HOME="."
RUNTIME_NAME="aksusbd_7.54-1_i386.deb"

install()
{
	sudo apt-get install -y libjpeg8:i386 \
						libsm6:i386 \
						libxext6:i386\
						libxrender1:i386

	sudo dpkg -i --force-architecture ${PROGRAM_HOME}/deps/${RUNTIME_NAME}
}

main()
{
	if [ -z "$(dpkg -l | grep aksusbd)" ];then
		install
	fi
}

main