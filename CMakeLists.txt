cmake_minimum_required(VERSION 3.10)
project(callbacks)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftrack-macro-expansion=0")

set(PATH_TO_SMSDK "C:/Users/Stephan/CLionProjects/Job_SMSDK")
set(PIP_DIR ${PATH_TO_SMSDK})
set(SMBRICKS_INCLUDES ${PATH_TO_SMSDK}/include/SMBricks)
set(SMBRICKS_LIB_DIR ${PATH_TO_SMSDK}/lib)
set(SM_BIN_DIR "C:/Users/Stephan/CLionProjects/Job_SMwin64")
find_package(PIP REQUIRED)

file(GLOB_RECURSE CPPS "mmwave/*.cpp")
file(GLOB_RECURSE HEADERS "mmwave/*.h")
file(GLOB_RECURSE CPPS2 "mmwave2/*.cpp")
file(GLOB_RECURSE HEADERS2 "mmwave2/*.h")

include_directories(${PIP_INCLUDES} ${SMBRICKS_INCLUDES} mmwave)
link_directories(${SMBRICKS_LIB_DIR})

add_executable(MMWFunctional MMWFunctional.cpp ${CPPS} ${HEADERS} ${CPPS2} ${HEADERS2} ${CMOCS} ${CCM})
target_link_libraries(MMWFunctional ${PIP_LIBRARY} ${PIP_CRYPT_LIBRARY} SMBricks_shared)

include(IncludeGoogleTests.cmake)
add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
message("-- googletest_BINARY_DIR: ${googletest_BINARY_DIR}")
message("-- googletest_SOURCE_DIR: ${googletest_SOURCE_DIR}")
message("-- gtest_SOURCE_DIR: ${gtest_SOURCE_DIR}")
message("-- gmock_SOURCE_DIR: ${gmock_SOURCE_DIR}")

include_directories("${gtest_SOURCE_DIR}/include" "${gmock_SOURCE_DIR}/include")

# Message example
add_executable(MessageExample MessageExample.cpp ${CPPS} ${HEADERS} ${CPPS2} ${HEADERS2} ${CMOCS} ${CCM})
target_link_libraries(MessageExample ${PIP_LIBRARY} ${PIP_CRYPT_LIBRARY} SMBricks_shared gtest gmock_main)
add_test(NAME MessageExample_test COMMAND MessageExample)

# My example
add_executable(example_my example_my.cpp)
target_link_libraries(example_my gtest gmock_main)
add_test(NAME example_my_test COMMAND example_my)

add_executable(simple simple_test.cpp ${CMOCS} ${CCM})
target_link_libraries(simple ${PIP_LIBRARY} ${PIP_CRYPT_LIBRARY} SMBricks_shared gtest gmock_main)
add_test(NAME simple_test COMMAND simple)

add_executable(MMWReceive ReceiveMMWDataTest.cpp ${CPPS} ${HEADERS} ${CMOCS} ${CCM})
target_link_libraries(MMWReceive ${PIP_LIBRARY} ${PIP_CRYPT_LIBRARY} SMBricks_shared gtest gmock_main)
add_test(NAME MMWReceiveTest COMMAND MMWReceive)

add_executable(SMBlockTest SMBlockTest.cpp ${CPPS} ${HEADERS} ${CPPS2} ${HEADERS2} ${CMOCS} ${CCM}
        plugin/MMWaveDemoInputMOC.h
        plugin/MMWaveDemoInputMOC.cpp
        plugin/MMWaveDemoCollection.h)
target_link_libraries(SMBlockTest ${PIP_LIBRARY} ${PIP_CRYPT_LIBRARY} SMBricks_shared SMBricks gtest gmock_main)
add_test(NAME SMBlockTest_test COMMAND SMBlock)

set(TEST_FILES_DIR "${PROJECT_SOURCE_DIR}/test_files")
configure_file(CmakeDefinitions.h.in ${CMAKE_BINARY_DIR}/generated/CmakeDefinitions.h )
include_directories( ${CMAKE_BINARY_DIR}/generated/ )
add_executable(MMWDeviceMOCTest MMWDeviceMOCTest.cpp ${CPPS} ${HEADERS} ${CPPS2} ${HEADERS2} ${CMOCS} ${CCM})
target_link_libraries(MMWDeviceMOCTest ${PIP_LIBRARY} ${PIP_CRYPT_LIBRARY} SMBricks_shared SMBricks gtest gmock_main)
add_test(NAME MMWDeviceMOCTest_test COMMAND SMBlock)

add_executable(MMWMessageTest MMWMessageUnitTests.cpp ${CPPS2} ${HEADERS2})
target_link_libraries(MMWMessageTest ${PIP_LIBRARY} ${PIP_CRYPT_LIBRARY} SMBricks_shared SMBricks gtest gmock_main)
add_test(NAME MMWMessageTest_test COMMAND SMBlock)