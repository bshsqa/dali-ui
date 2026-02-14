#ifndef DALI_UI_KEYBOARD_FOCUS_MANAGER_H
#define DALI_UI_KEYBOARD_FOCUS_MANAGER_H

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
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/actors/actor.h>
#include <string>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/focus/focus-direction.h>

namespace Dali
{
namespace UI
{
namespace Internal DALI_INTERNAL
{
class KeyboardFocusManager;
}

/**
 * @brief Provides the functionality of handling keyboard navigation
 * and maintaining the two dimensional keyboard focus chain.
 *
 * It provides functionality of setting the focus and moving the focus
 * in four directions (i.e. Left, Right, Up and Down). It also draws a
 * highlight for the focused actor and emits a signal when the focus
 * is changed.
 */
class DALI_UI_API KeyboardFocusManager : public BaseHandle
{
public:
  /// @brief Pre focus change signal
  typedef Signal<Actor(Actor, Actor, Focus::Direction)> PreFocusChangeSignalType;

  /// @brief Focus changed signal
  typedef Signal<void(Actor, Actor)> FocusChangedSignalType;

  /// @brief Focus group changed signal
  typedef Signal<void(Actor, bool)> FocusGroupChangedSignalType;

  /// @brief Focused actor has the enter key pressed signal
  typedef Signal<void(Actor)> FocusedActorEnterKeySignalType;

  /**
   * @brief Creates a KeyboardFocusManager handle.
   */
  KeyboardFocusManager();

  /**
   * @brief Destructor.
   */
  ~KeyboardFocusManager();

  /**
   * @brief Gets the singleton of KeyboardFocusManager object.
   * @return A handle to the KeyboardFocusManager
   */
  static KeyboardFocusManager Get();

  /**
   * @brief Moves the keyboard focus to the given actor.
   * @param actor The actor to be focused
   * @return Whether the focus is successful or not
   */
  bool SetCurrentFocusActor(Actor actor);

  /**
   * @brief Gets the current focused actor.
   * @return A handle to the current focused actor or an empty handle
   */
  Actor GetCurrentFocusActor();

  /**
   * @brief Moves the focus to the next focusable actor in the given direction.
   * @param direction The direction of focus movement
   * @return true if the movement was successful
   */
  bool MoveFocus(Focus::Direction direction);

  /**
   * @brief Clears the focus from the current focused actor.
   */
  void ClearFocus();

  /**
   * @brief Sets whether the focus movement should be looped.
   * @param enabled Whether the focus movement should be looped
   */
  void SetFocusGroupLoop(bool enabled);

  /**
   * @brief Gets whether the focus movement should be looped.
   * @return Whether the focus movement should be looped
   */
  bool GetFocusGroupLoop() const;

  /**
   * @brief Sets whether an actor is a focus group.
   * @param actor The actor to be set as a focus group
   * @param isFocusGroup Whether to set the actor as a focus group
   */
  void SetAsFocusGroup(Actor actor, bool isFocusGroup);

  /**
   * @brief Checks whether the actor is set as a focus group.
   * @param actor The actor to be checked
   * @return Whether the actor is set as a focus group
   */
  bool IsFocusGroup(Actor actor) const;

  /**
   * @brief Returns the closest ancestor of the given actor that is a focus group.
   * @param actor The actor to be checked
   * @return The focus group or an empty handle
   */
  Actor GetFocusGroup(Actor actor);

  /**
   * @brief Sets the focus indicator actor.
   * @param indicator The indicator actor
   */
  void SetFocusIndicatorActor(Actor indicator);

  /**
   * @brief Gets the focus indicator actor.
   * @return A handle to the focus indicator actor
   */
  Actor GetFocusIndicatorActor();

  /**
   * @brief Move the focus to prev focused actor.
   */
  void MoveFocusBackward();

  /**
   * @brief Sets whether to clear focus when window loses focus.
   * @param enabled Whether to clear focus when window loses focus
   */
  void SetClearFocusOnWindowFocusLost(bool enabled);

  /**
   * @brief Gets whether to clear focus when window loses focus.
   * @return Whether clear focus is enabled
   */
  bool GetClearFocusOnWindowFocusLost() const;

  /**
   * @brief Enables default focus algorithm.
   * @param enable Whether to enable
   */
  void EnableDefaultAlgorithm(bool enable);

  /**
   * @brief Gets whether default algorithm is enabled.
   * @return Whether default algorithm is enabled
   */
  bool IsDefaultAlgorithmEnabled() const;

public: // Signals

  /**
   * @brief This signal is emitted before the focus is going to be changed.
   * @return The signal to connect to
   */
  PreFocusChangeSignalType& PreFocusChangeSignal();

  /**
   * @brief This signal is emitted after the current focused actor has been changed.
   * @return The signal to connect to
   */
  FocusChangedSignalType& FocusChangedSignal();

  /**
   * @brief This signal is emitted when the focus group has been changed.
   * @return The signal to connect to
   */
  FocusGroupChangedSignalType& FocusGroupChangedSignal();

  /**
   * @brief This signal is emitted when the current focused actor has enter key pressed.
   * @return The signal to connect to
   */
  FocusedActorEnterKeySignalType& FocusedActorEnterKeySignal();

  // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL KeyboardFocusManager(Internal::KeyboardFocusManager* impl);
  /// @endcond

}; // class KeyboardFocusManager

} // namespace UI

} // namespace Dali

#endif // DALI_UI_KEYBOARD_FOCUS_MANAGER_H
