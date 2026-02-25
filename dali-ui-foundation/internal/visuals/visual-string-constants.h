#ifndef DALI_UI_INTERNAL_VISUAL_STRING_CONSTANTS_H
#define DALI_UI_INTERNAL_VISUAL_STRING_CONSTANTS_H

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
#include <dali/devel-api/scripting/enum-helper.h>

namespace Dali
{
namespace UI
{
namespace Internal
{

// Visual type
extern const char* const VISUAL_TYPE;
extern const Dali::Scripting::StringEnum VISUAL_TYPE_TABLE[];
extern const unsigned int VISUAL_TYPE_TABLE_COUNT;

// Custom shader
extern const char* const CUSTOM_SHADER;
extern const char* const CUSTOM_VERTEX_SHADER;
extern const char* const CUSTOM_FRAGMENT_SHADER;
extern const char* const CUSTOM_SUBDIVIDE_GRID_X;
extern const char* const CUSTOM_SUBDIVIDE_GRID_Y;
extern const char* const CUSTOM_RENDER_PASS_TAG;
extern const char* const CUSTOM_SHADER_HINTS;
extern const char* const CUSTOM_SHADER_NAME;

// Transform
extern const char* const TRANSFORM;
extern const char* const SIZE;
extern const char* const OFFSET;
extern const char* const OFFSET_SIZE_MODE;
extern const char* const ORIGIN;
extern const char* const ANCHOR_POINT;
extern const char* const EXTRA_SIZE;

// Premultiplied alpha
extern const char* const PREMULTIPLIED_ALPHA;

// Mix color
extern const char* const MIX_COLOR;
extern const char* const OPACITY;

// Fitting mode
extern const char* const VISUAL_FITTING_MODE;

// Border line
extern const char* const BORDERLINE_WIDTH;
extern const char* const BORDERLINE_COLOR;
extern const char* const BORDERLINE_OFFSET;

// Corner radius
extern const char* const CORNER_RADIUS;
extern const char* const CORNER_RADIUS_POLICY;
extern const char* const CORNER_SQUARENESS;

// Color visual
extern const char* const BLUR_RADIUS_NAME;
extern const char* const CUTOUT_POLICY_NAME;
extern const char* const CUTOUT_CORNER_RADIUS_UNIFORM_NAME;

} // namespace Internal
} // namespace UI
} // namespace Dali

#endif // DALI_UI_INTERNAL_VISUAL_STRING_CONSTANTS_H
