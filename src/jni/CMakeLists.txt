set(JAVA_HOME "/usr/lib/jvm/jdk1.8.0_201/")
#set(JAVA_HOME "/usr/lib/jvm/java-8-openjdk-amd64/")


include_directories(${JAVA_HOME}/include)
include_directories(${JAVA_HOME}/include/linux)

add_library(simdscan SHARED
        "edu_uchicago_cs_db_sboost_SimdScanner.h"
        "SimdScanner.cpp")


target_link_libraries(simdscan scan)
target_link_libraries(simdscan decode)

