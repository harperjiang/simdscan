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
add_library(scan STATIC ${SCAN_SOURCE} scan/HaoScanner512.cpp scan/HaoScanner512.h scan/BitWeaverHScanner128.cpp scan/BitWeaverHScanner128.h scan/BitWeaverHScanner256.cpp scan/BitWeaverHScanner256.h scan/BitWeaverHScanner512.cpp scan/BitWeaverHScanner512.h)

set(SOURCE_FILES
        "main.cpp"
        scan/delta/SimdDeltaScanner128.cpp scan/delta/SimdDeltaScanner128.h scan/delta/TrivialDeltaScanner.cpp scan/delta/TrivialDeltaScanner.h scan/delta/SimdDeltaScanner256.cpp scan/delta/SimdDeltaScanner256.h)

add_executable(main
    ${SOURCE_FILES}
)

target_link_libraries(scan util)
target_link_libraries(main util scan)
