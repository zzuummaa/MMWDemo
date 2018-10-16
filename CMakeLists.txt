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

pip_code_model(CCM "mmwave2/MMWMessage.h" OPTIONS "-Es")

file(GLOB_RECURSE CPPS "mmwave/*.cpp")
file(GLOB_RECURSE HEADERS "mmwave/*.h")
file(GLOB_RECURSE CPPS2 "mmwave2/*.cpp")
file(GLOB_RECURSE HEADERS2 "mmwave2/*.h")

include_directories(${PIP_INCLUDES} ${SMBRICKS_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} mmwave mmwave2)
link_directories(${SMBRICKS_LIB_DIR})

include(IncludeGoogleTests.cmake)
add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
message("-- googletest_BINARY_DIR: ${googletest_BINARY_DIR}")
message("-- googletest_SOURCE_DIR: ${googletest_SOURCE_DIR}")
message("-- gtest_SOURCE_DIR: ${gtest_SOURCE_DIR}")
message("-- gmock_SOURCE_DIR: ${gmock_SOURCE_DIR}")

include_directories("${gtest_SOURCE_DIR}/include" "${gmock_SOURCE_DIR}/include")

# My example
add_executable(example_my example_my.cpp)
target_link_libraries(example_my gtest gmock_main)
add_test(NAME example_my_test COMMAND example_my)

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

add_executable(MMWDeviceUnitTest MMWDeviceUnitTests.cpp ${CCM} ${CPPS} ${HEADERS} ${CPPS2} ${HEADERS2} ${CMOCS} ${CCM})
target_link_libraries(MMWDeviceUnitTest ${PIP_LIBRARY} ${PIP_CRYPT_LIBRARY} SMBricks_shared SMBricks gtest gmock_main)
add_test(NAME MMWDeviceUnitTest_test COMMAND SMBlock)

add_executable(MMWMessageUnitTests MMWMessageUnitTests.cpp ${CCM} ${CPPS2} ${HEADERS2})
target_link_libraries(MMWMessageUnitTests ${PIP_LIBRARY} ${PIP_CRYPT_LIBRARY} SMBricks_shared SMBricks gtest gmock_main)
add_test(NAME MMWMessageUnitTests_test COMMAND SMBlock)

add_executable(MMWPacketExtractorUnitTests MMWPacketExtractorUnitTests.cpp ${CCM} ${CPPS} ${HEADERS} ${CPPS2} ${HEADERS2})
target_link_libraries(MMWPacketExtractorUnitTests ${PIP_LIBRARY} ${PIP_CRYPT_LIBRARY} SMBricks_shared SMBricks gtest gmock_main)
add_test(NAME MMWPacketExtractorUnitTests_test COMMAND SMBlock)

add_executable(MMWDeviceIntegrationTests MMWDeviceIntegrationTests.cpp ${CCM} ${CPPS} ${HEADERS} ${CPPS2} ${HEADERS2})
target_link_libraries(MMWDeviceIntegrationTests ${PIP_LIBRARY} ${PIP_CRYPT_LIBRARY} SMBricks_shared SMBricks gtest gmock_main)
add_test(NAME MMWDeviceIntegrationTests_test COMMAND SMBlock)