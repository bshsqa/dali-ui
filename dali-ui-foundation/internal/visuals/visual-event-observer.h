#ifndef DALI_UI_INTERNAL_VISUAL_EVENT_OBSERVER_H
#define DALI_UI_INTERNAL_VISUAL_EVENT_OBSERVER_H

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
 */

// EXTERNAL INCLUDES
#include <dali/public-api/object/property-value.h>

namespace Dali
{
namespace UI
{
namespace Internal
{
namespace Visual
{
class Base;

class EventObserver
{
public:
  virtual void ResourceReady(Visual::Base& object) = 0;
  virtual void NotifyVisualEvent(Visual::Base& object, Property::Index signalId) = 0;
  virtual void RelayoutRequest(Visual::Base& object) = 0;

protected:
  EventObserver()
  {
  }
  virtual ~EventObserver()
  {
  }
  EventObserver(const EventObserver&) = delete;
  EventObserver& operator=(const EventObserver&) = delete;
};

} // namespace Visual
} // namespace Internal
} // namespace UI
} // namespace Dali

#endif // DALI_UI_INTERNAL_VISUAL_EVENT_OBSERVER_H
