# cmake_minimum_required(VERSION 3.14)
# project(bestreviewer CXX)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# include(FetchContent)
# FetchContent_Declare(
#     googletest
#     GIT_REPOSITORY https://github.com/google/googletest.git
#     GIT_TAG v1.14.0
# )
# FetchContent_MakeAvailable(googletest)

# include_directories(include)

# enable_testing()

# add_executable(TunerTest test/TunerTest.cpp)
# target_link_libraries(TunerTest GTest::gtest_main GTest::gmock)

# add_executable(TVControllerTest test/TVControllerTest.cpp)
# target_link_libraries(TVControllerTest GTest::gtest_main GTest::gmock)

# include(GoogleTest)
# gtest_discover_tests(TunerTest)
# gtest_discover_tests(TVControllerTest)


cmake_minimum_required(VERSION 3.14)

project(TDD_TV CXX)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0)

FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

add_executable(tv_test
    test/TunerTest.cpp
    test/TVChannelControllerTest.cpp
    test/TVChannelControllerMockTest.cpp
    src/TVChannelController.cpp)

target_include_directories(tv_test PRIVATE 
    include
    test)

# target_link_libraries(tv_test
#     GTest::gtest_main
#     GMock::gmock_main)

target_link_libraries(tv_test
   GTest::gtest_main
   gmock)

gtest_discover_tests(tv_test)

