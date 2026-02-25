#ifndef DALI_UI_CONTROL_IMPL_H
#define DALI_UI_CONTROL_IMPL_H

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
#include <dali/public-api/adaptor-framework/style-change.h>
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/events/long-press-gesture.h>
#include <dali/public-api/events/pan-gesture.h>
#include <dali/public-api/events/pinch-gesture.h>
#include <dali/public-api/events/tap-gesture.h>
#include <dali/public-api/object/property-index-ranges.h>
#include <dali/public-api/object/type-info.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/controls/control.h>

namespace Dali
{
namespace UI
{
/**
 * @addtogroup dali_ui_controls
 * @{
 */

namespace DevelControl
{
class ControlAccessible;

} // namespace DevelControl

namespace Internal
{
/**
 * @brief This is the internal base class for all controls.
 *
 * It will provide some common functionality required by all controls.
 * Implements ConnectionTrackerInterface so that signals (typically connected to member functions) will
 * be disconnected automatically when the control is destroyed.
 */
class DALI_UI_API Control : public CustomActorImpl, public ConnectionTrackerInterface
{
public:
  class Extension; ///< Forward declare future extension interface

  // Creation & Destruction
  /**
   * @brief Creates a new ControlImpl instance that does not require touch by default.
   *
   * If touch is required, then the user can connect to this class' touch signal.
   * @return A handle to the ControlImpl instance
   */
  static UI::Control New();

protected:
  /**
   * @brief Virtual destructor.
   */
  virtual ~Control();

public:
  // Styling

  /**
   * @copydoc Dali::UI::Control::SetStyleName
   */
  void SetStyleName(const std::string& styleName);

  /**
   * @copydoc Dali::UI::Control::GetStyleName
   */
  const std::string& GetStyleName() const;

  // Accessibility

  /**
   * @brief Gets the Accessible object that represents this control.
   *
   * This method calls CreateAccessibleObject() if CreateAccessible is true.
   *
   * @return The Accessible object
   *
   * @see CreateAccessibleObject()
   */
  std::shared_ptr<UI::DevelControl::ControlAccessible> GetAccessibleObject();

  // Gesture Detection

  /**
   * @brief Allows deriving classes to enable any of the gesture detectors that are available.
   *
   * Gesture detection can be enabled one at a time or in bitwise format as shown:
   * @code
   * EnableGestureDetection(GestureType::Value(GestureType::PINCH | GestureType::TAP | GestureType::PAN));
   * @endcode
   * @param[in] type The gesture type(s) to enable
   */
  void EnableGestureDetection(GestureType::Value type);

  /**
   * @brief Allows deriving classes to disable any of the gesture detectors.
   *
   * Like EnableGestureDetection, this can also be called using bitwise or.
   * @param[in] type The gesture type(s) to disable
   * @see EnableGetureDetection
   */
  void DisableGestureDetection(GestureType::Value type);

  /**
   * @brief If deriving classes wish to fine tune pinch gesture
   * detection, then they can access the gesture detector through this
   * API and modify the detection.
   *
   * @return The pinch gesture detector
   * @pre Pinch detection should have been enabled via EnableGestureDetection().
   * @see EnableGestureDetection
   */
  PinchGestureDetector GetPinchGestureDetector() const;

  /**
   * @brief If deriving classes wish to fine tune pan gesture
   * detection, then they can access the gesture detector through this
   * API and modify the detection.
   *
   * @return The pan gesture detector
   * @pre Pan detection should have been enabled via EnableGestureDetection().
   * @see EnableGestureDetection
   */
  PanGestureDetector GetPanGestureDetector() const;

  /**
   * @brief If deriving classes wish to fine tune tap gesture
   * detection, then they can access the gesture detector through this
   * API and modify the detection.
   *
   * @return The tap gesture detector
   * @pre Tap detection should have been enabled via EnableGestureDetection().
   * @see EnableGestureDetection
   */
  TapGestureDetector GetTapGestureDetector() const;

  /**
   * @brief If deriving classes wish to fine tune long press gesture
   * detection, then they can access the gesture detector through this
   * API and modify the detection.
   *
   * @return The long press gesture detector
   * @pre Long press detection should have been enabled via EnableGestureDetection().
   * @see EnableGestureDetection
   */
  LongPressGestureDetector GetLongPressGestureDetector() const;

  // Keyboard Navigation

  /**
   * @brief Sets whether this control supports two dimensional
   * keyboard navigation (i.e. whether it knows how to handle the
   * keyboard focus movement between its child actors).
   *
   * The control doesn't support it by default.
   * @param[in] isSupported Whether this control supports two dimensional keyboard navigation
   */
  void SetKeyboardNavigationSupport(bool isSupported);

  /**
   * @brief Gets whether this control supports two dimensional keyboard navigation.
   *
   * @return true if this control supports two dimensional keyboard navigation
   */
  bool IsKeyboardNavigationSupported();

  // Key Input

  /**
   * @copydoc UI::Control::SetKeyInputFocus()
   */
  void SetKeyInputFocus();

  /**
   * @copydoc UI::Control::HasKeyInputFocus()
   */
  bool HasKeyInputFocus();

  /**
   * @copydoc UI::Control::ClearKeyInputFocus()
   */
  void ClearKeyInputFocus();

  // Keyboard Focus

  /**
   * @brief Sets whether this control is a focus group for keyboard navigation.
   *
   * (i.e. the scope of keyboard focus movement
   * can be limited to its child actors). The control is not a focus group by default.
   * @param[in] isFocusGroup Whether this control is set as a focus group for keyboard navigation
   */
  void SetAsKeyboardFocusGroup(bool isFocusGroup);

  /**
   * @brief Gets whether this control is a focus group for keyboard navigation.
   *
   * @return true if this control is set as a focus group for keyboard navigation
   */
  bool IsKeyboardFocusGroup();

  /// @cond internal
  /**
   * @brief Called by the KeyboardFocusManager.
   */
  DALI_INTERNAL void KeyboardEnter();
  /// @endcond

  // Signals

  /**
   * @copydoc Dali::UI::Control::KeyEventSignal()
   */
  UI::Control::KeyEventSignalType& KeyEventSignal();

  /**
   * @copydoc Dali::UI::Control::KeyInputFocusGainedSignal()
   */
  UI::Control::KeyInputFocusSignalType& KeyInputFocusGainedSignal();

  /**
   * @copydoc Dali::UI::Control::KeyInputFocusLostSignal()
   */
  UI::Control::KeyInputFocusSignalType& KeyInputFocusLostSignal();

  /// @cond internal
  /**
   * @brief Called by the KeyInputFocusManager to emit key event signals.
   *
   * @param[in] event The key event
   * @return True if the event was consumed
   */
  DALI_INTERNAL bool EmitKeyEventSignal(const KeyEvent& event);
  /// @endcond

protected: // For derived classes to call
  /**
   * @brief Emits KeyInputFocusGained signal if true else emits KeyInputFocusLost signal.
   *
   * Should be called last by the control after it acts on the Input Focus change.
   *
   * @param[in] focusGained True if gained, False if lost
   */
  void EmitKeyInputFocusSignal(bool focusGained);

protected: // From CustomActorImpl
  /**
   * @copydoc CustomActorImpl::OnSceneConnection()
   * @note If overridden, then an up-call to Control::OnSceneConnection MUST be made at the end.
   */
  void OnSceneConnection(int depth) override;

  /**
   * @copydoc CustomActorImpl::OnSceneDisconnection()
   * @note If overridden, then an up-call to Control::OnSceneDisconnection MUST be made at the end.
   */
  void OnSceneDisconnection() override;

  /**
   * @copydoc CustomActorImpl::OnChildAdd()
   * @note If overridden, then an up-call to Control::OnChildAdd MUST be made at the end.
   */
  void OnChildAdd(Actor& child) override;

  /**
   * @copydoc CustomActorImpl::OnChildRemove()
   * @note If overridden, then an up-call to Control::OnChildRemove MUST be made at the end.
   */
  void OnChildRemove(Actor& child) override;

  /**
   * @copydoc CustomActorImpl::OnPropertySet()
   * @note If overridden, then an up-call to Control::OnPropertySet MUST be made at the end.
   */
  void OnPropertySet(Property::Index index, const Property::Value& propertyValue) override;

  /**
   * @copydoc CustomActorImpl::OnSizeSet()
   * @note If overridden, then an up-call to Control::OnSizeSet MUST be made at the end.
   */
  void OnSizeSet(const Vector3& targetSize) override;

  /**
   * @copydoc CustomActorImpl::OnSizeAnimation()
   * @note If overridden, then an up-call to Control::OnSizeAnimation MUST be made at the end.
   */
  void OnSizeAnimation(Animation& animation, const Vector3& targetSize) override;

  /**
   * @copydoc CustomActorImpl::OnRelayout()
   */
  void OnRelayout(const Vector2& size, RelayoutContainer& container) override;

  /**
   * @copydoc CustomActorImpl::OnSetResizePolicy()
   */
  void OnSetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension) override;

  /**
   * @copydoc CustomActorImpl::GetNaturalSize()
   */
  Vector3 GetNaturalSize() override;

  /**
   * @copydoc CustomActorImpl::CalculateChildSize()
   */
  float CalculateChildSize(const Dali::Actor& child, Dimension::Type dimension) override;

  /**
   * @copydoc CustomActorImpl::GetHeightForWidth()
   */
  float GetHeightForWidth(float width) override;

  /**
   * @copydoc CustomActorImpl::GetWidthForHeight()
   */
  float GetWidthForHeight(float height) override;

  /**
   * @copydoc CustomActorImpl::RelayoutDependentOnChildren()
   */
  bool RelayoutDependentOnChildren(Dimension::Type dimension = Dimension::ALL_DIMENSIONS) override;

  /**
   * @copydoc CustomActorImpl::OnCalculateRelayoutSize()
   */
  void OnCalculateRelayoutSize(Dimension::Type dimension) override;

  /**
   * @copydoc CustomActorImpl::OnLayoutNegotiated()
   */
  void OnLayoutNegotiated(float size, Dimension::Type dimension) override;

  /**
   * @copydoc CustomActorImpl::GetOffScreenRenderTasks()
   */
  void GetOffScreenRenderTasks(Dali::Vector<Dali::RenderTask>& tasks, bool isForward) override;

public: // Helpers for deriving classes
  /**
   * @brief Flags for the constructor.
   */
  enum ControlBehaviour
  {
    CONTROL_BEHAVIOUR_DEFAULT = 0, ///< Default behaviour: Size negotiation is enabled & listens to Style Change signal,
                                   ///< but doesn't receive event callbacks.
    NOT_IN_USE_1 = 1 << (CustomActorImpl::ACTOR_FLAG_COUNT + 0),
    REQUIRES_KEYBOARD_NAVIGATION_SUPPORT =
        1 << (CustomActorImpl::ACTOR_FLAG_COUNT + 1), ///< True if needs to support keyboard navigation
    DISABLE_STYLE_CHANGE_SIGNALS =
        1 << (CustomActorImpl::ACTOR_FLAG_COUNT + 2), ///< True if control should not monitor style change signals

    LAST_CONTROL_BEHAVIOUR_FLAG
  };

  static const int CONTROL_BEHAVIOUR_FLAG_COUNT =
      Log<LAST_CONTROL_BEHAVIOUR_FLAG - 1>::value + 1; ///< Total count of flags

  /**
   * @brief Creates a new ControlImpl instance that does not require touch by default.
   *
   * If touch is required, then the user can connect to this class' touch signal.
   * @param[in] additionalBehaviour Additional control behaviour.
   * @return A handle to the ControlImpl instance
   */
  static UI::Control New(ControlBehaviour additionalBehaviour);

protected:
  // Construction

  /**
   * @brief Control constructor.
   *
   * @param[in] behaviourFlags Behavioural flags from ControlBehaviour enum
   */
  Control(ControlBehaviour behaviourFlags);

  /**
   * @brief Second phase initialization.
   */
  void Initialize();

public: // API for derived classes to override
  // Lifecycle

  /**
   * @brief This method is called after the Control has been initialized.
   *
   * Derived classes should do any second phase initialization by overriding this method.
   */
  virtual void OnInitialize();

  // Accessibility

  /**
   * @brief This method is called when the control is accessibility activated.
   *
   * Derived classes should override this to perform custom accessibility activation.
   * @return true if this control can perform accessibility activation
   */
  virtual bool OnAccessibilityActivated();

  /**
   * @brief This method should be overridden by deriving classes when they wish to respond the accessibility
   * pan gesture.
   *
   * @param[in] gesture The pan gesture
   * @return true if the pan gesture has been consumed by this control
   */
  virtual bool OnAccessibilityPan(PanGesture gesture);

  /**
   * @brief This method should be overridden by deriving classes when they wish to respond
   * the accessibility up and down action (i.e. value change of slider control).
   *
   * @param[in] isIncrease Whether the value should be increased or decreased
   * @return true if the value changed action has been consumed by this control
   */
  virtual bool OnAccessibilityValueChange(bool isIncrease);

  /**
   * @brief This method should be overridden by deriving classes when they wish to respond
   * the accessibility zoom action.
   *
   * @return true if the zoom action has been consumed by this control
   */
  virtual bool OnAccessibilityZoom();

  /**
   * @brief This method should be overriden by deriving classes when they wish to be
   * represented by a custom Accessible object implementation.
   *
   * The AT-SPI infrastructure is responsible for destroying the returned object.
   *
   * @return The newly created Accessible object
   *
   * @see GetAccessibleObject()
   */
  virtual DevelControl::ControlAccessible* CreateAccessibleObject();

  // Keyboard focus

  /**
   * @brief Called when the control gains key input focus.
   *
   * Should be overridden by derived classes if they need to customize what happens when focus is gained.
   */
  virtual void OnKeyInputFocusGained();

  /**
   * @brief Called when the control loses key input focus.
   *
   * Should be overridden by derived classes if they need to customize what happens when focus is lost.
   */
  virtual void OnKeyInputFocusLost();

  /**
   * @brief Gets the next keyboard focusable actor in this control towards the given direction.
   *
   * A control needs to override this function in order to support two dimensional keyboard navigation.
   * @param[in] currentFocusedActor The current focused actor
   * @param[in] direction The direction to move the focus towards
   * @param[in] loopEnabled Whether the focus movement should be looped within the control
   * @return The next keyboard focusable actor in this control or an empty handle if no actor can be focused
   */
  virtual Actor GetNextKeyboardFocusableActor(Actor currentFocusedActor,
                                              UI::Control::KeyboardFocus::Direction direction, bool loopEnabled);

  /**
   * @brief Informs this control that its chosen focusable actor will be focused.
   *
   * This allows the application to perform any actions if wishes
   * before the focus is actually moved to the chosen actor.
   *
   * @param[in] commitedFocusableActor The commited focusable actor
   */
  virtual void OnKeyboardFocusChangeCommitted(Actor commitedFocusableActor);

  /**
   * @brief This method is called when the control has enter pressed on it.
   *
   * Derived classes should override this to perform custom actions.
   * @return true if this control supported this action
   */
  virtual bool OnKeyboardEnter();

  /**
   * @brief Called after a key-event is received by the actor that has had its focus set.
   *
   * @param[in] event The Key Event
   * @return True if the event should be consumed
   */
  virtual bool OnKeyEvent(const KeyEvent& event);

  // Gestures

  /**
   * @brief Called whenever a pinch gesture is detected on this control.
   *
   * This can be overridden by deriving classes when pinch detection
   * is enabled.  The default behaviour is to scale the control by the
   * pinch scale.
   *
   * @param[in] pinch The pinch gesture
   * @note If overridden, then the default behavior will not occur.
   * @note Pinch detection should be enabled via EnableGestureDetection().
   * @see EnableGestureDetection
   */
  virtual void OnPinch(const PinchGesture& pinch);

  /**
   * @brief Called whenever a pan gesture is detected on this control.
   *
   * This should be overridden by deriving classes when pan detection
   * is enabled.
   *
   * @param[in] pan The pan gesture
   * @note There is no default behavior with panning.
   * @note Pan detection should be enabled via EnableGestureDetection().
   * @see EnableGestureDetection
   */
  virtual void OnPan(const PanGesture& pan);

  /**
   * @brief Called whenever a tap gesture is detected on this control.
   *
   * This should be overridden by deriving classes when tap detection
   * is enabled.
   *
   * @param[in] tap The tap gesture
   * @note There is no default behavior with a tap.
   * @note Tap detection should be enabled via EnableGestureDetection().
   * @see EnableGestureDetection
   */
  virtual void OnTap(const TapGesture& tap);

  /**
   * @brief Called whenever a long press gesture is detected on this control.
   *
   * This should be overridden by deriving classes when long press
   * detection is enabled.
   *
   * @param[in] longPress The long press gesture
   * @note There is no default behaviour associated with a long press.
   * @note Long press detection should be enabled via EnableGestureDetection().
   * @see EnableGestureDetection
   */
  virtual void OnLongPress(const LongPressGesture& longPress);

  // From ConnectionTrackerInterface

  /**
   * @copydoc ConnectionTrackerInterface::SignalConnected
   */
  void SignalConnected(SlotObserver* slotObserver, CallbackBase* callback) override;

  /**
   * @copydoc ConnectionTrackerInterface::SignalDisconnected
   */
  void SignalDisconnected(SlotObserver* slotObserver, CallbackBase* callback) override;

  /**
   * @brief Retrieves the extension for this control.
   *
   * @return The extension if available, NULL otherwise
   */
  virtual Extension* GetControlExtension()
  {
    return NULL;
  }

private:
  /// @cond internal

  // Not copyable or movable
  DALI_INTERNAL Control(const Control&) = delete;            ///< Deleted copy constructor.
  DALI_INTERNAL Control(Control&&) = delete;                 ///< Deleted move constructor.
  DALI_INTERNAL Control& operator=(const Control&) = delete; ///< Deleted copy assignment operator.
  DALI_INTERNAL Control& operator=(Control&&) = delete;      ///< Deleted move assignment operator.

public:
  class DALI_INTERNAL Impl; // Class declaration is public so we can internally add devel API's to the Controls Impl

private:
  Impl* mImpl;
  /// @endcond
};

/**
 * @brief Gets implementation from the handle.
 *
 * @param handle
 * @return Implementation
 * @pre handle is initialized and points to a control
 */
DALI_UI_API Internal::Control& GetImplementation(Dali::UI::Control& handle);

/**
 * @brief Gets implementation from the handle.
 *
 * @param handle
 * @return Implementation
 * @pre Handle is initialized and points to a control.
 */
DALI_UI_API const Internal::Control& GetImplementation(const Dali::UI::Control& handle);

} // namespace Internal

/**
 * @}
 */
} // namespace UI

} // namespace Dali

#endif // DALI_UI_CONTROL_IMPL_H
