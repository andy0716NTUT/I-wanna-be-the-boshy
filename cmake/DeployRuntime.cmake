if(NOT WIN32)
    return()
endif()
get_filename_component(destination "${EXECUTABLE}" DIRECTORY)
get_filename_component(compiler_directory "${COMPILER}" DIRECTORY)
if(OBJDUMP)
    set(CMAKE_GET_RUNTIME_DEPENDENCIES_PLATFORM windows+pe)
    set(CMAKE_GET_RUNTIME_DEPENDENCIES_TOOL objdump)
    set(CMAKE_GET_RUNTIME_DEPENDENCIES_COMMAND "${OBJDUMP}")
endif()
file(GET_RUNTIME_DEPENDENCIES
    EXECUTABLES "${EXECUTABLE}"
    DIRECTORIES "${compiler_directory}"
    RESOLVED_DEPENDENCIES_VAR resolved
    UNRESOLVED_DEPENDENCIES_VAR unresolved
    PRE_EXCLUDE_REGEXES "api-ms-.*" "ext-ms-.*"
    POST_EXCLUDE_REGEXES ".*[\\/]Windows[\\/].*" ".*[\\/]WINDOWS[\\/].*")
if(unresolved)
    message(FATAL_ERROR "Unresolved runtime DLLs for ${EXECUTABLE}: ${unresolved}")
endif()
foreach(dll IN LISTS resolved)
    file(COPY "${dll}" DESTINATION "${destination}")
endforeach()
