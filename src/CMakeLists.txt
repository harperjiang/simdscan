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

add_library(util STATIC ${UTIL_SOURCE})
add_library(scan STATIC ${SCAN_SOURCE})

set(SOURCE_FILES
        "main.cpp"
)

add_executable(main
    ${SOURCE_FILES}
)

add_executable(perf_uload
        "perf_uload.cpp")

target_link_libraries(scan util)
target_link_libraries(main util scan)
