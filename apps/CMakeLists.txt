include(../dependencies/dependencies.cmake)

include_directories(../base)
include_directories(../dependencies)
include_directories(../sensors)
include_directories(../utils)
include_directories(../extras/pointviewer/include)

set(APPS_BUILD_PATH ${PROJECT_SOURCE_DIR}/build/apps)

file(GLOB SAMPLES_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/ *.cpp)

foreach(SOURCEFILE IN LISTS SAMPLES_SRCS)
    string(REPLACE ".cpp" "" PROGRAM ${SOURCEFILE})
    add_executable(${PROGRAM} ${SOURCEFILE})
    link_directories(../extras/pointviewer/lib)
    target_link_libraries(${PROGRAM} ${OpenCV_LIBRARIES} ${PCL_LIBRARIES} ${PCAP_LIBRARIES} PtVisualizer.dll)
    if(USE_EXTRA_SWITCH)
        target_link_libraries(${PROGRAM} ${EXTRA_LIBRARIES})
    endif(USE_EXTRA_SWITCH)
endforeach(SOURCEFILE)