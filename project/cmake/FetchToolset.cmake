include_guard(GLOBAL)

find_package(Git REQUIRED)

set(ATFRAMEWORK_CMAKE_TOOLSET_DIR
    "${PROJECT_SOURCE_DIR}/project/cmake/toolset"
    CACHE PATH "PATH to cmake-toolset")
set(ATFRAMEWORK_CMAKE_TOOLSET_GIT_URL
    "https://github.com/atframework/cmake-toolset.git"
    CACHE STRING "Git URL of cmake-toolset")
set(ATFRAMEWORK_CMAKE_TOOLSET_VERSION
    "v1"
    CACHE STRING "Branch or tag of cmake-toolset")

if(NOT EXISTS "${ATFRAMEWORK_CMAKE_TOOLSET_DIR}/Import.cmake" OR NOT EXISTS "${ATFRAMEWORK_CMAKE_TOOLSET_DIR}/.git")
  get_filename_component(ATFRAMEWORK_CMAKE_TOOLSET_PARENT_DIR "${ATFRAMEWORK_CMAKE_TOOLSET_DIR}" DIRECTORY)
  get_filename_component(ATFRAMEWORK_CMAKE_TOOLSET_BASENAME "${ATFRAMEWORK_CMAKE_TOOLSET_DIR}" NAME)
  if(NOT EXISTS "${ATFRAMEWORK_CMAKE_TOOLSET_PARENT_DIR}")
    file(MAKE_DIRECTORY "${ATFRAMEWORK_CMAKE_TOOLSET_PARENT_DIR}")
  endif()
  if(EXISTS "${ATFRAMEWORK_CMAKE_TOOLSET_DIR}")
    file(REMOVE_RECURSE "${ATFRAMEWORK_CMAKE_TOOLSET_DIR}")
  endif()
  unset(ATFRAMEWORK_CMAKE_TOOLSET_EXECUTE_PROCESS_OUTPUT_OPTIONS)
  if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.15")
    list(APPEND ATFRAMEWORK_CMAKE_TOOLSET_EXECUTE_PROCESS_OUTPUT_OPTIONS COMMAND_ECHO STDOUT)
  endif()
  if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.18")
    list(APPEND ATFRAMEWORK_CMAKE_TOOLSET_EXECUTE_PROCESS_OUTPUT_OPTIONS ECHO_OUTPUT_VARIABLE ECHO_ERROR_VARIABLE)
  endif()
  execute_process(
    COMMAND ${GIT_EXECUTABLE} -c "advice.detachedHead=false" clone -b "${ATFRAMEWORK_CMAKE_TOOLSET_VERSION}" --depth 1
            "${ATFRAMEWORK_CMAKE_TOOLSET_GIT_URL}" "${ATFRAMEWORK_CMAKE_TOOLSET_BASENAME}"
    WORKING_DIRECTORY "${ATFRAMEWORK_CMAKE_TOOLSET_PARENT_DIR}"
                      ${ATFRAMEWORK_CMAKE_TOOLSET_EXECUTE_PROCESS_OUTPUT_OPTIONS})
endif()