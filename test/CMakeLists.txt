

file(GLOB_RECURSE TEST_SOURCES
        "*.cpp")

set(GOOGLETEST_DIR "/home/harper/git/googletest/googletest")
#set(GOOGLETEST_DIR "/local/hajiang/libs/googletest/googletest")

include_directories(${GOOGLETEST_DIR}/include)
link_directories(${GOOGLETEST_DIR}/make)


add_executable(unittest ${TEST_SOURCES})

target_link_libraries(unittest util scan)
target_link_libraries(unittest gtest gtest_main)
