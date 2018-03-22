file(GLOB_RECURSE UTIL_SOURCE
        "util/*.cpp"
        "util/*.h"
        )

file(GLOB_RECURSE SCAN_SOURCE
        "predicate/*.h"
        "predicate/*.cpp"
        "scan/*.h"
        "scan/*.cpp"
        )

file(GLOB_RECURSE DECODE_SOURCE
        "decode/*.h"
        "decode/*.cpp"
        )

add_library(util STATIC ${UTIL_SOURCE})
add_library(scan STATIC ${SCAN_SOURCE})
add_library(decode STATIC ${DECODE_SOURCE})

add_executable(perf_bp "perf_bp.cpp")

add_executable(perf_delta "perf_delta.cpp")
add_executable(perf_rle "perf_rle.cpp")
add_executable(perf_uload "perf_uload.cpp")

add_executable(parallel_bp "parallel_bp.cpp")
add_executable(parallel_delta "parallel_delta.cpp")
add_executable(parallel_rle "parallel_rle.cpp")

target_link_libraries(decode util)
target_link_libraries(scan util)
target_link_libraries(perf_bp util scan)
target_link_libraries(perf_delta util scan)
target_link_libraries(perf_rle util scan)
target_link_libraries(perf_uload util scan)

target_link_libraries(parallel_bp util scan)
target_link_libraries(parallel_delta util scan)
target_link_libraries(parallel_rle util scan)

add_subdirectory(jni)
