#! /bin/bash
TopK=20
PER=1000000
DIM=512
DATA_PATH="data/fixdata/"
FILE_NUM=20
CHUNK_SIZE=2500000

QUERY_PROGRAM="query"
BUILD_PROGRAM="buildStatic.sh"

pre_check()
{
    if [ ! -e ${QUERY_PROGRAM} ];then 
        echo "Cannot find ${QUERY_PROGRAM}."
        exit 1
    fi

    if [ ! -e ${BUILD_PROGRAM} ];then 
        echo "Cannot find ${BUILD_PROGRAM}."
        exit 1
    fi

    if [ ! -e ${DATA_PATH}/bmap.txt ];then 
        echo "Cannot find bmap.txt."
        exit 1
    fi

    if [ ! -e ${DATA_PATH}/qmap.txt ];then 
        echo "Cannot find qmap.txt."
        exit 1
    fi

    if [ ! -e ${DATA_PATH}/query.dat ];then 
        echo "Cannot find query.dat."
        exit 1
    fi

    if [ ! -e ${DATA_PATH}/1.dat ];then 
        echo "Cannot find base feature data."
        exit 1
    fi
}

set_param()
{
    FILE_NUM=$(ls ${DATA_PATH}/[0-9]*.dat | wc -l)
    echo "File number : ${FILE_NUM} "
}

build_database()
{
    echo "start build dataset ...."
    bash ${BUILD_PROGRAM} query
    echo "build query set done!"
}

gen_md5()
{
    RESULT_FILE="result.csv"
    if [ -e ${RESULT_FILE} ];then
        md5sum ${RESULT_FILE} | tee md5.txt -a
    fi
}

main()
{
    build_database    
    pre_check
    set_param
    mpirun -np 3 ./${QUERY_PROGRAM} --outK=${TopK} --per=${PER} --dim=${DIM} --datapath=${DATA_PATH} --fileNum=${FILE_NUM} --chunksize=${CHUNK_SIZE}
    wait
    gen_md5
}

main