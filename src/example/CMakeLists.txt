cmake_minimum_required(VERSION 3.18)
project(AsyncExamle)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


list( APPEND
CMAKE_PREFIX_PATH
${common_cmake_root}
)

## LINUX
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
  # do nothing
else()
  find_package(Threads REQUIRED)
endif()

# 找到依赖库
find_package(oct_asynclog)


add_executable(${PROJECT_NAME} 
${CMAKE_CURRENT_SOURCE_DIR}/main.cc
)

  # 链接动态库
target_link_libraries(${PROJECT_NAME} 
PRIVATE oct::AsyncLog
)

## LINUX
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
  # do nothing
else()
  target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
endif()

