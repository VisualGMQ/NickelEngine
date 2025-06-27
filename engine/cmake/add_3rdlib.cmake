function(recursive_visit_dir dir old_paths)
    get_property(new_paths DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
    list(APPEND ${old_paths} ${new_paths} ${dir})

    foreach(new_path ${new_paths})
        recursive_visit_dir(${new_path} ${old_paths})
    endforeach()
    set(${old_paths} ${${old_paths}} PARENT_SCOPE)
endfunction()

function(get_all_subdir dir out_paths)
    recursive_visit_dir(${dir} paths)
    set(${out_paths} ${paths} PARENT_SCOPE)
endfunction()

macro(add_3rdlib dir)
    add_subdirectory(${dir})
    get_all_subdir(${CMAKE_CURRENT_SOURCE_DIR}/${dir} paths)
    foreach(path ${paths})
        get_property(target_names DIRECTORY ${path} PROPERTY BUILDSYSTEM_TARGETS)
        cmake_path(RELATIVE_PATH path
                    BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    OUTPUT_VARIABLE final_path)
        foreach(name ${target_names})
            mark_as_3rdlib_to_folder(${name} ${dir})
        endforeach()
    endforeach()
    set(paths "")
endmacro()

