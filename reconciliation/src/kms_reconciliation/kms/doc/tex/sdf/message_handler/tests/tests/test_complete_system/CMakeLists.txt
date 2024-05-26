project(Message_Handler_end_to_end)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Get Google test
#find_package(GTest REQUIRED)
#include_directories(${GTEST_INCLUDE_DIRS})

# Get Google test automatically
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.tar.gz
)
FetchContent_MakeAvailable(googletest)



set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/tests)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../../lib
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../../eigen/Eigen
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../../cryptopp850
)

set(SOURCES
		${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../../lib/netxpto_20200819.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../../lib/ms_windows_console_output_common_20200819.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../../lib/message_handler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../../lib/ip_tunnel_ms_windows_20200819.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_complete_system.cpp
    )

add_executable(message_handler_end_to_end ${SOURCES})
#add_executable(discretion_test
#    ${CMAKE_CURRENT_SOURCE_DIR}/t_handler_message_tests.cpp
#)

target_link_libraries(message_handler_end_to_end gtest)
target_link_libraries(message_handler_end_to_end gtest_main)

#    PRIVATE
#        GTest::GTest
#        GTest::Main
#        # add any necessary libraries here
#)
add_test(message_handler_end_to_end message_handler_end_to_end)
