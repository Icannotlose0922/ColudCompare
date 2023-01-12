cmake_minimum_required( VERSION 3.0 )

# CloudCompare example for standard plugins

# REPLACE ALL 'ExamplePlugin' OCCURENCES BY YOUR PLUGIN NAME
# AND ADAPT THE CODE BELOW TO YOUR OWN NEEDS!

#在cmake中添加一个选项，确认是否添加该插件
option( PLUGIN_GRIDMIN_STANDARD "Check to install example plugin" OFF )

if ( PLUGIN_GRIDMIN_STANDARD )
	# 添加PCL环境
	find_package(PCL REQUIRED)
	
	# Name the plugin
    project( GridMinimum )
    
	# load any subdirectories (see qAdditionalIO for an example)
    # add_subdirectory( LIB1 )
	
    include_directories( ${PCL_INCLUDE_DIRS} )
	message("target:" ${TARGET})
    
	#define the PCL_VER_1_6_OR_OLDER preprocessor to compile qPCL with older versions of PCL
	#if ( PCL_VERSION VERSION_LESS  1.7 ) # VERSION_GREATER Works just like "greater or equal"
	#	set_property( TARGET ${PROJECT_NAME} APPEND PROPERTY COMPILE_DEFINITIONS PCL_VER_1_6_OR_OLDER )
	#endif()

	# Luca's PCL patch support
	#if( PCL_VERSION VERSION_GREATER  1.7 ) #from 1.7 the patch was merged
	#	set_property( TARGET ${PROJECT_NAME} APPEND PROPERTY COMPILE_DEFINITIONS LP_PCL_PATCH_ENABLED )
	#endif()
	
    link_directories( ${PCL_LIBRARY_DIRS} )
    add_definitions( ${PCL_DEFINITIONS} )
	target_link_libraries( ${PROJECT_NAME} ${PCL_LIBRARIES})		#指定目标在运行时要寻找的链接库
	
    # set dependencies to necessary libraries (see qPCV for an example)
    # target_link_libraries( ${PROJECT_NAME} LIB1 )
    # include_directories( ${LIB1_INCLUDE_DIR} )
endif()

