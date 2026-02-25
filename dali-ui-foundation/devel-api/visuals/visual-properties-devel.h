#ifndef DALI_UI_DEVEL_VISUAL_PROPERTIES_H
#define DALI_UI_DEVEL_VISUAL_PROPERTIES_H

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
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace UI
{
namespace DevelVisual
{

enum Type
{
  BORDER = Dali::UI::Visual::BORDER,
  COLOR = Dali::UI::Visual::COLOR,
  GRADIENT = Dali::UI::Visual::GRADIENT,
  IMAGE = Dali::UI::Visual::IMAGE,
  MESH = Dali::UI::Visual::MESH,
  PRIMITIVE = Dali::UI::Visual::PRIMITIVE,
  WIREFRAME = Dali::UI::Visual::WIREFRAME,
  TEXT = Dali::UI::Visual::TEXT,
  N_PATCH = Dali::UI::Visual::N_PATCH,
  SVG = Dali::UI::Visual::SVG,
  ANIMATED_IMAGE = Dali::UI::Visual::ANIMATED_IMAGE,

  NOT_IN_USE_1 = ANIMATED_IMAGE + 1,
  ANIMATED_VECTOR_IMAGE = ANIMATED_IMAGE + 2,
  ARC = ANIMATED_IMAGE + 3,
};

namespace Property
{

enum Type
{
  TYPE = Dali::UI::Visual::Property::TYPE,
  SHADER = Dali::UI::Visual::Property::SHADER,
  TRANSFORM = Dali::UI::Visual::Property::TRANSFORM,
  PREMULTIPLIED_ALPHA = Dali::UI::Visual::Property::PREMULTIPLIED_ALPHA,
  MIX_COLOR = Dali::UI::Visual::Property::MIX_COLOR,
  OPACITY = Dali::UI::Visual::Property::OPACITY,

  VISUAL_FITTING_MODE = OPACITY + 1,
  CORNER_RADIUS = OPACITY + 2,
  CORNER_RADIUS_POLICY = OPACITY + 3,
  BORDERLINE_WIDTH = OPACITY + 4,
  BORDERLINE_COLOR = OPACITY + 5,
  BORDERLINE_OFFSET = OPACITY + 6,
  CORNER_SQUARENESS = OPACITY + 7,
};

} // namespace Property

enum FittingMode
{
  FIT_KEEP_ASPECT_RATIO,
  FILL,
  OVER_FIT_KEEP_ASPECT_RATIO,
  CENTER,
  FIT_HEIGHT,
  FIT_WIDTH,
  DONT_CARE
};

namespace Transform
{
namespace Property
{

enum Type
{
  SIZE_POLICY = Dali::UI::Visual::Transform::Property::SIZE_POLICY,
  EXTRA_SIZE = SIZE_POLICY + 1,
};

} // namespace Property
} // namespace Transform

} // namespace DevelVisual
} // namespace UI
} // namespace Dali

#endif // DALI_UI_DEVEL_VISUAL_PROPERTIES_H
