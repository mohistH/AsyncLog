
cmake_minimum_required(VERSION 3.14)
project(AsyncLog)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 指定源码目录
set(PROJECT_ROOT
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 有这些文件需要纳入编译: 不推荐这样用， 应该一个一个文件的手动加入到这里
file(GLOB_RECURSE 
PROJECT_SRC_FILES 
${PROJECT_ROOT}/*.h
${PROJECT_ROOT}/*.cpp
)
 # debug模式下增加后缀D
 set(CMAKE_DEBUG_POSTFIX "D") 

 # 判断系统是否为64位
# if(CMAKE_SIZEOF_VOID_P EQUAL 8)
# 64位
if (CMAKE_CL_64)
set(PROJECT_NAME ${PROJECT_NAME}64)
else()
message(STATUS "系统不是64位")
endif()

# 创建可执行程序项目
add_library(${PROJECT_NAME} SHARED
${PROJECT_SRC_FILES}
)

# 指定头文件搜索路径
target_include_directories(${PROJECT_NAME}
PRIVATE ${PROJECT_ROOT}
)

# 拷贝cmake文件到输出文件夹
copy_folder_macro(${CMAKE_CURRENT_SOURCE_DIR}/cmake ${common_cmake_root})

# 拷贝头文件到指定的文件夹
copy_file_macro(${CMAKE_CURRENT_SOURCE_DIR}/src/AsyncLogging.hpp ${pir})

# 如果是windows，设置管理员权限。注意使用if需要配合endif使用
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
	# set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/level='requireAdministrator'" )
endif()

# 关闭控制台
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()
