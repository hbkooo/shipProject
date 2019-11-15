file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

find_library(CAFFE_LIBRARY caffe ${CMAKE_BINARY_DIR}/deps)

IF(BUILD_CAFFE_CPU)
    add_definitions(-DCPU_ONLY)
    add_definitions(-DUSE_CAFFE_CPU)
ENDIF()

if (BUILD_LISENCE)
    add_definitions(-DUSE_LICENSE)
endif()

IF(BUILD_FAISS)
    add_definitions(-DUSE_FAISS)
ENDIF()

IF(BUILD_FAISS_GPU)
    add_definitions(-DUSE_FAISS_GPU)
ENDIF()

IF(BUILD_REST)
    add_definitions(-DUSE_REST)
ELSE()
    find_library(CZMQ_LIBRARY czmq ${CMAKE_BINARY_DIR}/deps/lib)
    find_library(UUID_LIBRARY uuid)
ENDIF()


IF(BUILD_OPENCV2)
    find_package(OpenCV 2 REQUIRED)
ELSE()
    find_package(OpenCV REQUIRED)
ENDIF()

list(REMOVE_ITEM OpenCV_LIBS opencv_freetype)

add_library(taskmodel STATIC ${SRC_LIST} ${HEADERS})
target_include_directories(taskmodel PRIVATE ${BOOST_INCLUDE_DIR} .. ${CAFFE_INCLUDE_DIR} ${OCEAN_AI_INCLUDE_DIR} ${FAISS_INCLUDE_DIR})
target_link_libraries(taskmodel  algorithm ${OpenCV_LIBS} ${CAFFE_LIBRARY} common glog protobuf ${DLIB_LIBRARIES}) # auth

if (BUILD_REST)
    target_link_libraries(taskmodel Pistache)
    add_dependencies(taskmodel Pistache)
else ()
    target_link_libraries(taskmodel ${CZMQ_LIBRARY} ${LIBZMQ_LIBRARIES} ${UUID_LIBRARY})
    add_dependencies(taskmodel CZmq)
endif()

if(NOT BUILD_FAISS_GPU)
    include_directories(${FAISS_INCLUDE_DIR})
    target_link_libraries(taskmodel blas lapack -fopenmp)

    if (BUILD_FAISS)
        target_link_libraries(taskmodel Faiss::CPU)
    endif()
elseif(BUILD_FAISS_GPU)
    add_definitions(-DCUDA_VERSION=${CUDA_VERSION_STRING})
    include_directories(${FAISS_INCLUDE_DIR} ${CUDA_INCLUDE_DIRS})
    target_link_libraries(taskmodel lapack ${CUDA_LIBRARIES} ${CUDA_CUBLAS_LIBRARIES} -fopenmp)

    if (BUILD_FAISS)
        target_link_libraries(taskmodel Faiss::GPU Faiss::CPU)
    endif()
endif()

add_dependencies(taskmodel algorithm)
#add_dependencies(taskmodel Openssl)

if (BUILD_SHIP)
    target_link_libraries(taskmodel ship_algorithm)
    add_dependencies(taskmodel ship_algorithm)
endif ()

