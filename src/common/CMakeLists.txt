file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

add_library(common STATIC ${SRC_LIST} ${HEADERS})
#add_library(common SHARED ${SRC_LIST} ${HEADERS})
#target_link_libraries(common ${OpenCV_LIBS})
add_dependencies(common Boost::Filesystem)
