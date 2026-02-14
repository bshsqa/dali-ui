# CMakeLists.txt for dali-ui-visuals shader generation
#
# This generates header files from .vert/.frag/.def shader sources using
# the shader-generator tool, producing:
#   - generated/<shader-name>.h  (individual shader string_view headers)
#   - builtin-shader-extern-gen.h (extern declarations for all shaders)
#   - generated/builtin-shader-gen.cpp (definitions for all shaders)
#
# Usage: Include this from the parent dali-ui-visuals CMakeLists.txt
#
# Expected variables:
#   SHADER_GENERATOR_BINARY - path to the dali-shader-generator executable
#
# Provides:
#   SHADER_GENERATED_CPP - the generated .cpp file to add to SOURCES

SET(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dali-ui-visuals/internal/graphics/shaders/")
SET(SHADER_GENERATED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dali-ui-visuals/internal/graphics/")

# Build the shader generator tool
SET(SHADER_GENERATOR_NAME dali-ui-shader-generator)
SET(SHADER_GENERATOR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/dali-ui-visuals/shader-generator/shader-generator.cpp")

IF(WIN32)
  IF(NOT CMAKE_BUILD_TYPE)
    SET(SHADER_GENERATOR_BINARY ${CMAKE_CURRENT_BINARY_DIR}/Debug/${SHADER_GENERATOR_NAME})
  ELSE()
    SET(SHADER_GENERATOR_BINARY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${SHADER_GENERATOR_NAME})
  ENDIF()
ELSE()
  SET(SHADER_GENERATOR_BINARY ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_GENERATOR_NAME})
ENDIF()

ADD_EXECUTABLE(${SHADER_GENERATOR_NAME} ${SHADER_GENERATOR_SOURCES})

# Collect all shader source files
FILE(GLOB SHADERS_SRC "${SHADER_SOURCE_DIR}/*.vert" "${SHADER_SOURCE_DIR}/*.frag" "${SHADER_SOURCE_DIR}/*.def")

# Generate shader headers + builtin-shader-gen.cpp
SET(BUILT_IN_SHADER_GEN_CPP "${SHADER_GENERATED_DIR}/generated/builtin-shader-gen.cpp")

ADD_CUSTOM_COMMAND(
  OUTPUT ${BUILT_IN_SHADER_GEN_CPP}
  DEPENDS ${SHADER_GENERATOR_NAME} ${SHADERS_SRC}
  COMMAND ${SHADER_GENERATOR_BINARY} ${SHADER_SOURCE_DIR} ${SHADER_GENERATED_DIR}
  COMMENT "Generating dali-ui-visuals built-in shader headers"
)

# Clean up generated files
SET_PROPERTY(DIRECTORY PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
  "${SHADER_GENERATED_DIR}/generated/"
  "${SHADER_GENERATED_DIR}/builtin-shader-extern-gen.h")

# Export the generated .cpp for parent CMakeLists.txt
SET(SHADER_GENERATED_CPP ${BUILT_IN_SHADER_GEN_CPP} PARENT_SCOPE)
