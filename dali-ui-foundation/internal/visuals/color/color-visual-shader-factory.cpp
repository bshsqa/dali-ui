/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// CLASS HEADER
#include <dali-ui-foundation/internal/visuals/color/color-visual-shader-factory.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace UI
{
namespace Internal
{

namespace
{

const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
    INPUT mediump vec2 aPosition;\n uniform highp mat4 uMvpMatrix;\n uniform highp vec3 uSize;\n uniform mediump vec2 offset;\n uniform highp vec2 size;\n uniform mediump vec2 extraSize;\n uniform mediump vec4 offsetSizeMode;\n uniform mediump vec2 origin;\n uniform mediump vec2 anchorPoint;\n
  \n vec4 ComputeVertexPosition()\n {
      \n vec2 visualSize = mix(uSize.xy * size, size, offsetSizeMode.zw) + extraSize;
      \n vec2 visualOffset = mix(offset * uSize.xy, offset, offsetSizeMode.xy);
      \n return vec4(aPosition * visualSize + visualOffset + origin * uSize.xy - anchorPoint * visualSize, 0.0, 1.0);
      \n
    }\n
  \n void main()\n {
      \n gl_Position = uMvpMatrix * ComputeVertexPosition();
      \n
    }\n);

const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(uniform lowp vec4 uColor;\n uniform lowp vec3 mixColor;\n
  \n void main()\n {
  \n OUT_COLOR = vec4(mixColor, 1.0) * uColor;
  \n
}\n);

} // unnamed namespace

ColorVisualShaderFactory::ColorVisualShaderFactory() = default;
ColorVisualShaderFactory::~ColorVisualShaderFactory() = default;

Shader ColorVisualShaderFactory::GetShader(VisualFactoryCache& cache, VisualFactoryCache::ShaderType type)
{
  Shader shader = cache.GetShader(type);
  if (!shader)
  {
    // For now, use the basic color shader for all types
    // TODO: Add rounded corner, borderline, blur, cutout shader variants
    shader = Shader::New(VERTEX_SHADER, FRAGMENT_SHADER, Shader::Hint::NONE, "COLOR_VISUAL_SHADER");
    cache.SaveShader(type, shader);
  }
  return shader;
}

VisualFactoryCache::ShaderType ColorVisualShaderFactory::GetShaderType(bool roundedCorner, bool squircleCorner,
                                                                       bool borderline, bool blur, bool cutout)
{
  int shaderIndex = 0;

  if (cutout)
  {
    shaderIndex += 12; // Offset to cutout variants
  }

  if (blur)
  {
    shaderIndex += 6; // Offset to blur variants
    if (borderline)
    {
      shaderIndex += 3;
    }
  }
  else if (borderline)
  {
    shaderIndex += 3;
  }

  if (squircleCorner)
  {
    shaderIndex += 2;
  }
  else if (roundedCorner)
  {
    shaderIndex += 1;
  }

  // Clamp to valid range
  if (shaderIndex >= static_cast<int>(VisualFactoryCache::SHADER_TYPE_MAX))
  {
    shaderIndex = 0;
  }

  return static_cast<VisualFactoryCache::ShaderType>(shaderIndex);
}

} // namespace Internal
} // namespace UI
} // namespace Dali
