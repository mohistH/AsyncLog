cmake_minimum_required(VERSION 3.18)
project(OctToolkit)

# 设置通用cmake
set( common_root
        ${CMAKE_CURRENT_SOURCE_DIR}/common
)

set( common_output_root 
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# 加载common 中的 cmake 脚本设置
set(common_cmake         ${common_root}/common.cmake)
set(linguist_cmake      ${common_root}/linguist.cmake)
include(${common_cmake})

MESSAGE("")
# message("CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG=${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}")
# MESSAGE("CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG}")

# 1. 打开允许创建文件夹的开关
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# option 
option(ASYNC_LOG "CREATE SHARED LIBRARY PROJECT" ON)
option(ASYNC_LOG_EXAMPLE "THE EXAMPLE TO CALL ASYNC_LOG" Off)

# 日志
if (ASYNC_LOG)
        add_subdirectory("src")
endif(ASYNC_LOG)

# 范例项目
if (ASYNC_LOG_EXAMPLE)
        add_subdirectory("example")
endif(ASYNC_LOG_EXAMPLE)

# 2. 把工程加到文件夹中
# set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "CMakeTargets")
# 3. 给 cmake 自动创建的工程重新命名, 默认名为 "CMakePredefinedTargets" 
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")
