#ifndef DALI_UI_INTERNAL_KEYBOARD_FOCUS_MANAGER_H
#define DALI_UI_INTERNAL_KEYBOARD_FOCUS_MANAGER_H

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
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/weak-handle.h>
#include <string>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/input/keyboard-focus-manager.h>
#include <dali-ui-foundation/public-api/view.h>
#include <dali-ui-foundation/public-api/focus/focus-direction.h>

namespace Dali
{
namespace Integration
{
class SceneHolder;
} // namespace Integration

namespace UI
{
namespace Internal
{

/**
 * @brief Internal implementation of KeyboardFocusManager.
 */
class KeyboardFocusManager : public Dali::BaseObject, public ConnectionTracker
{
public:
  enum FocusIndicatorState
  {
    UNKNOWN = -1,
    HIDE    = 0,
    SHOW    = 1,
  };

  /**
   * @copydoc UI::KeyboardFocusManager::Get
   */
  static UI::KeyboardFocusManager Get();

  /**
   * Construct a new KeyboardFocusManager.
   */
  KeyboardFocusManager();

  /**
   * @copydoc UI::KeyboardFocusManager::SetCurrentFocusActor
   */
  bool SetCurrentFocusActor(Actor actor);

  /**
   * @copydoc UI::KeyboardFocusManager::GetCurrentFocusActor
   */
  Actor GetCurrentFocusActor();

  /**
   * @copydoc UI::KeyboardFocusManager::MoveFocus
   */
  bool MoveFocus(UI::Focus::Direction direction);

  /**
   * @copydoc UI::KeyboardFocusManager::ClearFocus
   */
  void ClearFocus();

  /**
   * @copydoc UI::KeyboardFocusManager::SetAsFocusGroup
   */
  void SetAsFocusGroup(Actor actor, bool isFocusGroup);

  /**
   * @copydoc UI::KeyboardFocusManager::IsFocusGroup
   */
  bool IsFocusGroup(Actor actor) const;

  /**
   * @copydoc UI::KeyboardFocusManager::GetFocusGroup
   */
  Actor GetFocusGroup(Actor actor);

  /**
   * @copydoc UI::KeyboardFocusManager::SetFocusGroupLoop
   */
  void SetFocusGroupLoop(bool enabled);

  /**
   * @copydoc UI::KeyboardFocusManager::GetFocusGroupLoop
   */
  bool GetFocusGroupLoop() const;

  /**
   * @copydoc UI::KeyboardFocusManager::SetFocusIndicatorActor
   */
  void SetFocusIndicatorActor(Actor indicator);

  /**
   * @copydoc UI::KeyboardFocusManager::GetFocusIndicatorActor
   */
  Actor GetFocusIndicatorActor();

  /**
   * Move current focus to backward
   */
  void MoveFocusBackward();

  /**
   * Enable default algorithm.
   */
  void EnableDefaultAlgorithm(bool enable);

  /**
   * Is default algorithm enabled.
   */
  bool IsDefaultAlgorithmEnabled() const;

  /**
   * @brief Sets whether to clear focus when window loses focus
   */
  void SetClearFocusOnWindowFocusLost(bool enabled);

  /**
   * @brief Gets whether to clear focus when window loses focus
   */
  bool GetClearFocusOnWindowFocusLost() const;

public:
  /**
   * @copydoc UI::KeyboardFocusManager::PreFocusChangeSignal()
   */
  UI::KeyboardFocusManager::PreFocusChangeSignalType& PreFocusChangeSignal();

  /**
   * @copydoc UI::KeyboardFocusManager::FocusChangedSignal()
   */
  UI::KeyboardFocusManager::FocusChangedSignalType& FocusChangedSignal();

  /**
   * @copydoc UI::KeyboardFocusManager::FocusGroupChangedSignal()
   */
  UI::KeyboardFocusManager::FocusGroupChangedSignalType& FocusGroupChangedSignal();

  /**
   * @copydoc UI::KeyboardFocusManager::FocusedActorEnterKeySignal()
   */
  UI::KeyboardFocusManager::FocusedActorEnterKeySignalType& FocusedActorEnterKeySignal();

protected:
  /**
   * Destructor
   */
  virtual ~KeyboardFocusManager();

private:
  typedef std::vector<WeakHandle<Actor> > FocusStack;
  typedef FocusStack::iterator            FocusStackIterator;

  /**
   * This will be called when the adaptor is initialized
   */
  void OnAdaptorInit();

  /**
   * This will be called when a new scene holder is created
   */
  void OnSceneHolderCreated(Dali::Integration::SceneHolder& sceneHolder);

  /**
   * Get the focus group of current focused actor.
   */
  Actor GetCurrentFocusGroup();

  /**
   * Move the focus to the specified actor and send notification.
   */
  bool DoSetCurrentFocusActor(Actor actor);

  /**
   * Move the focus to the next actor within the layout control.
   */
  bool DoMoveFocusWithinLayoutControl(UI::View control, Actor actor, UI::Focus::Direction direction);

  /**
   * Move the focus to the next focus group.
   */
  bool DoMoveFocusToNextFocusGroup(bool forward);

  /**
   * Enter has been pressed on the actor.
   */
  void DoKeyboardEnter(Actor actor);

  /**
   * Check whether the actor is a layout control.
   */
  bool IsLayoutControl(Actor actor) const;

  /**
   * Returns the parent layout control.
   */
  UI::View GetParentLayoutControl(Actor actor) const;

  /**
   * Callback for the key event.
   */
  void OnKeyEvent(const KeyEvent& event);

  /**
   * Callback for the touch event.
   */
  void OnTouch(const TouchEvent& touch);

  /**
   * Called when the window focus is changed.
   */
  void OnWindowFocusChanged(Window window, bool focusIn);

  /**
   * Get the focus Actor from current window
   */
  Actor GetFocusActorFromCurrentWindow();

  /**
   * Clear the focus actor
   */
  void ClearFocus(Actor actor);

  /**
   * Clear the focus indicator actor.
   */
  void ClearFocusIndicator(Actor actor);

  /**
   * Gets the current native window id
   */
  uint32_t GetCurrentWindowId() const;

  /**
   * Signal handler called when a focused actor is removed.
   */
  void OnSceneDisconnection(Dali::Actor actor);

private:
  // Undefined
  KeyboardFocusManager(const KeyboardFocusManager&);
  KeyboardFocusManager& operator=(const KeyboardFocusManager& rhs);

private:
  UI::KeyboardFocusManager::PreFocusChangeSignalType       mPreFocusChangeSignal;
  UI::KeyboardFocusManager::FocusChangedSignalType         mFocusChangedSignal;
  UI::KeyboardFocusManager::FocusGroupChangedSignalType    mFocusGroupChangedSignal;
  UI::KeyboardFocusManager::FocusedActorEnterKeySignalType mFocusedActorEnterKeySignal;

  WeakHandle<Actor> mCurrentFocusActor;

  Actor mFocusIndicatorActor;

  WeakHandle<Actor> mFocusFinderRootActor;

  FocusStack mFocusHistory;

  SlotDelegate<KeyboardFocusManager> mSlotDelegate;

  typedef std::vector<std::pair<WeakHandle<Layer>, WeakHandle<Actor> > > FocusActorContainer;

  FocusActorContainer mCurrentFocusActors;

  WeakHandle<Layer> mCurrentFocusedWindow;

  FocusIndicatorState mIsFocusIndicatorShown;

  bool mFocusGroupLoopEnabled : 1;
  bool mIsWaitingKeyboardFocusChangeCommit : 1;
  bool mClearFocusOnTouch : 1;
  bool mEnableDefaultAlgorithm : 1;
  bool mClearFocusOnWindowFocusLost : 1;

  uint32_t mCurrentWindowId;

  static const unsigned int MAX_HISTORY_AMOUNT = 30;
};

} // namespace Internal

inline Internal::KeyboardFocusManager& GetImpl(UI::KeyboardFocusManager& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  Dali::BaseObject& handle = obj.GetBaseObject();
  return static_cast<Internal::KeyboardFocusManager&>(handle);
}

inline const Internal::KeyboardFocusManager& GetImpl(const UI::KeyboardFocusManager& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  const Dali::BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Internal::KeyboardFocusManager&>(handle);
}

} // namespace UI

} // namespace Dali

#endif // DALI_UI_INTERNAL_KEYBOARD_FOCUS_MANAGER_H
