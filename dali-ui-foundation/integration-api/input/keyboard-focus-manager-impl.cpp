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
#include "keyboard-focus-manager-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/lifecycle-controller.h>
#include <dali/devel-api/common/singleton-service.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/events/key-event.h>
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/integration-api/input/focus-finder.h>

namespace Dali
{
namespace UI
{
namespace Internal
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_UI_KEYBOARD_FOCUS_MANAGER");
#endif

const char* const IS_FOCUS_GROUP_PROPERTY_NAME = "isKeyboardFocusGroup";

// Key name constants
constexpr const char* KEY_NAME_LEFT      = "Left";
constexpr const char* KEY_NAME_RIGHT     = "Right";
constexpr const char* KEY_NAME_UP        = "Up";
constexpr const char* KEY_NAME_DOWN      = "Down";
constexpr const char* KEY_NAME_TAB       = "Tab";
constexpr const char* KEY_NAME_SPACE     = "space";
constexpr const char* KEY_NAME_BACKSPACE = "Backspace";
constexpr const char* KEY_NAME_ESCAPE    = "Escape";
constexpr const char* KEY_NAME_RETURN    = "Return";

} // unnamed namespace

UI::KeyboardFocusManager KeyboardFocusManager::Get()
{
  UI::KeyboardFocusManager manager;

  SingletonService singletonService(SingletonService::Get());
  if(singletonService)
  {
    Dali::BaseHandle handle = singletonService.GetSingleton(typeid(UI::KeyboardFocusManager));
    if(handle)
    {
      manager = UI::KeyboardFocusManager(dynamic_cast<KeyboardFocusManager*>(handle.GetObjectPtr()));
    }
  }

  return manager;
}

KeyboardFocusManager::KeyboardFocusManager()
: mPreFocusChangeSignal(),
  mFocusChangedSignal(),
  mFocusGroupChangedSignal(),
  mFocusedActorEnterKeySignal(),
  mCurrentFocusActor(),
  mFocusIndicatorActor(),
  mFocusFinderRootActor(),
  mFocusHistory(),
  mSlotDelegate(this),
  mCurrentFocusedWindow(),
  mIsFocusIndicatorShown(HIDE),
  mFocusGroupLoopEnabled(false),
  mIsWaitingKeyboardFocusChangeCommit(false),
  mClearFocusOnTouch(true),
  mEnableDefaultAlgorithm(false),
  mClearFocusOnWindowFocusLost(true),
  mCurrentWindowId(0)
{
  LifecycleController::Get().PreInitSignal().Connect(mSlotDelegate, &KeyboardFocusManager::OnAdaptorInit);
}

void KeyboardFocusManager::OnAdaptorInit()
{
  if(Adaptor::IsAvailable())
  {
    Dali::SceneHolderList sceneHolders = Adaptor::Get().GetSceneHolders();
    for(auto iter = sceneHolders.begin(); iter != sceneHolders.end(); ++iter)
    {
      (*iter).KeyEventSignal().Connect(mSlotDelegate, &KeyboardFocusManager::OnKeyEvent);
      (*iter).TouchedSignal().Connect(mSlotDelegate, &KeyboardFocusManager::OnTouch);
      Window window = Window::DownCast(*iter);
      if(window)
      {
        window.FocusChangeSignal().Connect(mSlotDelegate, &KeyboardFocusManager::OnWindowFocusChanged);
      }
    }

    Adaptor::Get().WindowCreatedSignal().Connect(mSlotDelegate, &KeyboardFocusManager::OnSceneHolderCreated);
  }
}

void KeyboardFocusManager::OnSceneHolderCreated(Dali::Integration::SceneHolder& sceneHolder)
{
  sceneHolder.KeyEventSignal().Connect(mSlotDelegate, &KeyboardFocusManager::OnKeyEvent);
  sceneHolder.TouchedSignal().Connect(mSlotDelegate, &KeyboardFocusManager::OnTouch);
  Window window = Window::DownCast(sceneHolder);
  if(window)
  {
    window.FocusChangeSignal().Connect(mSlotDelegate, &KeyboardFocusManager::OnWindowFocusChanged);
  }
}

KeyboardFocusManager::~KeyboardFocusManager()
{
}

bool KeyboardFocusManager::SetCurrentFocusActor(Actor actor)
{
  DALI_ASSERT_DEBUG(!mIsWaitingKeyboardFocusChangeCommit && "Calling this function in the PreFocusChangeSignal callback?");
  return DoSetCurrentFocusActor(actor);
}

bool KeyboardFocusManager::DoSetCurrentFocusActor(Actor actor)
{
  bool                           success = false;
  Dali::Integration::SceneHolder currentWindow;

  if(actor &&
     actor.GetProperty<bool>(Actor::Property::KEYBOARD_FOCUSABLE) &&
     actor.GetProperty<bool>(DevelActor::Property::USER_INTERACTION_ENABLED) &&
     actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE) &&
     (currentWindow = Dali::Integration::SceneHolder::Get(actor)))
  {
    // If the parent's KEYBOARD_FOCUSABLE_CHILDREN is false, it cannot have focus.
    Actor parent = actor.GetParent();
    while(parent)
    {
      if(!parent.GetProperty<bool>(DevelActor::Property::KEYBOARD_FOCUSABLE_CHILDREN))
      {
        return false;
      }
      parent = parent.GetParent();
    }

    Actor currentFocusedActor = GetCurrentFocusActor();
    if(actor == currentFocusedActor)
    {
      return true;
    }

    if(currentWindow.GetRootLayer() != mCurrentFocusedWindow.GetHandle())
    {
      Layer rootLayer       = currentWindow.GetRootLayer();
      mCurrentFocusedWindow = rootLayer;
      mCurrentWindowId      = static_cast<uint32_t>(currentWindow.GetNativeId());
    }

    if(mIsFocusIndicatorShown == SHOW)
    {
      actor.Add(GetFocusIndicatorActor());
    }

    actor.OffSceneSignal().Connect(mSlotDelegate, &KeyboardFocusManager::OnSceneDisconnection);

    // Save the current focused actor
    mCurrentFocusActor = actor;

    bool focusedWindowFound = false;
    for(unsigned int i = 0; i < mCurrentFocusActors.size(); i++)
    {
      if(mCurrentFocusActors[i].first == mCurrentFocusedWindow)
      {
        mCurrentFocusActors[i].second = actor;
        focusedWindowFound            = true;
        break;
      }
    }
    if(!focusedWindowFound)
    {
      mCurrentFocusActors.push_back(std::pair<WeakHandle<Layer>, WeakHandle<Actor> >(mCurrentFocusedWindow, actor));
    }

    // Send notification for the change of focus actor
    if(!mFocusChangedSignal.Empty())
    {
      mFocusChangedSignal.Emit(currentFocusedActor, actor);
    }

    // Notify old view
    if(currentFocusedActor)
    {
      UI::View oldView = UI::View::DownCast(currentFocusedActor);
      if(oldView)
      {
        Integration::GetImpl(oldView).OnKeyInputFocusLost();
      }
    }

    // Notify new view
    UI::View newView = UI::View::DownCast(actor);
    if(newView)
    {
      Integration::GetImpl(newView).OnKeyInputFocusGained();
    }

    // Push Current Focused Actor to FocusHistory
    mFocusHistory.push_back(actor);

    if(mFocusHistory.size() > MAX_HISTORY_AMOUNT)
    {
      FocusStackIterator beginPos = mFocusHistory.begin();
      mFocusHistory.erase(beginPos);
    }

    DALI_LOG_INFO(gLogFilter, Debug::General, "[%s:%d] SUCCEED\n", __FUNCTION__, __LINE__);
    success = true;
  }
  else
  {
    DALI_LOG_WARNING("[%s:%d] FAILED\n", __FUNCTION__, __LINE__);
  }

  return success;
}

Actor KeyboardFocusManager::GetCurrentFocusActor()
{
  Actor actor = mCurrentFocusActor.GetHandle();

  if(actor && !actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    actor.Reset();
    mCurrentFocusActor.Reset();
  }
  return actor;
}

Actor KeyboardFocusManager::GetFocusActorFromCurrentWindow()
{
  Actor        actor;
  unsigned int index;
  for(index = 0; index < mCurrentFocusActors.size(); index++)
  {
    if(mCurrentFocusActors[index].first == mCurrentFocusedWindow)
    {
      actor = mCurrentFocusActors[index].second.GetHandle();
      break;
    }
  }

  if(actor && !actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    actor.Reset();
    mCurrentFocusActors.erase(mCurrentFocusActors.begin() + index);
  }

  return actor;
}

Actor KeyboardFocusManager::GetCurrentFocusGroup()
{
  return GetFocusGroup(GetCurrentFocusActor());
}

void KeyboardFocusManager::MoveFocusBackward()
{
  if(mFocusHistory.size() > 1)
  {
    mFocusHistory.pop_back();

    while(mFocusHistory.size() > 0)
    {
      Actor target = mFocusHistory[mFocusHistory.size() - 1].GetHandle();

      if(target && target.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
      {
        mFocusHistory.pop_back();
        SetCurrentFocusActor(target);
        break;
      }
      else
      {
        mFocusHistory.pop_back();
      }
    }

    if(mFocusHistory.size() == 0)
    {
      Actor currentFocusedActor = GetCurrentFocusActor();
      mFocusHistory.push_back(currentFocusedActor);
    }
  }
}

bool KeyboardFocusManager::IsLayoutControl(Actor actor) const
{
  UI::View view = UI::View::DownCast(actor);
  return view && Integration::GetImpl(view).IsKeyboardNavigationSupported();
}

UI::View KeyboardFocusManager::GetParentLayoutControl(Actor actor) const
{
  Actor rootActor;
  Actor parent;
  if(actor)
  {
    Dali::Integration::SceneHolder window = Dali::Integration::SceneHolder::Get(actor);
    if(window)
    {
      rootActor = window.GetRootLayer();
    }

    parent = actor.GetParent();
  }

  while(parent && !IsLayoutControl(parent) && parent != rootActor)
  {
    parent = parent.GetParent();
  }

  return UI::View::DownCast(parent);
}

bool KeyboardFocusManager::MoveFocus(UI::Focus::Direction direction)
{
  Actor currentFocusActor = GetCurrentFocusActor();

  bool succeed = false;

  // Go through the actor's hierarchy until we find a layout control that knows how to move the focus
  UI::View layoutControl = IsLayoutControl(currentFocusActor) ? UI::View::DownCast(currentFocusActor) : GetParentLayoutControl(currentFocusActor);
  while(layoutControl && !succeed)
  {
    succeed       = DoMoveFocusWithinLayoutControl(layoutControl, currentFocusActor, direction);
    layoutControl = GetParentLayoutControl(layoutControl);
  }

  if(!succeed)
  {
    Actor nextFocusableActor;

    if(!nextFocusableActor)
    {
      if(!mPreFocusChangeSignal.Empty())
      {
        mIsWaitingKeyboardFocusChangeCommit = true;
        nextFocusableActor                  = mPreFocusChangeSignal.Emit(currentFocusActor, Actor(), direction);
        mIsWaitingKeyboardFocusChangeCommit = false;
      }
      else if(mEnableDefaultAlgorithm)
      {
        Actor rootActor = mFocusFinderRootActor.GetHandle();
        if(!rootActor)
        {
          if(currentFocusActor)
          {
            Dali::Integration::SceneHolder window = Dali::Integration::SceneHolder::Get(currentFocusActor);
            if(window)
            {
              rootActor = window.GetRootLayer();
            }
          }
          else
          {
            rootActor = mCurrentFocusedWindow.GetHandle();
          }
        }
        if(rootActor)
        {
          nextFocusableActor = FocusFinder::GetNearestFocusableActor(rootActor, currentFocusActor, direction);
        }
      }
    }

    if(nextFocusableActor &&
       nextFocusableActor.GetProperty<bool>(Actor::Property::KEYBOARD_FOCUSABLE) &&
       nextFocusableActor.GetProperty<bool>(DevelActor::Property::USER_INTERACTION_ENABLED))
    {
      if(IsLayoutControl(nextFocusableActor))
      {
        UI::View layoutView = UI::View::DownCast(nextFocusableActor);
        succeed = DoMoveFocusWithinLayoutControl(layoutView, currentFocusActor, direction);
      }
      if(!succeed)
      {
        succeed = DoSetCurrentFocusActor(nextFocusableActor);
      }
    }
  }

  return succeed;
}

bool KeyboardFocusManager::DoMoveFocusWithinLayoutControl(UI::View control, Actor actor, UI::Focus::Direction direction)
{
  Actor nextFocusableActor = Integration::GetImpl(control).GetNextKeyboardFocusableActor(actor, direction, mFocusGroupLoopEnabled);
  if(nextFocusableActor)
  {
    if(!(nextFocusableActor.GetProperty<bool>(Actor::Property::KEYBOARD_FOCUSABLE) ||
         nextFocusableActor.GetProperty<bool>(DevelActor::Property::USER_INTERACTION_ENABLED)))
    {
      return DoMoveFocusWithinLayoutControl(control, nextFocusableActor, direction);
    }
    else
    {
      Actor currentFocusActor   = GetCurrentFocusActor();
      Actor committedFocusActor = nextFocusableActor;

      if(!mPreFocusChangeSignal.Empty())
      {
        mIsWaitingKeyboardFocusChangeCommit = true;
        committedFocusActor                 = mPreFocusChangeSignal.Emit(currentFocusActor, nextFocusableActor, direction);
        mIsWaitingKeyboardFocusChangeCommit = false;
      }

      if(committedFocusActor &&
         committedFocusActor.GetProperty<bool>(Actor::Property::KEYBOARD_FOCUSABLE) &&
         committedFocusActor.GetProperty<bool>(DevelActor::Property::USER_INTERACTION_ENABLED))
      {
        if(IsLayoutControl(committedFocusActor) && UI::View::DownCast(committedFocusActor) != control)
        {
          UI::View layoutView = UI::View::DownCast(committedFocusActor);
          return DoMoveFocusWithinLayoutControl(layoutView, currentFocusActor, direction);
        }
        else
        {
          if(committedFocusActor == nextFocusableActor)
          {
            Integration::GetImpl(control).OnKeyboardFocusChangeCommitted(committedFocusActor);
          }
          return DoSetCurrentFocusActor(committedFocusActor);
        }
      }
      else
      {
        return false;
      }
    }
  }
  else
  {
    return false;
  }
}

bool KeyboardFocusManager::DoMoveFocusToNextFocusGroup(bool forward)
{
  bool succeed = false;

  UI::View parentLayoutControl = GetParentLayoutControl(GetCurrentFocusGroup());

  while(parentLayoutControl && !succeed)
  {
    UI::Focus::Direction direction = forward ? UI::Focus::Direction::RIGHT : UI::Focus::Direction::LEFT;
    succeed                        = DoMoveFocusWithinLayoutControl(parentLayoutControl, GetCurrentFocusActor(), direction);
    parentLayoutControl            = GetParentLayoutControl(parentLayoutControl);
  }

  if(!mFocusGroupChangedSignal.Empty())
  {
    mFocusGroupChangedSignal.Emit(GetCurrentFocusActor(), forward);
  }

  return succeed;
}

void KeyboardFocusManager::DoKeyboardEnter(Actor actor)
{
  if(actor)
  {
    UI::View view = UI::View::DownCast(actor);
    if(view)
    {
      Integration::GetImpl(view).OnKeyboardEnter();
    }

    if(!mFocusedActorEnterKeySignal.Empty())
    {
      mFocusedActorEnterKeySignal.Emit(actor);
    }
  }
}

void KeyboardFocusManager::ClearFocus(Actor actor)
{
  if(actor)
  {
    actor.OffSceneSignal().Disconnect(mSlotDelegate, &KeyboardFocusManager::OnSceneDisconnection);

    for(auto iter = mCurrentFocusActors.begin(); iter != mCurrentFocusActors.end(); ++iter)
    {
      if(iter->first == mCurrentFocusedWindow && iter->second.GetHandle() == actor)
      {
        mCurrentFocusActors.erase(iter);
        break;
      }
    }

    if(!mFocusChangedSignal.Empty())
    {
      mFocusChangedSignal.Emit(actor, Actor());
    }

    UI::View view = UI::View::DownCast(actor);
    if(view)
    {
      Integration::GetImpl(view).OnKeyInputFocusLost();
    }
  }
  mCurrentFocusActor.Reset();
}

void KeyboardFocusManager::ClearFocusIndicator(Actor actor)
{
  if(actor)
  {
    if(mFocusIndicatorActor)
    {
      actor.Remove(mFocusIndicatorActor);
    }
  }
  mIsFocusIndicatorShown = HIDE;
}

void KeyboardFocusManager::ClearFocus()
{
  Actor actor = GetCurrentFocusActor();
  ClearFocusIndicator(actor);
  ClearFocus(actor);
}

void KeyboardFocusManager::SetFocusGroupLoop(bool enabled)
{
  mFocusGroupLoopEnabled = enabled;
}

bool KeyboardFocusManager::GetFocusGroupLoop() const
{
  return mFocusGroupLoopEnabled;
}

void KeyboardFocusManager::SetAsFocusGroup(Actor actor, bool isFocusGroup)
{
  if(actor)
  {
    UI::View view = UI::View::DownCast(actor);
    if(view)
    {
      Integration::GetImpl(view).SetAsKeyboardFocusGroup(isFocusGroup);
    }
    else
    {
      // For non-View actors, use a custom property
      actor.RegisterProperty(IS_FOCUS_GROUP_PROPERTY_NAME, isFocusGroup, Property::READ_WRITE);
    }
  }
}

bool KeyboardFocusManager::IsFocusGroup(Actor actor) const
{
  UI::View view = UI::View::DownCast(actor);
  if(view)
  {
    return Integration::GetImpl(view).IsKeyboardFocusGroup();
  }

  // Check custom property for non-View actors
  Property::Index propertyIsFocusGroup = actor.GetPropertyIndex(IS_FOCUS_GROUP_PROPERTY_NAME);
  if(propertyIsFocusGroup != Property::INVALID_INDEX)
  {
    return actor.GetProperty<bool>(propertyIsFocusGroup);
  }
  return false;
}

Actor KeyboardFocusManager::GetFocusGroup(Actor actor)
{
  Actor parent = actor;
  while(parent && !IsFocusGroup(parent))
  {
    parent = parent.GetParent();
  }
  return parent;
}

void KeyboardFocusManager::SetFocusIndicatorActor(Actor indicator)
{
  if(mFocusIndicatorActor != indicator)
  {
    Actor currentFocusActor = GetCurrentFocusActor();
    if(mFocusIndicatorActor)
    {
      if(currentFocusActor)
      {
        currentFocusActor.Remove(mFocusIndicatorActor);
      }
    }

    mFocusIndicatorActor = indicator;

    if(currentFocusActor && mIsFocusIndicatorShown == SHOW)
    {
      currentFocusActor.Add(mFocusIndicatorActor);
    }
  }
}

Actor KeyboardFocusManager::GetFocusIndicatorActor()
{
  if(!mFocusIndicatorActor)
  {
    // Create the default focus indicator actor (a simple renderable)
    // In actual production, could load an image or 9-patch
  }
  return mFocusIndicatorActor;
}

void KeyboardFocusManager::EnableDefaultAlgorithm(bool enable)
{
  mEnableDefaultAlgorithm = enable;
}

bool KeyboardFocusManager::IsDefaultAlgorithmEnabled() const
{
  return mEnableDefaultAlgorithm;
}

void KeyboardFocusManager::SetClearFocusOnWindowFocusLost(bool enabled)
{
  mClearFocusOnWindowFocusLost = enabled;
}

bool KeyboardFocusManager::GetClearFocusOnWindowFocusLost() const
{
  return mClearFocusOnWindowFocusLost;
}

uint32_t KeyboardFocusManager::GetCurrentWindowId() const
{
  return mCurrentWindowId;
}

void KeyboardFocusManager::OnKeyEvent(const KeyEvent& event)
{
  const std::string& keyName = event.GetKeyName();

  if(event.GetState() == KeyEvent::DOWN)
  {
    UI::Focus::Direction direction;
    bool                 isFocusDirection = false;

    if(keyName == KEY_NAME_LEFT)
    {
      direction        = UI::Focus::Direction::LEFT;
      isFocusDirection = true;
    }
    else if(keyName == KEY_NAME_RIGHT)
    {
      direction        = UI::Focus::Direction::RIGHT;
      isFocusDirection = true;
    }
    else if(keyName == KEY_NAME_UP)
    {
      direction        = UI::Focus::Direction::UP;
      isFocusDirection = true;
    }
    else if(keyName == KEY_NAME_DOWN)
    {
      direction        = UI::Focus::Direction::DOWN;
      isFocusDirection = true;
    }
    else if(keyName == KEY_NAME_TAB)
    {
      if(event.IsShiftModifier())
      {
        direction = UI::Focus::Direction::BACKWARD;
      }
      else
      {
        direction = UI::Focus::Direction::FORWARD;
      }
      isFocusDirection = true;
    }
    else if(keyName == KEY_NAME_RETURN)
    {
      DoKeyboardEnter(GetCurrentFocusActor());
    }

    if(isFocusDirection)
    {
      // Show the focus indicator when tab or direction key is pressed
      if(mIsFocusIndicatorShown == HIDE)
      {
        mIsFocusIndicatorShown = SHOW;

        Actor actor = GetCurrentFocusActor();
        if(actor)
        {
          actor.Add(GetFocusIndicatorActor());
        }
        else
        {
          // No actor is focused, try setting focus to the first focusable actor
          Actor rootActor = mCurrentFocusedWindow.GetHandle();
          if(!rootActor)
          {
            if(Adaptor::IsAvailable())
            {
              Dali::SceneHolderList sceneHolders = Adaptor::Get().GetSceneHolders();
              if(!sceneHolders.empty())
              {
                rootActor = sceneHolders[0].GetRootLayer();
              }
            }
          }
        }
      }

      MoveFocus(direction);
    }
  }
}

void KeyboardFocusManager::OnTouch(const TouchEvent& touch)
{
  if(mClearFocusOnTouch)
  {
    ClearFocusIndicator(GetCurrentFocusActor());
  }
}

void KeyboardFocusManager::OnWindowFocusChanged(Window window, bool focusIn)
{
  if(focusIn)
  {
    Layer rootLayer       = window.GetRootLayer();
    mCurrentFocusedWindow = rootLayer;
    mCurrentWindowId      = static_cast<uint32_t>(Dali::Integration::SceneHolder::Get(rootLayer).GetNativeId());

    // Retrieve the saved focus actor for this window
    Actor savedFocusActor = GetFocusActorFromCurrentWindow();
    if(savedFocusActor)
    {
      SetCurrentFocusActor(savedFocusActor);
    }
  }
  else
  {
    if(mClearFocusOnWindowFocusLost)
    {
      ClearFocusIndicator(GetCurrentFocusActor());
    }
  }
}

void KeyboardFocusManager::OnSceneDisconnection(Dali::Actor actor)
{
  Actor currentFocusActor = GetCurrentFocusActor();
  if(currentFocusActor == actor)
  {
    ClearFocusIndicator(actor);
    ClearFocus(actor);
  }
}

// Signals
UI::KeyboardFocusManager::PreFocusChangeSignalType& KeyboardFocusManager::PreFocusChangeSignal()
{
  return mPreFocusChangeSignal;
}

UI::KeyboardFocusManager::FocusChangedSignalType& KeyboardFocusManager::FocusChangedSignal()
{
  return mFocusChangedSignal;
}

UI::KeyboardFocusManager::FocusGroupChangedSignalType& KeyboardFocusManager::FocusGroupChangedSignal()
{
  return mFocusGroupChangedSignal;
}

UI::KeyboardFocusManager::FocusedActorEnterKeySignalType& KeyboardFocusManager::FocusedActorEnterKeySignal()
{
  return mFocusedActorEnterKeySignal;
}

} // namespace Internal

} // namespace UI

} // namespace Dali
