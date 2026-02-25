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
#include <dali-ui-foundation/internal/visuals/visual-factory-cache.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/visuals/color/color-visual-shader-factory.h>

// EXTERNAL INCLUDES
#include <dali/public-api/rendering/vertex-buffer.h>

namespace Dali
{
namespace UI
{
namespace Internal
{

VisualFactoryCache::VisualFactoryCache(bool preMultiplyOnLoad)
  : mPreMultiplyOnLoad(preMultiplyOnLoad)
{
}

VisualFactoryCache::~VisualFactoryCache() = default;

Shader VisualFactoryCache::GetShader(ShaderType type)
{
  if (type < SHADER_TYPE_MAX)
  {
    return mShaderCache[type];
  }
  return Shader();
}

void VisualFactoryCache::SaveShader(ShaderType type, Shader shader)
{
  if (type < SHADER_TYPE_MAX)
  {
    mShaderCache[type] = shader;
  }
}

Geometry VisualFactoryCache::GetGeometry(GeometryType type)
{
  if (type < GEOMETRY_TYPE_MAX)
  {
    return mGeometryCache[type];
  }
  return Geometry();
}

void VisualFactoryCache::SaveGeometry(GeometryType type, Geometry geometry)
{
  if (type < GEOMETRY_TYPE_MAX)
  {
    mGeometryCache[type] = geometry;
  }
}

Geometry VisualFactoryCache::CreateQuadGeometry()
{
  const float halfWidth = 0.5f;
  const float halfHeight = 0.5f;

  struct QuadVertex
  {
    Vector2 position;
  };

  QuadVertex quadVertexData[4] = {
      {Vector2(-halfWidth, -halfHeight)},
      {Vector2(-halfWidth, halfHeight)},
      {Vector2(halfWidth, -halfHeight)},
      {Vector2(halfWidth, halfHeight)},
  };

  Property::Map vertexFormat;
  vertexFormat["aPosition"] = Property::VECTOR2;

  VertexBuffer vertexBuffer = VertexBuffer::New(vertexFormat);
  vertexBuffer.SetData(quadVertexData, 4);

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer(vertexBuffer);
  geometry.SetType(Geometry::TRIANGLE_STRIP);

  return geometry;
}

ColorVisualShaderFactory& VisualFactoryCache::GetColorVisualShaderFactory()
{
  if (!mColorVisualShaderFactory)
  {
    mColorVisualShaderFactory = std::make_unique<ColorVisualShaderFactory>();
  }
  return *mColorVisualShaderFactory;
}

bool VisualFactoryCache::GetPreMultiplyOnLoad() const
{
  return mPreMultiplyOnLoad;
}

void VisualFactoryCache::SetPreMultiplyOnLoad(bool preMultiply)
{
  mPreMultiplyOnLoad = preMultiply;
}

} // namespace Internal
} // namespace UI
} // namespace Dali
