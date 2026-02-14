#ifndef DALI_UI_FOCUS_DIRECTION_H
#define DALI_UI_FOCUS_DIRECTION_H

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

// INTERAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace UI
{
/**
 * @brief Focus functionality
 * @SINCE_1_0.0
 */
namespace Focus
{
/**
 * @brief Focus direction.
 * @SINCE_1_0.0
 */
enum class Direction
{
  LEFT,              ///< Move focus towards the left direction @SINCE_1_0.0
  RIGHT,             ///< Move focus towards the right direction @SINCE_1_0.0
  UP,                ///< Move focus towards the up direction @SINCE_1_0.0
  DOWN,              ///< Move focus towards the down direction @SINCE_1_0.0
  PAGE_UP,           ///< Move focus towards the previous page direction @SINCE_1_0.0
  PAGE_DOWN,         ///< Move focus towards the next page direction @SINCE_1_0.0
  FORWARD,           ///< Move focus towards the forward direction @SINCE_1_0.0
  BACKWARD,          ///< Move focus towards the backward direction @SINCE_1_0.0
  CLOCKWISE,         ///< Move focus towards the clockwise direction @SINCE_1_0.0
  COUNTER_CLOCKWISE, ///< Move focus towards the counter clockwise direction @SINCE_1_0.0
};

} // namespace Focus

} // namespace UI

} // namespace Dali

#endif // DALI_UI_FOCUS_DIRECTION_H
