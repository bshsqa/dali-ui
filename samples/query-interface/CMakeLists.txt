cmake_minimum_required(VERSION 3.20)
project(query-interface.example LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
if(NOT COMMAND DALI_ADD_SAMPLE)
  include("${CMAKE_CURRENT_LIST_DIR}/../common.cmake")
endif()
dali_add_sample(query-interface.example query-interface-example.cpp component-checks.cpp)
if(DEFINED dali${DALI_API_MAJOR}-core_QUERY_INTERFACE_CMAKE)
  set(_module "${dali${DALI_API_MAJOR}-core_QUERY_INTERFACE_CMAKE}")
  set(_core_manifest "${dali${DALI_API_MAJOR}-core_QUERY_INTERFACE_MANIFEST}")
  set(_ui_manifest "${dali${DALI_API_MAJOR}-ui-foundation_QUERY_INTERFACE_MANIFEST}")
else()
  pkg_get_variable(_module dali${DALI_API_MAJOR}-core query_interface_cmake)
  pkg_get_variable(_core_manifest dali${DALI_API_MAJOR}-core query_interface_manifest)
  pkg_get_variable(_ui_manifest dali${DALI_API_MAJOR}-ui-foundation query_interface_manifest)
endif()
include("${_module}")
dali_generate_query_interfaces(
  TARGET query-interface.example
  SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}"
  HEADER_LIST query-interface-headers.list
  INTERFACE_DIRS interfaces
  DEPENDENCY_MANIFESTS "${_core_manifest}" "${_ui_manifest}")
enable_testing()
add_test(NAME query-interface-self-test COMMAND $<TARGET_FILE:query-interface.example> --self-test)
set_tests_properties(query-interface-self-test PROPERTIES TIMEOUT 30)
