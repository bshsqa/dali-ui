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
#include <dali-ui-foundation/internal/visuals/visual-string-constants.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/visuals/visual-factory-cache.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace UI
{
namespace Internal
{

// Visual type
DALI_ENUM_TO_STRING_TABLE_BEGIN(VISUAL_TYPE)
DALI_ENUM_TO_STRING_WITH_SCOPE(UI::Visual, BORDER)
DALI_ENUM_TO_STRING_WITH_SCOPE(UI::Visual, COLOR)
DALI_ENUM_TO_STRING_WITH_SCOPE(UI::Visual, GRADIENT)
DALI_ENUM_TO_STRING_WITH_SCOPE(UI::Visual, IMAGE)
DALI_ENUM_TO_STRING_WITH_SCOPE(UI::Visual, MESH)
DALI_ENUM_TO_STRING_WITH_SCOPE(UI::Visual, PRIMITIVE)
DALI_ENUM_TO_STRING_WITH_SCOPE(UI::Visual, WIREFRAME)
DALI_ENUM_TO_STRING_WITH_SCOPE(UI::Visual, TEXT)
DALI_ENUM_TO_STRING_WITH_SCOPE(UI::Visual, N_PATCH)
DALI_ENUM_TO_STRING_WITH_SCOPE(UI::Visual, SVG)
DALI_ENUM_TO_STRING_WITH_SCOPE(UI::Visual, ANIMATED_IMAGE)
DALI_ENUM_TO_STRING_TABLE_END(VISUAL_TYPE)

// Custom shader
const char* const CUSTOM_SHADER = "shader";
const char* const CUSTOM_VERTEX_SHADER = "vertexShader";
const char* const CUSTOM_FRAGMENT_SHADER = "fragmentShader";
const char* const CUSTOM_SUBDIVIDE_GRID_X = "subdivideGridX";
const char* const CUSTOM_SUBDIVIDE_GRID_Y = "subdivideGridY";
const char* const CUSTOM_RENDER_PASS_TAG = "renderPassTag";
const char* const CUSTOM_SHADER_HINTS = "hints";
const char* const CUSTOM_SHADER_NAME = "name";

// Transform
const char* const TRANSFORM = "transform";
const char* const SIZE = "size";
const char* const OFFSET = "offset";
const char* const OFFSET_SIZE_MODE = "offsetSizeMode";
const char* const ORIGIN = "origin";
const char* const ANCHOR_POINT = "anchorPoint";
const char* const EXTRA_SIZE = "extraSize";

// Premultiplied alpha
const char* const PREMULTIPLIED_ALPHA = "premultipliedAlpha";

// Mix color
const char* const MIX_COLOR = "mixColor";
const char* const OPACITY = "opacity";

// Fitting mode
const char* const VISUAL_FITTING_MODE = "fittingMode";

// Border line
const char* const BORDERLINE_WIDTH = "borderlineWidth";
const char* const BORDERLINE_COLOR = "borderlineColor";
const char* const BORDERLINE_OFFSET = "borderlineOffset";

// Corner radius
const char* const CORNER_RADIUS = "cornerRadius";
const char* const CORNER_RADIUS_POLICY = "cornerRadiusPolicy";
const char* const CORNER_SQUARENESS = "cornerSquareness";

// Color visual
const char* const BLUR_RADIUS_NAME = "blurRadius";
const char* const CUTOUT_POLICY_NAME = "cutoutPolicy";
const char* const CUTOUT_CORNER_RADIUS_UNIFORM_NAME = "cutoutCornerRadius";

} // namespace Internal
} // namespace UI
} // namespace Dali
