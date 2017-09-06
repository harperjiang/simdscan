file(GLOB SOURCE_FILES
        *.h
        *.cpp
)

set(UTIL_SOURCE
        "util/encode.cpp"
        "util/math.cpp"
        "util/template.h"
        "util/template.cpp"
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

target_link_libraries(main util scan)
