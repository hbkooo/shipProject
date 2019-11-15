if(BUILD_TOOLS)
    add_subdirectory(modelEncrypt)
    add_subdirectory(machineCode)

    if (BUILD_LISENCE)
        if(BUILD_RSA)
            add_subdirectory(rsa)
        else()
            add_subdirectory(lockertool)
        endif()
    endif()
endif(BUILD_TOOLS)

