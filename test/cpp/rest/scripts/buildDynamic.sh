#! /bin/bash

BASE_PHOTO_DIR="data/basePhoto"
IMAGE_LIST_NAME="image_list"
BASE_SUFFIX=".base"
TEST_PROGRAM_NAME="wuhanTest"
GETLIST_PROGRAM_NAME="getList.py"
PYTHON="python"
IMAGE_NUM=0
IMAGE_LIST_0=${IMAGE_LIST_NAME}_0${BASE_SUFFIX}
IMAGE_LIST_1=${IMAGE_LIST_NAME}_1${BASE_SUFFIX}

pre_check()
{
    if [ ! -e ${TEST_PROGRAM_NAME} ];then
        echo "Cannot find program ${TEST_PROGRAM_NAME}."
        exit 1
    fi

    if [ ! -e ${GETLIST_PROGRAM_NAME} ];then
        echo "Cannot find program ${GETLIST_PROGRAM_NAME}."
        exit 1
    fi
    echo "pre-check done!"
}

gen_base_image_list()
{
    LIST_NAME="${IMAGE_LIST_NAME}${BASE_SUFFIX}"
    $PYTHON ${GETLIST_PROGRAM_NAME} ${BASE_PHOTO_DIR} ${LIST_NAME}
    IMAGE_NUM=$(cat ${LIST_NAME}|wc -l)
    if [ ${IMAGE_NUM} -le 0 ];then
        echo "Cannot find image."
        exit 1
    fi
    echo "generate base image list done!"
}


divide_list()
{
    HALF=`expr ${IMAGE_NUM} / 2`
    ANOTHER=`expr ${IMAGE_NUM} - ${HALF}`
    head -n ${HALF}  ${IMAGE_LIST_NAME}${BASE_SUFFIX} > ${IMAGE_LIST_0}
    tail -n $ANOTHER ${IMAGE_LIST_NAME}${BASE_SUFFIX} > ${IMAGE_LIST_1}
    echo "Divide done!"
}

start_build_database()
{
    # "Usage: ./wuhanTest [image-list filename] [section size] [GPU id] [id offset] [write destination(0:file,1:redis)]"
    ./${TEST_PROGRAM_NAME} ${IMAGE_LIST_0} 0 0 0 1 &
    ./${TEST_PROGRAM_NAME} ${IMAGE_LIST_1} 0 1 0 1
    wait
    echo "build database done!"
}


print_help()
{   
    printf "Usage: ./buildDynamic.sh \n"
}

main()
{
   
    pre_check 
    gen_base_image_list 
    divide_list
    start_build_database
    exit 1
}

main