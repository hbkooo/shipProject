#! /bin/bash

IMAGE_LIST=0
IMAGE_PATH=0
IMAGE_NUM=0

function read_dir()
{
    for file in `ls $1` 
        do
            if [ -d $1"/"$file ]  
            then
                read_dir $1"/"$file
            else
                echo $1"/"$file >> "${IMAGE_LIST}"  #写入文件
                IMAGE_NUM=`expr $IMAGE_NUM + 1`
                if [ `expr ${IMAGE_NUM} % 1000` -eq 0 ];
                then
                    date
                    echo $IMAGE_NUM
                fi  
            fi
        done
}

function pre_check()
{
    IMAGE_PATH=$1
    IMAGE_LIST=$2
    if [ -e $IMAGE_LIST ];then
        rm "$IMAGE_LIST"
    fi
    touch "$IMAGE_LIST"
}

function main()
{
    echo "data path : ${1}"
    echo "image list : ${2}" 
    
    pre_check $1 $2
    read_dir $IMAGE_PATH
    echo "Total : ${IMAGE_NUM}"
}

main $@