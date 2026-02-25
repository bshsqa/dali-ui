#ifndef DALI_UI_VISUAL_PROPERTIES_H
#define DALI_UI_VISUAL_PROPERTIES_H

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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/ui-property-index-ranges.h>

namespace Dali
{
namespace UI
{

namespace Visual
{

enum Type
{
  BORDER,
  COLOR,
  GRADIENT,
  IMAGE,
  MESH,
  PRIMITIVE,
  WIREFRAME,
  TEXT,
  N_PATCH,
  SVG,
  ANIMATED_IMAGE,
};

namespace Property
{

enum
{
  TYPE = VISUAL_PROPERTY_BASE_START_INDEX,
  SHADER,
  TRANSFORM,
  PREMULTIPLIED_ALPHA,
  MIX_COLOR,
  OPACITY,
};

} // namespace Property

namespace Transform
{

namespace Policy
{

enum Type
{
  RELATIVE = 0,
  ABSOLUTE = 1
};

} // namespace Policy

namespace Property
{

enum Type
{
  OFFSET,
  SIZE,
  ORIGIN,
  ANCHOR_POINT,
  OFFSET_POLICY,
  SIZE_POLICY,
};

} // namespace Property

} // namespace Transform

namespace Shader
{
namespace Property
{

enum
{
  VERTEX_SHADER,
  FRAGMENT_SHADER,
  SUBDIVIDE_GRID_X,
  SUBDIVIDE_GRID_Y,
  RENDER_PASS_TAG,
  HINTS,
  NAME,
};

} // namespace Property
} // namespace Shader

enum class ResourceStatus
{
  PREPARING,
  READY,
  FAILED
};

} // namespace Visual

} // namespace UI
} // namespace Dali

#endif // DALI_UI_VISUAL_PROPERTIES_H
