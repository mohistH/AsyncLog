
# 参数1 项目名称， 参数2：输出目录下的qrc, 参数3： ts文件， 参数4：， 参数5：要翻译哪些文件夹
macro(create_qm_file target_name qt_bin_dir  TS_FILES bin_qrc_dir update_src_dir)
        set(update_all_ts_files ${target_name}_update_all_ts_files)
        set(create_all_qm_files ${target_name}_create_all_qm_files)

        add_custom_target(${update_all_ts_files} ALL)
        add_custom_target(${create_all_qm_files} ALL)

        IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
                find_file(LUPDATE_PATH lupdate ${qt_bin_dir})
                find_file(LRELEASE_PATH lrelease ${qt_bin_dir})
        ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
                find_file(LUPDATE_PATH lupdate.exe ${qt_bin_dir})
                find_file(LRELEASE_PATH lrelease.exe ${qt_bin_dir})
        endif()

        foreach(TS_FILE ${TS_FILES})
                # 把zh_CN.ts中的zh_CN提取出来
                get_filename_component(LOCAL_LANGUAGE_NAME ${TS_FILE} NAME_WE)
                set(TS_TARGET_NAME "${target_name}_update_ts_file_${LOCAL_LANGUAGE_NAME}")
                
                # 遍历目录 
                foreach( which_root ${update_src_dir})
					 add_custom_target(${TS_TARGET_NAME}    COMMAND ${LUPDATE_PATH} ${which_root} -ts ${TS_FILE} VERBATIM)
                endforeach()
                
               

                # 将update_ts_file_<NAME>添加为update_all_ts_files的依赖，下同
                add_dependencies(${update_all_ts_files} ${TS_TARGET_NAME})

                set(QM_TARGET_NAME "${target_name}_create_qm_file_${LOCAL_LANGUAGE_NAME}")
                set(QM_FILE "${bin_qrc_dir}/${LOCAL_LANGUAGE_NAME}.qm")

                add_custom_target(${QM_TARGET_NAME}  COMMAND ${LRELEASE_PATH} ${TS_FILE} -qm ${QM_FILE}    VERBATIM)

                # 因为得先有ts文件才能生成qm文件，所以把构建ts文件的目标作为自己的依赖

                
                add_dependencies(${QM_TARGET_NAME} ${TS_TARGET_NAME})
                add_dependencies(${create_all_qm_files} ${QM_TARGET_NAME})


                SET_PROPERTY(TARGET ${create_all_qm_files} PROPERTY FOLDER "LANGUAGE")
                SET_PROPERTY(TARGET ${QM_TARGET_NAME} PROPERTY FOLDER "LANGUAGE")
                SET_PROPERTY(TARGET ${update_all_ts_files} PROPERTY FOLDER "LANGUAGE")
                SET_PROPERTY(TARGET ${TS_TARGET_NAME} PROPERTY FOLDER "LANGUAGE") 
        endforeach()
endmacro( )
