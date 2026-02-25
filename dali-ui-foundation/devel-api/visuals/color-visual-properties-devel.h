#ifndef DALI_UI_COLOR_VISUAL_PROPERTIES_DEVEL_H
#define DALI_UI_COLOR_VISUAL_PROPERTIES_DEVEL_H

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
#include <dali-ui-foundation/public-api/visuals/color-visual-properties.h>

namespace Dali
{
namespace UI
{
namespace DevelColorVisual
{

namespace Property
{

enum
{
  MIX_COLOR = UI::ColorVisual::Property::MIX_COLOR,

  BLUR_RADIUS = MIX_COLOR + 2,
  CUTOUT_POLICY = MIX_COLOR + 3,
};

} // namespace Property

namespace CutoutPolicy
{

enum Type
{
  NONE,
  CUTOUT_VIEW,
  CUTOUT_VIEW_WITH_CORNER_RADIUS,
  CUTOUT_OUTSIDE,
  CUTOUT_OUTSIDE_WITH_CORNER_RADIUS,
};

} // namespace CutoutPolicy

} // namespace DevelColorVisual
} // namespace UI
} // namespace Dali

#endif // DALI_UI_COLOR_VISUAL_PROPERTIES_DEVEL_H
