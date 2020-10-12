cmake_minimum_required (VERSION 3.0.2)
project (verifier)

# The project version number.
set(VERSION_MAJOR   0   CACHE STRING "Project major version number.")
set(VERSION_MINOR   3   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   2   CACHE STRING "Project patch version number.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)
message(STATUS "Build ${CMAKE_PROJECT_NAME} ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

add_definitions(-DVERSION_MAJOR=${VERSION_MAJOR})
add_definitions(-DVERSION_MINOR=${VERSION_MINOR})
add_definitions(-DVERSION_PATCH=${VERSION_PATCH})

# Cmake scripts directory
set(VERIFIER_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}/cmake" CACHE PATH "The path to the cmake directory")
list(APPEND CMAKE_MODULE_PATH ${VERIFIER_CMAKE_DIR})

# Options
OPTION(BUILD_CAFFE_CPU "Build without CUDA support." OFF)
OPTION(BUILD_OPENCV2 "Build with OpenCV2 support." OFF)
OPTION(BUILD_TESTS "Build Tests." ON)                # start test
OPTION(BUILD_TOOLS "Build Tools." OFF)
OPTION(BUILD_DOCS "Build documentation" OFF)
OPTION(BUILD_REST "Build rest server or zmq server." OFF)
OPTION(BUILD_LISENCE "Build using license" OFF) # If you use a dongle,BUILD_LISENCE set ON,and BUILD_RSA set OFF
OPTION(BUILD_RSA "Build RSA" OFF)
OPTION(BUILD_FAISS "Build with FAISS support" OFF)
OPTION(BUILD_UPDATE "Build with UPDATE" OFF)
OPTION(BUILD_SHIP "Build with ship detect support" ON)
OPTION(BUILD_SAR "Buils with sar detect support" ON)
OPTION(BUILD_YOLO "Build with YOLO detect support" ON)
enable_testing()

# set opencv path
#set(OpenCV_DIR "/home/hbk/software/opencv-3.4.0/build/")

# Include cmake scripts
include(CompilerSettings)
include(ProjectBoost)


if (NOT BUILD_CAFFE_CPU)
    include(ProjectNCCL)
endif ()

# caffe library in the deps/caffe_ship/caffe_fast_rcnn/
include(ProjectCaffeShip)

if (BUILD_SHIP)
    add_definitions(-DUSE_SHIP)
endif ()

if (BUILD_SAR)
    add_definitions(-DUSE_SAR)
endif ()

if (BUILD_YOLO)
    add_definitions(-DUSE_YOLO)
    include(ProjectYolo)
endif () 


# if (BUILD_RSA)
    #include(ProjectOpenssl)
# endif()

if (NOT BUILD_REST)
    include(ProjectZmq)
    include(ProjectCZmq)
else()
    include(ProjectPistache)
    include(ProjectCurlpp)
endif ()

include(ProjectCppNetlib)
if (BUILD_FAISS)
    include(ProjectFaiss)
endif()

# Set binary path
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_subdirectory (src)
add_subdirectory (tools)
add_subdirectory (test)
add_subdirectory (docs)

# Install
install(PROGRAMS ${CMAKE_BINARY_DIR}/bin/verifier DESTINATION bin)
install(FILES  ${CMAKE_BINARY_DIR}/deps/lib/libcaffe.so.1.0.0 DESTINATION lib)
install(FILES  ${CMAKE_BINARY_DIR}/deps/caffe_ssd_rc3/lib/libcaffe.so.1.0.0-rc3 DESTINATION lib)
install(DIRECTORY model DESTINATION /etc/verifier)
install(FILES config.json DESTINATION /etc/verifier)

# cpack config, make package
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME ${CMAKE_PROJECT_NAME})
set(CPACK_DEBIAN_PACKAGE_NAME ${CMAKE_PROJECT_NAME})
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_PACKAGE_CONTACT "support@xxx.cn")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libstdc++6, libopencv-dev(>=2.4.0), libopencv-dev(<=2.4.13)")
include(CPack)

