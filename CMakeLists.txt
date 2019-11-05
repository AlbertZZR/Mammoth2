cmake_minimum_required(VERSION 3.5)
project(Mammoth2 C CXX)

# configure paths
# Important!!!: You should replace these paths to your own.
set(OpenCV_ROOT_PATH C:/opencv/opencv)
set(PCL_ROOT_PATH C:/PCL1.8.0)
set(PCAP_ROOT_PATH C:/WinPcap/WpdPack)

# configure executable program switches
set(APPS_BUILD_SWITCH ON)
set(SAMPLES_BUILD_SWITCH OFF)
set(TESTS_BUILD_SWITCH OFF)

# configure extra-libs compile switch
set(USE_EXTRA_SWITCH ON)

# configure extra component
if(USE_EXTRA_SWITCH)
	# include_directories(${extra_directories})
	# fill ${EXTRA_LIBRARIES} varible
	include(extras/extras.cmake)
endif(USE_EXTRA_SWITCH)

# configure each component part-by-part.
if(APPS_BUILD_SWITCH)
	message("Apps building switch on")
	add_subdirectory(apps apps)
else(APPS_BUILD_SWITCH)
	message("Apps building switch off")
endif(APPS_BUILD_SWITCH)

if(SAMPLES_BUILD_SWITCH)
	message("Samples building switch on")
	add_subdirectory(samples samples)
else(SAMPLES_BUILD_SWITCH)
	message("Samples building switch off")
endif(SAMPLES_BUILD_SWITCH)

if(TESTS_BUILD_SWITCH)
	message("Tests building switch on")
	add_subdirectory(tests tests)
else(TESTS_BUILD_SWITCH)
	message("Tests building switch off")
endif(TESTS_BUILD_SWITCH)