

macro(set_lib_async_log_properties target_name config_name bin_root bin_name lib_root lib_name lib_config_name)
set_property(TARGET ${target_name} APPEND PROPERTY IMPORTED_CONFIGURATIONS ${config_name})

# 指定动态库: 动态库文件
set(bin_file ${bin_root}/${lib_config_name}/${bin_name})
set_target_properties(${target_name}
PROPERTIES
"IMPORTED_LOCATION_${config_name}" ${bin_file}
)

# 指定lib库文件
set(lib_file ${lib_root}/${lib_config_name}/${lib_name})
set_target_properties(${target_name}
PROPERTIES
"IMPORTED_IMPLIB_${config_name}" ${lib_file}
)
endmacro()

# linux set the property
macro(set_lib_async_log_properties_linux target_name config_name  lib_root lib_name)
# set_property(TARGET ${target_name} APPEND PROPERTY IMPORTED_CONFIGURATIONS ${config_name})

# 指定lib库文件
set(lib_file ${lib_root}/${config_name}/${lib_name})

# set_target_properties(${target_name}
# PROPERTIES
# "IMPORTED_LOCATION_${config_name}" ${lib_file}
# )


set_target_properties(${target_name} PROPERTIES
  IMPORTED_LOCATION ${lib_file}
  IMPORTED_CONFIGURATIONS ${config_name}
)
endmacro()


# 指定根目录
set(lib_async_log_root 
${CMAKE_CURRENT_LIST_DIR}/..
)

if (NOT TARGET oct::AsyncLog)
# message("1111")
# 建库
add_library(oct::AsyncLog SHARED IMPORTED)
endif()

if (TARGET oct::AsyncLog)
# message("1111")
# 指定头文件属性
set_property(TARGET oct::AsyncLog PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${lib_async_log_root}/include)

# 关闭控制台
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
  set_lib_async_log_properties( oct::AsyncLog DEBUG   ${lib_async_log_root}/bin  "AsyncLog64D.dll" 	${lib_async_log_root}/lib "AsyncLog64D.lib" debug)
  set_lib_async_log_properties( oct::AsyncLog RELEASE ${lib_async_log_root}/bin  "AsyncLog64.dll" 	${lib_async_log_root}/lib "AsyncLog64.lib"  release)
else()  
  set_lib_async_log_properties_linux( oct::AsyncLog debug  	${lib_async_log_root}/lib "libAsyncLogD.so" )
  set_lib_async_log_properties_linux( oct::AsyncLog release  	${lib_async_log_root}/lib "libAsyncLog.so"  )
endif()


# set_property(TARGET oct::edv APPEND PROPERTY IMPORTED_CONFIGURATIONS Debug)
# set_property(TARGET oct::edv APPEND PROPERTY IMPORTED_CONFIGURATIONS Release)

# set_target_properties(oct::edv 
# PROPERTIES 
# "IMPORTED_LOCATION_DEBUG" ${lib_async_log_root_dir}/bin/Debug/OctExeDllVersionD.dll
# "IMPORTED_IMPLIB_DEBUG" ${lib_async_log_root_dir}/lib/Debug/OctExeDllVersionD.lib
# )

# set_target_properties(oct::edv 
# PROPERTIES 
# "IMPORTED_LOCATION_RELEASE" ${lib_async_log_root_dir}/bin/Release/OctExeDllVersion.dll
# "IMPORTED_IMPLIB_RELEASE" ${lib_async_log_root_dir}/lib/Release/OctExeDllVersion.lib
# )

endif()
