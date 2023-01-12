cmake_minimum_required( VERSION 3.0 )

# CloudCompare example for standard plugins

# REPLACE ALL 'ExamplePlugin' OCCURENCES BY YOUR PLUGIN NAME
# AND ADAPT THE CODE BELOW TO YOUR OWN NEEDS!

# Add an option to CMake to control whether we build this plugin or not
option( PLUGIN_CLUSTER_BY_PICK_POINT "Check to install ByPickPoint plugin" OFF )

if ( PLUGIN_CLUSTER_BY_PICK_POINT )
	#pcl package
	find_package(PCL REQUIRED)
		
	# Name the plugin
    project( ClusterByPickPointPlugin )
	AddPlugin( NAME ${PROJECT_NAME} )
    # load any subdirectories (see qAdditionalIO for an example)
    # add_subdirectory( LIB1 )
	include_directories( ${PCL_INCLUDE_DIRS} )
	
	#SET(CMAKE_AUTOMOC ON)# 设置自动生成moc文件,AUTOMOC打开可以省去QT5_WRAP_CPP命令
	
	#SET(CMAKE_AUTOUIC ON)# 设置自动生成ui.h文件,AUTOUIC打开可以省去QT5_WRAP_UI命令
	
	#SET(CMAKE_AUTORCC ON)# 【optional】设置自动处理.qrc文件
	#FILE(GLOB UI_FILES "./ui/ByPickPointDlg.ui") 
	
	#QT5_WRAP_UI(WRAP_FILES ${UI_FILES}) # 通过Ui文件生成对应的头文件
	#SOURCE_GROUP("UI" FILES ${UI_FILES} ${WRAP_FILES} )
	add_subdirectory(ui)
	add_subdirectory(include)
   # add_subdirectory(src)
	 

	#if ( PCL_VERSION VERSION_LESS  1.7 ) # VERSION_GREATER Works just like "greater or equal"
	#	set_property( TARGET ${PROJECT_NAME} APPEND PROPERTY COMPILE_DEFINITIONS PCL_VER_1_6_OR_OLDER )
	#endif()
	
	# Luca's PCL patch support
	#if( PCL_VERSION VERSION_GREATER  1.7 ) #from 1.7 the patch was merged
	#	set_property( TARGET ${PROJECT_NAME} APPEND PROPERTY COMPILE_DEFINITIONS LP_PCL_PATCH_ENABLED )
	#endif()
	link_directories( ${PCL_LIBRARY_DIRS} )
    add_definitions( ${PCL_DEFINITIONS} )
	
	target_link_libraries( ${PROJECT_NAME} ${PCL_LIBRARIES})

    # set dependencies to necessary libraries (see qPCV for an example)
    # target_link_libraries( ${PROJECT_NAME} LIB1 )
    # include_directories( ${LIB1_INCLUDE_DIR} )
endif()
