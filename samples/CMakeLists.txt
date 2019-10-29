aux_source_directory(. SAMPLES_SRCS)

include_directories(../base)
include_directories(../dependencies)

set(SAMPLES_BUILD_PATH ${PROJECT_SOURCE_DIR}/build/samples)

file(GLOB APP_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/ *.cpp)

foreach(SOURCEFILE IN LISTS APP_SOURCES)
    string(REPLACE ".cpp" "" PROGRAM ${SOURCEFILE})
    add_executable(${PROGRAM} ${SOURCEFILE})
endforeach(SOURCEFILE)