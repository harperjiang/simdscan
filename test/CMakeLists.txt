

file(GLOB_RECURSE TEST_SOURCES
        "*.cpp")

set(GOOGLETEST_DIR "/Users/harper/git/googletest/googletest")

include_directories(${GOOGLETEST_DIR}/include)
link_directories(${GOOGLETEST_DIR}/make)


add_executable(unittest ${TEST_SOURCES} scan/bitpack/BitWeaverHScanner128_test.cpp)

target_link_libraries(unittest util scan)
target_link_libraries(unittest gtest gtest_main)
