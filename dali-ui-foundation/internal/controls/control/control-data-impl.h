#ifndef DALI_UI_CONTROL_DATA_IMPL_H
#define DALI_UI_CONTROL_DATA_IMPL_H

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
#include <dali/devel-api/adaptor-framework/accessibility.h>
#include <dali/integration-api/processor-interface.h>
#include <dali/public-api/object/property-notification.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/integration-api/debug.h>
#include <string>
#include <map>
#include <memory>
#include <set>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/controls/control-devel.h>
#include <dali-ui-foundation/public-api/controls/control-impl.h>
#include <dali-ui-foundation/devel-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/internal/visuals/visual-event-observer.h>

namespace Dali
{
namespace UI
{
namespace Internal
{
enum class TriStateProperty
{
  AUTO = 0,
  TRUE,
  FALSE
};

/**
 * @brief Holds the Implementation for the internal control class
 */
class Control::Impl : public ConnectionTracker, public Dali::Integration::Processor, public Visual::EventObserver
{
private:
  class AccessibilityData;

  /**
   * @brief Registered visual info.
   */
  struct RegisteredVisual
  {
    Property::Index index;
    UI::Visual::Base visual;
    bool enabled;
    int depthIndex;

    RegisteredVisual(Property::Index idx, UI::Visual::Base vis, bool en, int depth)
      : index(idx),
        visual(vis),
        enabled(en),
        depthIndex(depth)
    {
    }
  };

public:
  /**
   * @brief Retrieves the implementation of the internal control class.
   * @param[in] internalControl A ref to the control whose internal implementation is required
   * @return The internal implementation
   */
  static Control::Impl& Get(Internal::Control& internalControl);

  /**
   * @copydoc Get( Internal::Control& )
   */
  static const Control::Impl& Get(const Internal::Control& internalControl);

  /**
   * @brief Constructor.
   * @param[in] controlImpl The control which owns this implementation
   */
  Impl(Control& controlImpl);

  /**
   * @brief Destructor.
   */
  ~Impl();

  /**
   * @brief Called when a pinch is detected.
   * @param[in] actor The actor the pinch occurred on
   * @param[in] pinch The pinch gesture details
   */
  void PinchDetected(Actor actor, const PinchGesture& pinch);

  /**
   * @brief Called when a pan is detected.
   * @param[in] actor The actor the pan occurred on
   * @param[in] pan The pan gesture details
   */
  void PanDetected(Actor actor, const PanGesture& pan);

  /**
   * @brief Called when a tap is detected.
   * @param[in] actor The actor the tap occurred on
   * @param[in] tap The tap gesture details
   */
  void TapDetected(Actor actor, const TapGesture& tap);

  /**
   * @brief Called when a long-press is detected.
   * @param[in] actor The actor the long-press occurred on
   * @param[in] longPress The long-press gesture details
   */
  void LongPressDetected(Actor actor, const LongPressGesture& longPress);

  /**
   * @brief Function used to set control properties.
   * @param[in] object The object whose property to set
   * @param[in] index The index of the property to set
   * @param[in] value The value of the property to set
   */
  static void SetProperty(BaseObject* object, Property::Index index, const Property::Value& value);

  /**
   * @brief Function used to retrieve the value of control properties.
   * @param[in] object The object whose property to get
   * @param[in] index The index of the property to get
   * @return The value of the property
   */
  static Property::Value GetProperty(BaseObject* object, Property::Index index);

  /**
   * @brief Sets the state of the control.
   * @param[in] newState The state to set
   * @param[in] withTransitions Whether to show a transition when changing to the new state
   */
  void SetState(DevelControl::State newState, bool withTransitions = true);

  /**
   * @brief Sets the sub-state of the control.
   * @param[in] newState The sub-state to set
   * @param[in] withTransitions Whether to show a transition when changing to the new sub-state
   */
  void SetSubState(const std::string& subStateName, bool withTransitions = true);

  /**
   * @copydoc CustomActorImpl::OnSceneConnection()
   */
  void OnSceneConnection();

  /**
   * @copydoc CustomActorImpl::OnSceneDisconnection()
   */
  void OnSceneDisconnection();

  /**
   * @brief Sets the margin.
   * @param[in] margin Margin is a collections of extent ( start, end, top, bottom )
   */
  void SetMargin(Extents margin);

  /**
   * @brief Returns the value of margin
   * @return The value of margin
   */
  Extents GetMargin() const;

  /**
   * @brief Sets the padding.
   * @param[in] padding Padding is a collections of extent ( start, end, top, bottom ).
   */
  void SetPadding(Extents padding);

  /**
   * @brief Returns the value of padding
   * @return The value of padding
   */
  Extents GetPadding() const;

  /**
   * @brief Set the input method context.
   * @param[in] inputMethodContext The input method context.
   */
  void SetInputMethodContext(InputMethodContext& inputMethodContext);

  /**
   * @brief Filter an key event.
   * @param[in] event The key to be filtered.
   * @return True if the key handled, otherwise false.
   */
  bool FilterKeyEvent(const KeyEvent& event);

  /**
   * @brief Get private AccessibilityData context for this impl. If not created yet, it will create new data.
   * @return The l-value of AccessibilityData context.
   */
  [[nodiscard]] AccessibilityData& GetOrCreateAccessibilityData();

  /**
   * @brief Get private AccessibilityData context for this impl.
   * @return The pointer of AccessibilityData context.
   */
  [[nodiscard]] AccessibilityData* GetAccessibilityData() const;

  /**
   * @brief Adds accessibility attribute
   * @param[in] key Attribute name to set
   * @param[in] value Attribute value to set
   */
  void AppendAccessibilityAttribute(const std::string& key, const std::string value);

  /**
   * @brief Removes accessibility attribute
   * @param[in] key Attribute name to remove
   */
  void RemoveAccessibilityAttribute(const std::string& key);

  /**
   * @brief Removes all accessibility attributes
   */
  void ClearAccessibilityAttributes();

  /**
   * @brief Sets reading info type attributes
   * @param[in] types info type attributes to set
   */
  void SetAccessibilityReadingInfoType(const Dali::Accessibility::ReadingInfoTypes types);

  /**
   * @brief Gets currently active reading info type attributes
   */
  Dali::Accessibility::ReadingInfoTypes GetAccessibilityReadingInfoType() const;

  /**
   * @copydoc Dali::UI::Internal::Control::GetAccessibleObject()
   */
  std::shared_ptr<UI::DevelControl::ControlAccessible> GetAccessibleObject();

  /**
   * @copydoc Dali::UI::DevelControl::IsAccessibleCreated()
   */
  bool IsAccessibleCreated() const;

  /**
   * @copydoc Dali::UI::DevelControl::EnableCreateAccessible()
   */
  void EnableCreateAccessible(bool enable);

  /**
   * @copydoc Dali::UI::DevelControl::IsCreateAccessibleEnabled()
   */
  bool IsCreateAccessibleEnabled() const;

  // Visual management
  void RegisterVisual(Property::Index index, UI::Visual::Base& visual, bool enabled = true);
  void UnregisterVisual(Property::Index index);
  UI::Visual::Base GetVisual(Property::Index index) const;
  bool IsVisualEnabled(Property::Index index) const;
  void EnableVisual(Property::Index index, bool enable);
  void SetBackground(const Property::Map& map);
  void ClearBackground();

  // Visual::EventObserver
  void ResourceReady(Visual::Base& object) override;
  void NotifyVisualEvent(Visual::Base& object, Property::Index signalId) override;
  void RelayoutRequest(Visual::Base& object) override;

protected: // From processor-interface
  /**
   * @copydoc Dali::Integration::Processor::Process()
   */
  void Process(bool postProcessor) override;

  /**
   * @copydoc Dali::Integration::Processor::GetProcessorName()
   */
  std::string_view GetProcessorName() const override
  {
    return "UIControlDataImpl";
  }

private:
  /**
   * @brief Callbacks called on idle.
   *
   * @return True if we need to call this idle callback one more time.
   */
  bool OnIdleCallback();

public:
  Control& mControlImpl;
  DevelControl::State mState;
  std::string mSubStateName;

  std::unique_ptr<AccessibilityData> mAccessibilityData;

  int mLeftFocusableActorId;             ///< Actor ID of Left focusable control.
  int mRightFocusableActorId;            ///< Actor ID of Right focusable control.
  int mUpFocusableActorId;               ///< Actor ID of Up focusable control.
  int mDownFocusableActorId;             ///< Actor ID of Down focusable control.
  int mClockwiseFocusableActorId;        ///< Actor ID of Clockwise focusable control.
  int mCounterClockwiseFocusableActorId; ///< Actor ID of Counter clockwise focusable control.

  std::string mStyleName;
  Vector3* mStartingPinchScale; ///< The scale when a pinch gesture starts
  Extents mMargin;              ///< The margin values
  Extents mPadding;             ///< The padding values
  Vector2 mSize;                ///< The size of the control
  UI::Control::KeyEventSignalType mKeyEventSignal;
  UI::Control::KeyInputFocusSignalType mKeyInputFocusGainedSignal;
  UI::Control::KeyInputFocusSignalType mKeyInputFocusLostSignal;

  // Gesture Detection
  PinchGestureDetector mPinchGestureDetector;
  PanGestureDetector mPanGestureDetector;
  TapGestureDetector mTapGestureDetector;
  LongPressGestureDetector mLongPressGestureDetector;

  InputMethodContext mInputMethodContext;
  CallbackBase* mIdleCallback; ///< The idle callback.

  std::vector<RegisteredVisual> mVisuals; ///< Registered visuals.

  ControlBehaviour mFlags : CONTROL_BEHAVIOUR_FLAG_COUNT; ///< Flags passed in from constructor.

  // Frequently touched accessibility relative values.
  int32_t mAccessibilityRole : Dali::Log<static_cast<uint32_t>(DevelControl::AccessibilityRole::MAX_COUNT)>::value + 2;

  bool mIsKeyboardNavigationSupported : 1; ///< Stores whether keyboard navigation is supported by the control.
  bool mIsKeyboardFocusGroup : 1;          ///< Stores whether the control is a focus group.
  bool mDispatchKeyEvents : 1;             ///< Whether the actor emits key event signals
  bool mAccessibleCreatable : 1;           ///< Whether we can create new accessible or not.
  bool mProcessorRegistered : 1;           ///< Whether the processor is registered.

  // Properties - these need to be members of Internal::Control::Impl
  static const PropertyRegistration PROPERTY_1;
  static const PropertyRegistration PROPERTY_2;
  static const PropertyRegistration PROPERTY_3;
  static const PropertyRegistration PROPERTY_4;
  static const PropertyRegistration PROPERTY_5;
  static const PropertyRegistration PROPERTY_6;
  static const PropertyRegistration PROPERTY_7;
  static const PropertyRegistration PROPERTY_8;
  static const PropertyRegistration PROPERTY_9;
  static const PropertyRegistration PROPERTY_10;
  static const PropertyRegistration PROPERTY_11;
  static const PropertyRegistration PROPERTY_12;
  static const PropertyRegistration PROPERTY_13;
  static const PropertyRegistration PROPERTY_14;
  static const PropertyRegistration PROPERTY_15;
  static const PropertyRegistration PROPERTY_16;
  static const PropertyRegistration PROPERTY_17;
  static const PropertyRegistration PROPERTY_18;
  static const PropertyRegistration PROPERTY_19;
  static const PropertyRegistration PROPERTY_20;
  static const PropertyRegistration PROPERTY_21;
  static const PropertyRegistration PROPERTY_22;
  static const PropertyRegistration PROPERTY_23;
};

} // namespace Internal

} // namespace UI

} // namespace Dali

#endif // DALI_UI_CONTROL_DATA_IMPL_H
