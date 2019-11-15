file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

include_directories(${CMAKE_SOURCE_DIR}/src
                    ${BOOST_INCLUDE_DIR}
                    ${OCEAN_AI_INCLUDE_DIR}
                    ${CAFFE_INCLUDE_DIR}
                    /usr/local/cuda/include/)

IF(BUILD_CAFFE_CPU)
    add_definitions(-DCPU_ONLY)
ENDIF()

IF(BUILD_REST)
    add_definitions(-DUSE_REST)
ENDIF()

add_executable(wuhanTest ${SRC_LIST} ${HEADERS})
target_include_directories(wuhanTest PRIVATE ./)

target_link_libraries(wuhanTest Boost::System Boost::Filesystem common algorithm searchtool taskmodel)
add_dependencies(wuhanTest Caffe)

add_executable(genFileList scripts/genImageList.cpp)
target_link_libraries(genFileList Boost::Filesystem)
add_dependencies(genFileList boost)
