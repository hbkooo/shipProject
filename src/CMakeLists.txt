
include_directories(${CMAKE_SOURCE_DIR}/src
                    ${PISTACHE_INCLUDE_DIR}
                    ${BOOST_INCLUDE_DIR}
                    ${LIBCRYPTO_INCLUDE_DIRS}
                    /usr/local/cuda/include/)

add_subdirectory(common)
add_subdirectory(libAlgorithm)
add_subdirectory(libTaskModel)

IF(BUILD_LISENCE)
    add_subdirectory(monitor)
ENDIF()

IF(BUILD_UPDATE)
    add_subdirectory(update)
ENDIF()

if (BUILD_REST)
    add_definitions(-DUSE_REST)
    add_subdirectory(restServer)
else()
    add_subdirectory(zmqServer)
endif ()


if(NOT EXISTS ${PROJECT_BINARY_DIR}/model)
    ADD_CUSTOM_TARGET(link_model ALL COMMAND ${CMAKE_COMMAND} -E create_symlink ${PROJECT_SOURCE_DIR}/model_encrypt
            ${PROJECT_BINARY_DIR}/model)
endif()

if(NOT EXISTS ${PROJECT_BINARY_DIR}/license)
    ADD_CUSTOM_TARGET(link_license ALL COMMAND ${CMAKE_COMMAND} -E create_symlink ${PROJECT_SOURCE_DIR}/license
            ${PROJECT_BINARY_DIR}/license)
endif()