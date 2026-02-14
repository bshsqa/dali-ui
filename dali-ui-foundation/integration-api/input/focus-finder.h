#ifndef DALI_UI_INTERNAL_FOCUS_FINDER_H
#define DALI_UI_INTERNAL_FOCUS_FINDER_H

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
#include <dali/public-api/actors/actor.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/focus/focus-direction.h>

namespace Dali
{
namespace UI
{
namespace Internal
{
namespace FocusFinder
{

/**
 * Get the nearest focusable actor.
 * @param[in] rootActor The root actor.
 * @param[in] focusedActor The current focused actor.
 * @param[in] direction The direction.
 * @return The nearest focusable actor, or an empty handle if none exists.
 */
Actor GetNearestFocusableActor(Actor rootActor, Actor focusedActor, UI::Focus::Direction direction);

} // namespace FocusFinder

} // namespace Internal

} // namespace UI

} // namespace Dali

#endif // DALI_UI_INTERNAL_FOCUS_FINDER_H
