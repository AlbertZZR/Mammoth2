# config opencv
set(OpenCV_DIR ${OpenCV_ROOT_PATH}/build)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# config pcl
set(PCL_DIR ${PCL_ROOT_PATH}/cmake)
find_package(PCL REQUIRED)
include_directories(${PCL_INCLUDE_DIRS})

# config pcap
include_directories(${PCAP_ROOT_PATH}/Include ${PCAP_ROOT_PATH}/Include/pcap)
file(GLOB_RECURSE PCAP_LIBRARIES "${PCAP_ROOT_PATH}/Lib/x64/*.lib")

# include all dependencies' header files
include_directories(./)