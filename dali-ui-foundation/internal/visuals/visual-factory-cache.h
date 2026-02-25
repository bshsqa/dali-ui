#ifndef DALI_UI_INTERNAL_VISUAL_FACTORY_CACHE_H
#define DALI_UI_INTERNAL_VISUAL_FACTORY_CACHE_H

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
#include <dali/public-api/math/uint-16-pair.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/rendering/geometry.h>
#include <dali/public-api/rendering/shader.h>

namespace Dali
{
namespace UI
{
namespace Internal
{

class ColorVisualShaderFactory;

/**
 * Caches shaders and geometries. Owned by VisualFactory.
 * Stripped for dali-ui: only supports Color Visual shaders.
 */
class VisualFactoryCache
{
public:
  enum ShaderType
  {
    COLOR_SHADER,
    COLOR_SHADER_ROUNDED_CORNER,
    COLOR_SHADER_SQUIRCLE_CORNER,
    COLOR_SHADER_BORDERLINE,
    COLOR_SHADER_ROUNDED_BORDERLINE,
    COLOR_SHADER_SQUIRCLE_BORDERLINE,
    COLOR_SHADER_BLUR_EDGE,
    COLOR_SHADER_ROUNDED_CORNER_BLUR_EDGE,
    COLOR_SHADER_SQUIRCLE_CORNER_BLUR_EDGE,
    COLOR_SHADER_BORDERLINE_BLUR_EDGE,
    COLOR_SHADER_ROUNDED_BORDERLINE_BLUR_EDGE,
    COLOR_SHADER_SQUIRCLE_BORDERLINE_BLUR_EDGE,
    COLOR_SHADER_CUTOUT,
    COLOR_SHADER_CUTOUT_ROUNDED_CORNER,
    COLOR_SHADER_CUTOUT_SQUIRCLE_CORNER,
    COLOR_SHADER_CUTOUT_BORDERLINE,
    COLOR_SHADER_CUTOUT_ROUNDED_BORDERLINE,
    COLOR_SHADER_CUTOUT_SQUIRCLE_BORDERLINE,
    COLOR_SHADER_CUTOUT_BLUR_EDGE,
    COLOR_SHADER_CUTOUT_ROUNDED_CORNER_BLUR_EDGE,
    COLOR_SHADER_CUTOUT_SQUIRCLE_CORNER_BLUR_EDGE,
    COLOR_SHADER_CUTOUT_BORDERLINE_BLUR_EDGE,
    COLOR_SHADER_CUTOUT_ROUNDED_BORDERLINE_BLUR_EDGE,
    COLOR_SHADER_CUTOUT_SQUIRCLE_BORDERLINE_BLUR_EDGE,
    SHADER_TYPE_MAX
  };

  enum GeometryType
  {
    QUAD_GEOMETRY,
    GEOMETRY_TYPE_MAX
  };

  VisualFactoryCache(bool preMultiplyOnLoad);
  ~VisualFactoryCache();

  /**
   * @brief Get cached shader. Returns empty handle if not found.
   */
  Shader GetShader(ShaderType type);

  /**
   * @brief Cache a shader.
   */
  void SaveShader(ShaderType type, Shader shader);

  /**
   * @brief Get cached geometry. Returns empty handle if not found.
   */
  Geometry GetGeometry(GeometryType type);

  /**
   * @brief Cache a geometry.
   */
  void SaveGeometry(GeometryType type, Geometry geometry);

  /**
   * @brief Create and return the default quad geometry.
   */
  static Geometry CreateQuadGeometry();

  /**
   * @brief Get the color visual shader factory.
   */
  ColorVisualShaderFactory& GetColorVisualShaderFactory();

  /**
   * @brief Returns whether the pre-multiply on load flag is set.
   */
  bool GetPreMultiplyOnLoad() const;

  /**
   * @brief Sets the pre-multiply on load flag.
   */
  void SetPreMultiplyOnLoad(bool preMultiply);

  VisualFactoryCache(const VisualFactoryCache&) = delete;
  VisualFactoryCache& operator=(const VisualFactoryCache&) = delete;

private:
  Shader mShaderCache[SHADER_TYPE_MAX];
  Geometry mGeometryCache[GEOMETRY_TYPE_MAX];

  std::unique_ptr<ColorVisualShaderFactory> mColorVisualShaderFactory;

  bool mPreMultiplyOnLoad;
};

} // namespace Internal
} // namespace UI
} // namespace Dali

#endif // DALI_UI_INTERNAL_VISUAL_FACTORY_CACHE_H
