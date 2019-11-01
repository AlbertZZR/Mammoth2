include(../dependencies/dependencies.cmake)

include_directories(../base)
include_directories(../dependencies)
include_directories(../extras/pointviewer/include/)

set(TESTS_BUILD_PATH ${PROJECT_SOURCE_DIR}/build/tests)

file(GLOB TESTS_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/ *.cpp)

foreach(SOURCEFILE IN LISTS TESTS_SRCS)
    string(REPLACE ".cpp" "" PROGRAM ${SOURCEFILE})
    add_executable(${PROGRAM} ${SOURCEFILE})
    target_link_libraries(${PROGRAM} ${OpenCV_LIBRARIES} ${PCL_LIBRARIES} ${PCAP_LIBRARIES})
    if(USE_EXTRA_SWITCH)
        target_link_libraries(${PROGRAM} ${EXTRA_LIBRARIES} -lPtVisualizer)
    endif(USE_EXTRA_SWITCH)
endforeach(SOURCEFILE)