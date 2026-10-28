cmake_minimum_required(VERSION 3.20)
project(dali2-ui VERSION 2.0.0 LANGUAGES C CXX)
if(NOT EMSCRIPTEN)
  message(FATAL_ERROR "The Web backend requires Emscripten. Configure with emcmake cmake.")
endif()
list(APPEND CMAKE_FIND_ROOT_PATH "${CMAKE_INSTALL_PREFIX}")
find_package(dali2-adaptor CONFIG REQUIRED)
include("${CMAKE_INSTALL_PREFIX}/share/dali-web/web-common.cmake")
get_filename_component(ROOT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
set(foundation "${ROOT_SRC_DIR}/dali-ui-foundation")
set(components "${ROOT_SRC_DIR}/dali-ui-components")

# The shader generator runs on the build host, not in WebAssembly.
find_program(DALI_WEB_HOST_CXX NAMES c++ g++ REQUIRED NO_CMAKE_FIND_ROOT_PATH)
set(generator "${CMAKE_CURRENT_BINARY_DIR}/host-shader-generator")
add_custom_command(OUTPUT "${generator}"
  COMMAND "${DALI_WEB_HOST_CXX}" -std=c++17 -O2 "${foundation}/shader-generator/shader-generator.cpp" -o "${generator}"
  DEPENDS "${foundation}/shader-generator/shader-generator.cpp" VERBATIM)
set(generated "${CMAKE_CURRENT_BINARY_DIR}/include/dali-ui-foundation/internal/graphics/generated")
file(GLOB shader_sources CONFIGURE_DEPENDS "${foundation}/internal/graphics/shaders/*")
add_custom_command(OUTPUT "${generated}/builtin-shader-gen.cpp"
  COMMAND "${generator}" "${foundation}/internal/graphics/shaders" "${generated}"
  DEPENDS "${generator}" ${shader_sources} VERBATIM)
add_custom_target(dali-web-shaders DEPENDS "${generated}/builtin-shader-gen.cpp")

file(GLOB_RECURSE foundation_sources CONFIGURE_DEPENDS "${foundation}/*.cpp")
list(FILTER foundation_sources EXCLUDE REGEX "/shader-generator/|/internal/graphics/generated/")
add_library(dali2-ui-foundation STATIC ${foundation_sources} "${generated}/builtin-shader-gen.cpp")
add_dependencies(dali2-ui-foundation dali-web-shaders)
dali_web_target(dali2-ui-foundation)
target_include_directories(dali2-ui-foundation BEFORE PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
  $<BUILD_INTERFACE:${ROOT_SRC_DIR}>
  $<INSTALL_INTERFACE:include>)
target_include_directories(dali2-ui-foundation PRIVATE "${foundation}/internal"
  "${foundation}/third-party" "${foundation}/integration-api")
target_compile_definitions(dali2-ui-foundation PRIVATE NO_THORVG
  DALI_IMAGE_DIR="/usr/share/dali/ui/images/" DALI_DATA_READ_ONLY_DIR="/usr/share/dali/"
  DALI_SOUND_DIR="/usr/share/dali/toolkit/sounds/" DALI_STYLE_DIR="/usr/share/dali/toolkit/styles/"
  DALI_STYLE_IMAGE_DIR="/usr/share/dali/toolkit/styles/images/")
target_link_libraries(dali2-ui-foundation PUBLIC dali2-adaptor::dali2-adaptor dali2-core::dali2-core)

option(ENABLE_INVOKE_METHOD_WRAPPERS "Generate public InvokeMethod wrappers" ON)
if(ENABLE_INVOKE_METHOD_WRAPPERS)
  find_package(Python3 COMPONENTS Interpreter REQUIRED)
  set(invoke_cpp "${CMAKE_CURRENT_BINARY_DIR}/generated/public-api-invoke-methods.cpp")
  file(GLOB_RECURSE invoke_inputs CONFIGURE_DEPENDS "${foundation}/*.h" "${foundation}/*.cpp")
  add_custom_command(OUTPUT "${invoke_cpp}"
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_INSTALL_PREFIX}/bin/dali-invoke-method-generator"
      --scan-root "${foundation}/public-api" --include-prefix dali-ui-foundation/public-api
      --dependency-scan-root "${ROOT_SRC_DIR}/../dali-core/dali/public-api" --dependency-include-prefix dali/public-api
      --dependency-scan-root "${ROOT_SRC_DIR}/../dali-adaptor/dali/public-api" --dependency-include-prefix dali/public-api
      --dependency-export-api DALI_CORE_API --dependency-export-api DALI_ADAPTOR_API
      --registration-scan-root "${foundation}" --namespace Dali::Ui --auto-export-api DALI_UI_API
      --allow-type Duration --allow-type Dali::Ui::Duration --allow-type UiColor --allow-type Dali::Ui::UiColor
      --output "${invoke_cpp}"
    DEPENDS ${invoke_inputs} "${CMAKE_INSTALL_PREFIX}/bin/dali-invoke-method-generator" VERBATIM)
  target_sources(dali2-ui-foundation PRIVATE "${invoke_cpp}")
endif()
dali_web_install(dali2-ui-foundation "dali2-core;dali2-adaptor")

file(GLOB_RECURSE component_sources CONFIGURE_DEPENDS "${components}/*.cpp")
add_library(dali2-ui-components STATIC ${component_sources}
  "${ROOT_SRC_DIR}/third-party/md4c/entity.c" "${ROOT_SRC_DIR}/third-party/md4c/md4c.c")
dali_web_target(dali2-ui-components)
target_include_directories(dali2-ui-components PUBLIC
  $<BUILD_INTERFACE:${ROOT_SRC_DIR}> $<INSTALL_INTERFACE:include>)
target_compile_definitions(dali2-ui-components PRIVATE
  DALI_UI_COMPONENTS_IMAGE_DIR="/usr/share/dali/ui/images/components/")
target_link_libraries(dali2-ui-components PUBLIC dali2-ui-foundation)
dali_web_install(dali2-ui-components "dali2-ui-foundation")
foreach(module dali-ui-foundation dali-ui-components)
  install(DIRECTORY "${ROOT_SRC_DIR}/${module}/" DESTINATION include/${module}
    FILES_MATCHING PATTERN "*.h" PATTERN "internal" EXCLUDE PATTERN "shader-generator" EXCLUDE)
endforeach()
install(DIRECTORY "${components}/images/" DESTINATION share/dali/ui/images/components)
install(DIRECTORY "${foundation}/images/" DESTINATION share/dali/ui/images)
