cmake_minimum_required(VERSION 2.4.6)
include($ENV{ROS_ROOT}/core/rosbuild/rosbuild.cmake)

# Set the build type.  Options are:
#  Coverage       : w/ debug symbols, w/o optimization, w/ code-coverage
#  Debug          : w/ debug symbols, w/o optimization
#  Release        : w/o debug symbols, w/ optimization
#  RelWithDebInfo : w/ debug symbols, w/ optimization
#  MinSizeRel     : w/o debug symbols, w/ optimization, stripped binaries
#set(ROS_BUILD_TYPE RelWithDebInfo)

rosbuild_init()

#set the default path for built executables to the "bin" directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#set the default path for built libraries to the "lib" directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#uncomment if you have defined messages
#rosbuild_genmsg()
#uncomment if you have defined services
#rosbuild_gensrv()

include_directories(
	${PROJECT_SOURCE_DIR}/common/include
	${PROJECT_SOURCE_DIR}/ros/include
)

#common commands for building c++ executables and libraries
#rosbuild_add_library(${PROJECT_NAME} src/example.cpp)
#target_link_libraries(${PROJECT_NAME} another_library)
#rosbuild_add_boost_directories()
#rosbuild_link_boost(${PROJECT_NAME} thread)
#rosbuild_add_executable(example examples/example.cpp)
#target_link_libraries(example ${PROJECT_NAME})

########### LIBRARIES ###############################
rosbuild_add_library(nxt_bluetooth 
	common/src/nxt.cpp
)

########### EXECUTEABLES ############################
rosbuild_add_executable(ar_kinect_bluetooth 
	ros/src/ar_kinect_bluetooth.cpp
	ros/src/ar_object.cpp
)
target_link_libraries(ar_kinect_bluetooth
	bluetooth
	nxt_bluetooth
	GLU 
	GL 
	glut 
	ARgsub 
	AR 
	ARMulti 
	ARvideo
)

########### TESTS ####################################
#rosbuild_add_executable(test_bluetooth_connection common/test/bluetooth_connection.cpp)
#target_link_libraries(test_bluetooth_connection 
#	bluetooth
#	nxt_bluetooth
#)
