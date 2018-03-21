set(JAVA_HOME "/usr/lib/jvm/jdk1.8.0_151/")
#set(JAVA_HOME "/usr/lib/jvm/java-8-openjdk-amd64/")


include_directories(${JAVA_HOME}/include)
include_directories(${JAVA_HOME}/include/linux)

add_library(simdscan SHARED
        "edu_uchicago_cs_encsel_query_simdscan_SimdScanner.h"
        "SimdScanner.cpp")

install(TARGETS simdscan DESTINATION bin)