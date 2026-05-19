cmake_minimum_required(VERSION 3.14)
project(bestreviewer CXX)

set(CMAKE_TLS_VERIFY OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

include_directories(include)

option(ENABLE_COVERAGE "Build with gcov/lcov coverage flags" OFF)
if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(--coverage -O0 -g)
        add_link_options(--coverage)
    endif()
endif()

enable_testing()

function(link_gtest target)
    target_link_libraries(${target} GTest::gtest_main GTest::gmock)
endfunction()

function(link_filesystem_if_needed target)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
        target_link_libraries(${target} stdc++fs)
    endif()
endfunction()

add_executable(TunerTest test/TunerTest.cpp)
link_gtest(TunerTest)

add_executable(TVControllerTest test/TVControllerTest.cpp src/TVController.cpp)
link_gtest(TVControllerTest)

file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/test/golden" GOLDEN_SOURCE_DIR_PATH)

add_executable(TVControllerGoldenTest
    test/TVControllerGoldenTest.cpp
    test/TextTestFixture.cpp
    src/TVController.cpp
)
target_include_directories(TVControllerGoldenTest PRIVATE test)
target_compile_definitions(TVControllerGoldenTest PRIVATE
    GOLDEN_SOURCE_DIR="${GOLDEN_SOURCE_DIR_PATH}"
)
link_gtest(TVControllerGoldenTest)
link_filesystem_if_needed(TVControllerGoldenTest)

include(GoogleTest)

gtest_discover_tests(TunerTest)
gtest_discover_tests(TVControllerTest)
gtest_discover_tests(TVControllerGoldenTest
    PROPERTIES LABELS "golden;regression"
)

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

# option(ENABLE_COVERAGE "Build with gcov/lcov coverage flags" OFF)
# if(ENABLE_COVERAGE)
#     if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
#         add_compile_options(--coverage -O0 -g)
#         add_link_options(--coverage)
#     endif()
# endif()

# enable_testing()

# function(link_gtest target)
#     target_link_libraries(${target} GTest::gtest_main GTest::gmock)
# endfunction()

# function(link_filesystem_if_needed target)
#     if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
#         target_link_libraries(${target} stdc++fs)
#     endif()
# endfunction()

# add_executable(TunerTest test/TunerTest.cpp)
# link_gtest(TunerTest)

# add_executable(TVControllerTest test/TVControllerTest.cpp src/TVController.cpp)
# link_gtest(TVControllerTest)

# file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/test/golden" GOLDEN_SOURCE_DIR_PATH)
# add_executable(TVControllerGoldenTest
#     test/TVControllerGoldenTest.cpp
#     test/TextTestFixture.cpp
#     src/TVController.cpp
# )
# target_include_directories(TVControllerGoldenTest PRIVATE test)
# target_compile_definitions(TVControllerGoldenTest PRIVATE
#     GOLDEN_SOURCE_DIR="${GOLDEN_SOURCE_DIR_PATH}"
# )
# link_gtest(TVControllerGoldenTest)
# link_filesystem_if_needed(TVControllerGoldenTest)

# include(GoogleTest)
# gtest_discover_tests(TunerTest)
# gtest_discover_tests(TVControllerTest)
# gtest_discover_tests(TVControllerGoldenTest
#     PROPERTIES LABELS "golden;regression"
# )
