cmake_minimum_required(VERSION 3.0)
project("WebCPP_Project")

set(SERVICE_COMPONENTS dynamodb)

set(CMAKE_CXX_STANDARD 11)

set(WINDOWS_BUILD ${MSVC})

if (WINDOWS_BUILD) # Set the location where CMake can find the installed libraries for the AWS SDK.
    string(REPLACE ";" "/aws-cpp-sdk-all;" SYSTEM_MODULE_PATH "${CMAKE_SYSTEM_PREFIX_PATH}/aws-cpp-sdk-all")
    list(APPEND CMAKE_PREFIX_PATH ${SYSTEM_MODULE_PATH})
endif ()

# AWS SDK를 찾아서 사용하기
find_package(AWSSDK REQUIRED COMPONENTS dynamodb)

if (WINDOWS_BUILD) 
     # Copy relevant AWS SDK for C++ libraries into the current binary directory for running and debugging.

     # set(BIN_SUB_DIR "/Debug") # if you are building from the command line you may need to uncomment this 
                                    # and set the proper subdirectory to the executables' location.

     AWSSDK_CPY_DYN_LIBS(SERVICE_COMPONENTS "" ${CMAKE_CURRENT_BINARY_DIR}${BIN_SUB_DIR})
endif ()

# 프로젝트의 실행 파일과 라이브러리 생성
add_executable(${PROJECT_NAME}
	main.cpp)

# 프로젝트의 실행 파일과 AWS SDK 라이브러리 링크
target_link_libraries(${PROJECT_NAME} 
	${AWSSDK_LINK_LIBRARIES}
)