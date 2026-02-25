#ifndef DALI_UI_INTERNAL_COLOR_VISUAL_SHADER_FACTORY_H
#define DALI_UI_INTERNAL_COLOR_VISUAL_SHADER_FACTORY_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/rendering/shader.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/visuals/visual-factory-cache.h>

namespace Dali
{
namespace UI
{
namespace Internal
{

/**
 * ColorVisualShaderFactory generates shader code and manages
 * cached shaders for ColorVisual.
 */
class ColorVisualShaderFactory
{
public:
  ColorVisualShaderFactory();
  ~ColorVisualShaderFactory();

  /**
   * Get or generate a shader for color visual with given options.
   */
  Shader GetShader(VisualFactoryCache& cache, VisualFactoryCache::ShaderType type);

  /**
   * Get the default shader type for given visual options.
   */
  static VisualFactoryCache::ShaderType GetShaderType(bool roundedCorner, bool squircleCorner, bool borderline,
                                                      bool blur, bool cutout);

private:
  ColorVisualShaderFactory(const ColorVisualShaderFactory&) = delete;
  ColorVisualShaderFactory& operator=(const ColorVisualShaderFactory&) = delete;
};

} // namespace Internal
} // namespace UI
} // namespace Dali

#endif // DALI_UI_INTERNAL_COLOR_VISUAL_SHADER_FACTORY_H
