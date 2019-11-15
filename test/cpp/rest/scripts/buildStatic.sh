#! /bin/bash

BASE_PHOTO_DIR="data/basePhoto"
BASE_FEATURE_DIR="data/baseFeature"
QUERY_PHOTO_DIR="data/queryPhoto"
QUERY_FEATURE_DIR="data/queryFeature"
IMAGE_LIST_NAME="image_list"
BASE_SUFFIX=".base"
QUERY_SUFFIX=".query"
IMAGE_LIST_0=${IMAGE_LIST_NAME}_0${BASE_SUFFIX}
IMAGE_LIST_1=${IMAGE_LIST_NAME}_1${BASE_SUFFIX}
TEST_PROGRAM_NAME="wuhanTest"
GETLIST_PROGRAM_NAME="getList.py"
PYTHON="python"
GENLIST_PROGRAM_NAME="genFileList"
RUN_GEN_LIST="./${GENLIST_PROGRAM_NAME}"
# RUN_GEN_LIST="${PYTHON} {GETLIST_PROGRAM_NAME}"
IMAGE_NUM=0
HALF=0
BLOCK_SIZE=0
TYPE=0
IFDIVIDE=0

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
    
    if [ ! -e ${GENLIST_PROGRAM_NAME} ];then
        echo "Cannot find program ${GENLIST_PROGRAM_NAME}."
        exit 1
    fi

    if [ `ls ${BASE_FEATURE_DIR} | wc -l` -ne 0 ];then
        rm ${BASE_FEATURE_DIR}/*
    fi
    
    if [ `ls ${QUERY_FEATURE_DIR} | wc -l ` -ne 0 ];then 
        rm ${QUERY_FEATURE_DIR}/*
    fi

    echo "pre-check done!"
}

gen_base_image_list()
{
    LIST_NAME="${IMAGE_LIST_NAME}${BASE_SUFFIX}"
    ${RUN_GEN_LIST} ${BASE_PHOTO_DIR} ${LIST_NAME}
    IMAGE_NUM=$(cat ${LIST_NAME}|wc -l)
    if [ ${IMAGE_NUM} -le 0 ];then
        echo "Cannot find image."
        exit 1
    fi
    echo "generate base image list done!"
}

gen_query_image_list()
{
    LIST_NAME="${IMAGE_LIST_NAME}${QUERY_SUFFIX}"
    ${PYTHON} ${GETLIST_PROGRAM_NAME} ${QUERY_PHOTO_DIR} ${LIST_NAME}
    IMAGE_NUM=$(cat ${LIST_NAME}|wc -l)
    if [ ${IMAGE_NUM} -le 0 ];then
        echo "Cannot find image."
        exit 1
    fi
    echo "generate query image list done!"    
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
    if [ ${TYPE} == "base" ];then
        if [ ${IFDIVIDE} == "y" ];then
            ./${TEST_PROGRAM_NAME} ${IMAGE_LIST_0} ${BLOCK_SIZE} 0 0 0 &
            ./${TEST_PROGRAM_NAME} ${IMAGE_LIST_1} ${BLOCK_SIZE} 1 ${HALF} 0
        else
            ./${TEST_PROGRAM_NAME} ${IMAGE_LIST_NAME}${BASE_SUFFIX} ${BLOCK_SIZE} 0 0 0
        fi
    elif [ ${TYPE} == "query" ];then
        ./${TEST_PROGRAM_NAME} ${IMAGE_LIST_NAME}${QUERY_SUFFIX} ${BLOCK_SIZE} 0 0 0
    else
        print_help
        exit 1
    fi
    echo "build database done!"
}

start_generate_list()
{
    if [ ${TYPE} == "base" ];then
        gen_base_image_list
        if [ ${IFDIVIDE} == "y" ];then
            divide_list
        fi
    elif [ ${TYPE} == "query" ];then
        gen_query_image_list
    else
        print_help
        exit 1
    fi
    echo "list done"
}

prepare_data()
{
    FIXED_DATA_DIR="data/fixdata"
    # cd ${BASE_PHOTO_DIR}/..
    if [ ! -e ${FIXED_DATA_DIR} ];then
        mkdir -p ${FIXED_DATA_DIR}
    fi

    if [ ${TYPE} == "base" ];then
        if [ `ls ${FIXED_DATA_DIR} | wc -l` -ne 0 ];then 
            rm ${FIXED_DATA_DIR}/*
        fi
        if [ ${IFDIVIDE} == 'y' ];then 
            cat ${BASE_FEATURE_DIR}/id_name_list_0 ${BASE_FEATURE_DIR}/id_name_list_1 > ${FIXED_DATA_DIR}/bmap.txt 
        else 
            cat ${BASE_FEATURE_DIR}/id_name_list_0 > ${FIXED_DATA_DIR}/bmap.txt 
        fi
        i=1
        for file in ` ls -rt ${BASE_FEATURE_DIR} | grep 0.fmem`
        do
            echo $file
            cp ${BASE_FEATURE_DIR}/${file} ${FIXED_DATA_DIR}/${i}.dat
            i=`expr $i + 1`
        done

        for file in ` ls -rt ${BASE_FEATURE_DIR} | grep 1.fmem`
        do
            echo $file
            cp ${BASE_FEATURE_DIR}/${file} ${FIXED_DATA_DIR}/${i}.dat
            i=`expr $i + 1`
        done

    elif [ ${TYPE} == "query" ];then
        cp ${QUERY_FEATURE_DIR}/id_name_list_0 ${FIXED_DATA_DIR}/qmap.txt
        cp ${QUERY_FEATURE_DIR}/0_0.fmem ${FIXED_DATA_DIR}/query.dat
    else
        print_help
        exit 1
    fi
}

print_help()
{   
    printf "Usage: ./buildStatic.sh [dataset type] [GPUs] [block size]\n"
    printf "dataset type   : 'base' for base photo, 'query' for query photo. \n"
    printf "use GPUs       : '0' to use 1 GPU, '1' to use 2 GPUs. DEFAULT: 0\n"
    printf "block size     : number of features for each file. DEFAULT: 2500000\n"
}

set_paremeter()
{
    TYPE="base"
    IFDIVIDE='n'
    BLOCK_SIZE=2500000
    if [ $1 -lt 1 ];then
        print_help
        exit 1
    fi

    if [ $1 -eq 1 ];then
        TYPE=$2
    fi

    if [ $1 -eq 2 ];then
        TYPE=$2
        if [ $3 -eq 1 ];then
            IFDIVIDE="y"
        fi
    fi

    if [ $1 -eq 3 ];then
        TYPE=$2
        if [ $3 -eq 1 ];then
            IFDIVIDE="y"
        fi
        BLOCK_SIZE=$4
    fi

    echo "set parameter done!"
}

output_param()
{
    echo "========================================================"
    echo "data set type                     : ${TYPE}"
    echo "if use 2 GPUs to build database   : ${IFDIVIDE}"
    echo "feature number for each file      : ${BLOCK_SIZE}"
    echo "========================================================"
    
}

main()
{
    set_paremeter $@
    output_param

    echo "所有数据将被覆盖，输入y继续，输入n终止。"
    read tmp
    if [ $tmp != "y" ];then
        exit 1
    fi
    pre_check 
    start_generate_list 
    start_build_database
    wait
    prepare_data
}

main $# $@ # $#是参数个数 $@是参数列表