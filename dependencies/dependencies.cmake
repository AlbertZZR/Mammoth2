# config paths
set(OpenCV_ROOT_PATH C:/opencv/opencv)
set(PCL_ROOT_PATH C:/PCL1.8.0)

# config opencv
set(OpenCV_DIR ${OpenCV_ROOT_PATH}/build)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# config pcl
set(PCL_DIR ${PCL_ROOT_PATH}/cmake)
find_package(PCL REQUIRED)
include_directories(${PCL_INCLUDE_DIRS})

# include all libs' header files
include_directories(./)