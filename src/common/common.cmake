
# 拷贝文件， 参数1-文件， 参数2-目标目录
macro(copy_file_macro src_file dst_root)

    foreach(which_file ${src_file})
        execute_process( COMMAND ${CMAKE_COMMAND} -E copy ${which_file} ${dst_root})
    endforeach(which_file ${src_fi})
        
endmacro(copy_file_macro src_file dst_root)



# 创建文件夹
macro(create_folder_macro target_folder)
    
    # 创建 public_include 指向的文件夹
    execute_process( COMMAND ${CMAKE_COMMAND} -E make_directory ${target_folder})

endmacro(create_folder_macro target_folder)


# 拷贝文件夹
macro(copy_folder_macro src_folder target_folder)
    execute_process( COMMAND ${CMAKE_COMMAND} -E copy_directory ${src_folder} ${target_folder})
endmacro(copy_folder_macro src_folder target_folder)







set(output_prefix 
	${common_output_root}/publish
)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
# using Clang
	set(compiler_name clang)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
# using GCC
	set(compiler_name gcc)
	
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
	set(compiler_name intel)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	if(MSVC_VERSION EQUAL 1400)
		set(compiler_name vc80)
	elseif(MSVC_VERSION EQUAL 1500)
		set(compiler_name vc90)
	elseif(MSVC_VERSION EQUAL 1600)
		set(compiler_name vc100)
	elseif(MSVC_VERSION EQUAL 1700)
		set(compiler_name vc110)
	elseif(MSVC_VERSION EQUAL 1800)
		set(compiler_name vc120)
	elseif(MSVC_VERSION EQUAL 1900)
		set(compiler_name vc140)
	elseif(MSVC_VERSION EQUAL 1910)
		set(compiler_name vc141)
	elseif(MSVC_VERSION EQUAL 1920)
		set(compiler_name vc142)
	elseif(MSVC_VERSION EQUAL 1930)
		set(compiler_name vc143)
	else()
		set(compiler_name vcXX)
	endif()
	
	# message("MSC_VER=${MSC_VER}")
	
endif()



# 64位
if (CMAKE_CL_64)
	set (platform "x64")
# 32
else()
	set (platform "x86")
endif()


# publish/vc100/x64
set(publish_prefix ${output_prefix}/${compiler_name}/${platform})

# 指定include目录
set(pir ${publish_prefix}/include)


# 指定可执行程序输出目录
set(pbd 	${publish_prefix}/bin/debug)
set(pbr 	${publish_prefix}/bin/release)
# 指定 库文件输出目录
set(pld 	${publish_prefix}/lib/debug)
set(plr 	${publish_prefix}/lib/release)

# 设置 可执行程序输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${pbd})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${pbr})

# 设置库文件输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${pld})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${plr})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${pld})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${plr})


# cmake文件夹
set(
	common_cmake_root 
	${publish_prefix}/cmake
)

create_folder_macro(${pir})

create_folder_macro(${pld})
create_folder_macro(${plr})
create_folder_macro(${pbd})
create_folder_macro(${pbr})
create_folder_macro(${common_cmake_root})















