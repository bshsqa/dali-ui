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
#include <dali-ui-foundation/integration-api/input/keyinput-focus-manager-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/actors/layer.h>
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-impl.h>

namespace Dali
{
namespace UI
{
namespace Internal
{
namespace
{
// Signals

const char* const SIGNAL_KEY_INPUT_FOCUS_CHANGED = "keyInputFocusChanged";

} // namespace

KeyInputFocusManager::KeyInputFocusManager()
: mSlotDelegate(this),
  mCurrentFocusView(),
  mCurrentWindowId(0)
{
  // Retrieve all the existing widnows
  Dali::SceneHolderList sceneHolders = Adaptor::Get().GetSceneHolders();
  for(auto iter = sceneHolders.begin(); iter != sceneHolders.end(); ++iter)
  {
    (*iter).KeyEventGeneratedSignal().Connect(mSlotDelegate, &KeyInputFocusManager::OnKeyEvent);
  }

  // Get notified when any new scene holder is created afterwards
  Adaptor::Get().WindowCreatedSignal().Connect(mSlotDelegate, &KeyInputFocusManager::OnSceneHolderCreated);
}

KeyInputFocusManager::~KeyInputFocusManager()
{
}

void KeyInputFocusManager::OnSceneHolderCreated(Dali::Integration::SceneHolder& sceneHolder)
{
  sceneHolder.KeyEventGeneratedSignal().Connect(mSlotDelegate, &KeyInputFocusManager::OnKeyEvent);
}

void KeyInputFocusManager::SetFocus(UI::View view)
{
  if(!view)
  {
    // No-op
    return;
  }

  if(view == mCurrentFocusView)
  {
    // View already has focus
    return;
  }

  // view.OffSceneSignal().Connect(mSlotDelegate, &KeyInputFocusManager::OnFocusViewSceneDisconnection);
  // CustomActor (View) inherits Actor, so OffSceneSignal is available.
  view.OffSceneSignal().Connect(mSlotDelegate, &KeyInputFocusManager::OnFocusViewSceneDisconnection);

  UI::View previousFocusView = GetCurrentFocusView();

  // Set view to currentFocusView
  mCurrentFocusView = view;
  mCurrentWindowId  = static_cast<uint32_t>(Dali::Integration::SceneHolder::Get(view).GetNativeId());

  if(previousFocusView)
  {
    // Notify the view that it has lost key input focus
    Integration::GetImpl(previousFocusView).OnKeyInputFocusLost();
  }

  // Tell the new view that it has gained focus.
  Integration::GetImpl(view).OnKeyInputFocusGained();

  // Emit the signal to inform focus change to the application.
  if(!mKeyInputFocusChangedSignal.Empty())
  {
    mKeyInputFocusChangedSignal.Emit(view, previousFocusView);
  }
}

void KeyInputFocusManager::RemoveFocus(UI::View view)
{
  if(view && view == mCurrentFocusView)
  {
    view.OffSceneSignal().Disconnect(mSlotDelegate, &KeyInputFocusManager::OnFocusViewSceneDisconnection);

    mCurrentFocusView.Reset();
    mCurrentWindowId = 0;

    // Notify the view that it has lost key input focus
    Integration::GetImpl(view).OnKeyInputFocusLost();
  }
}

UI::View KeyInputFocusManager::GetCurrentFocusView() const
{
  return mCurrentFocusView;
}

uint32_t KeyInputFocusManager::GetCurrentWindowId() const
{
  return mCurrentWindowId;
}

UI::KeyInputFocusManager::KeyInputFocusChangedSignalType& KeyInputFocusManager::KeyInputFocusChangedSignal()
{
  return mKeyInputFocusChangedSignal;
}

bool KeyInputFocusManager::OnKeyEvent(const KeyEvent& event)
{
  bool consumed = false;

  UI::View view = GetCurrentFocusView();
  if(view)
  {
    // Key events that occur in windows other than the currently focused view are skipped.
    uint32_t eventWindowId = event.GetWindowId();
    if(eventWindowId > 0 && GetCurrentWindowId() != eventWindowId)
    {
      return consumed;
    }

    Dali::Actor dispatch = view;
    while(dispatch)
    {
      // Check dispatch property logic if needed.
      // For now, simpler than Toolkit.
      dispatch = dispatch.GetParent();
    }

    // Notify the view about the key event
    consumed = EmitKeyEventSignal(view, event);
  }

  return consumed;
}

bool KeyInputFocusManager::EmitKeyEventSignal(UI::View view, const KeyEvent& event)
{
  bool consumed = false;

  if(view)
  {
    consumed = Integration::GetImpl(view).EmitKeyEventSignal(event);

    // if view doesn't consume KeyEvent, give KeyEvent to its parent.
    if(!consumed)
    {
      UI::View parent = UI::View::DownCast(view.GetParent());

      if(parent)
      {
        consumed = EmitKeyEventSignal(parent, event);
      }
    }
  }

  return consumed;
}

void KeyInputFocusManager::OnFocusViewSceneDisconnection(Dali::Actor actor)
{
  RemoveFocus(Dali::UI::View::DownCast(actor));
}

bool KeyInputFocusManager::DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor)
{
  bool                  connected(true);
  KeyInputFocusManager* manager = dynamic_cast<KeyInputFocusManager*>(object);

  if(manager)
  {
    if(0 == strcmp(signalName.c_str(), SIGNAL_KEY_INPUT_FOCUS_CHANGED))
    {
      manager->KeyInputFocusChangedSignal().Connect(tracker, functor);
    }
    else
    {
      // signalName does not match any signal
      connected = false;
    }
  }

  return connected;
}

} // namespace Internal

} // namespace UI

} // namespace Dali
