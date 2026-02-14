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
#include <dali-ui-foundation/public-api/input/keyboard-focus-manager.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/common/singleton-service.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/input/keyboard-focus-manager-impl.h>

namespace Dali
{
namespace UI
{

KeyboardFocusManager::KeyboardFocusManager()
{
}

KeyboardFocusManager::~KeyboardFocusManager()
{
}

KeyboardFocusManager KeyboardFocusManager::Get()
{
  KeyboardFocusManager manager;

  SingletonService singletonService(SingletonService::Get());
  if(singletonService)
  {
    Dali::BaseHandle handle = singletonService.GetSingleton(typeid(UI::KeyboardFocusManager));
    if(handle)
    {
      manager = KeyboardFocusManager(dynamic_cast<Internal::KeyboardFocusManager*>(handle.GetObjectPtr()));
    }

    if(!manager)
    {
      Internal::KeyboardFocusManager* impl = new Internal::KeyboardFocusManager();
      manager = KeyboardFocusManager(impl);
      singletonService.Register(typeid(manager), manager);
    }
  }

  return manager;
}

bool KeyboardFocusManager::SetCurrentFocusActor(Actor actor)
{
  return GetImpl(*this).SetCurrentFocusActor(actor);
}

Actor KeyboardFocusManager::GetCurrentFocusActor()
{
  return GetImpl(*this).GetCurrentFocusActor();
}

bool KeyboardFocusManager::MoveFocus(Focus::Direction direction)
{
  return GetImpl(*this).MoveFocus(direction);
}

void KeyboardFocusManager::ClearFocus()
{
  GetImpl(*this).ClearFocus();
}

void KeyboardFocusManager::SetFocusGroupLoop(bool enabled)
{
  GetImpl(*this).SetFocusGroupLoop(enabled);
}

bool KeyboardFocusManager::GetFocusGroupLoop() const
{
  return GetImpl(*this).GetFocusGroupLoop();
}

void KeyboardFocusManager::SetAsFocusGroup(Actor actor, bool isFocusGroup)
{
  GetImpl(*this).SetAsFocusGroup(actor, isFocusGroup);
}

bool KeyboardFocusManager::IsFocusGroup(Actor actor) const
{
  return GetImpl(*this).IsFocusGroup(actor);
}

Actor KeyboardFocusManager::GetFocusGroup(Actor actor)
{
  return GetImpl(*this).GetFocusGroup(actor);
}

void KeyboardFocusManager::SetFocusIndicatorActor(Actor indicator)
{
  GetImpl(*this).SetFocusIndicatorActor(indicator);
}

Actor KeyboardFocusManager::GetFocusIndicatorActor()
{
  return GetImpl(*this).GetFocusIndicatorActor();
}

void KeyboardFocusManager::MoveFocusBackward()
{
  GetImpl(*this).MoveFocusBackward();
}

void KeyboardFocusManager::SetClearFocusOnWindowFocusLost(bool enabled)
{
  GetImpl(*this).SetClearFocusOnWindowFocusLost(enabled);
}

bool KeyboardFocusManager::GetClearFocusOnWindowFocusLost() const
{
  return GetImpl(*this).GetClearFocusOnWindowFocusLost();
}

void KeyboardFocusManager::EnableDefaultAlgorithm(bool enable)
{
  GetImpl(*this).EnableDefaultAlgorithm(enable);
}

bool KeyboardFocusManager::IsDefaultAlgorithmEnabled() const
{
  return GetImpl(*this).IsDefaultAlgorithmEnabled();
}

KeyboardFocusManager::PreFocusChangeSignalType& KeyboardFocusManager::PreFocusChangeSignal()
{
  return GetImpl(*this).PreFocusChangeSignal();
}

KeyboardFocusManager::FocusChangedSignalType& KeyboardFocusManager::FocusChangedSignal()
{
  return GetImpl(*this).FocusChangedSignal();
}

KeyboardFocusManager::FocusGroupChangedSignalType& KeyboardFocusManager::FocusGroupChangedSignal()
{
  return GetImpl(*this).FocusGroupChangedSignal();
}

KeyboardFocusManager::FocusedActorEnterKeySignalType& KeyboardFocusManager::FocusedActorEnterKeySignal()
{
  return GetImpl(*this).FocusedActorEnterKeySignal();
}

KeyboardFocusManager::KeyboardFocusManager(Internal::KeyboardFocusManager* impl)
: BaseHandle(impl)
{
}

} // namespace UI

} // namespace Dali
