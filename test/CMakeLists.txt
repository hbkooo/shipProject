if(BUILD_TESTS)
    include_directories(${CMAKE_SOURCE_DIR}/src
                        ${BOOST_INCLUDE_DIR})	
    #add_executable(faceDataTest faceDataTest.cpp)
    #target_link_libraries(faceDataTest facedata pthread Boost::System Boost::Thread) 
    
    #add_executable(uuidTest uuidTest.cpp)
    #add_dependencies(uuidTest Boost::Filesystem)
    if (NOT BUILD_REST)
        add_subdirectory(cpp/zmq)
    else()
        add_subdirectory(cpp/rest)
    endif()
 
    # Unit test
    #add_test(NAME test_function 
	  #  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/nodejs/zmq/function_test.sh
	  #  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/nodejs/zmq)
    #add_test(test_faceData ${CMAKE_BINARY_DIR}/bin/faceDataTest)
endif(BUILD_TESTS) 
