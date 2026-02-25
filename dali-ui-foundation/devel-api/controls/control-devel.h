#ifndef DALI_UI_CONTROL_DEVEL_H
#define DALI_UI_CONTROL_DEVEL_H

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
#include <dali/devel-api/adaptor-framework/accessibility-bridge.h>
#include <dali/devel-api/adaptor-framework/input-method-context.h>
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/animation/time-period.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/controls/control-accessible.h>
#include <dali-ui-foundation/public-api/controls/control.h>

namespace Dali
{
namespace UI
{
namespace DevelControl
{
/// @brief AccessibilityActivate signal type.
typedef Signal<void()> AccessibilityActivateSignalType;

/// @brief AccessibilityReadingSkipped signal type.
typedef Signal<void()> AccessibilityReadingSkippedSignalType;

/// @brief AccessibilityReadingPaused signal type.
typedef Signal<void()> AccessibilityReadingPausedSignalType;

/// @brief AccessibilityReadingResumed signal type.
typedef Signal<void()> AccessibilityReadingResumedSignalType;

/// @brief AccessibilityReadingCancelled signal type.
typedef Signal<void()> AccessibilityReadingCancelledSignalType;

/// @brief AccessibilityReadingStopped signal type.
typedef Signal<void()> AccessibilityReadingStoppedSignalType;

/// @brief AccessibilityGetName signal type.
typedef Signal<void(std::string&)> AccessibilityGetNameSignalType;

/// @brief AccessibilityGetDescription signal type.
typedef Signal<void(std::string&)> AccessibilityGetDescriptionSignalType;

/// @brief AccessibilityDoGesture signal type.
typedef Signal<void(std::pair<Dali::Accessibility::GestureInfo, bool>&)> AccessibilityDoGestureSignalType;

/// @brief AccessibilityAction signal type.
typedef Signal<bool(const Dali::Accessibility::ActionInfo&)> AccessibilityActionSignalType;

/// @brief AccessibilityHighlighted signal type.
/// @param bool highlighted true if control is highlighted, false if highlight is removed.
typedef Signal<void(bool)> AccessibilityHighlightedSignalType;

enum State
{
  NORMAL,
  FOCUSED,
  DISABLED
};

namespace Property
{
enum
{
  STYLE_NAME = Control::Property::STYLE_NAME,
  KEY_INPUT_FOCUS = Control::Property::KEY_INPUT_FOCUS,
  MARGIN = Control::Property::MARGIN,
  PADDING = Control::Property::PADDING,

  /**
   * @brief The current state of the control.
   * @details Name "state", type DevelControl::State ( Property::INTEGER ) or Property::STRING
   *
   * @see DevelControl::State
   */
  STATE = PADDING + 2,

  /**
   * @brief The current sub state of the control.
   * @details Name "subState", type Property::INTEGER or Property::STRING. The enumeration used is dependent on the
   * derived control.
   *
   * @see DevelControl::State
   */
  SUB_STATE = PADDING + 3,

  /**
   * @brief The actor ID of the left focusable control.
   * @details Name "leftFocusableActorId", type Property::INTEGER.
   *
   */
  LEFT_FOCUSABLE_ACTOR_ID = PADDING + 4,

  /**
   * @brief The actor ID of the right focusable control.
   * @details Name "rightFocusableActorId", type Property::INTEGER.
   *
   */
  RIGHT_FOCUSABLE_ACTOR_ID = PADDING + 5,

  /**
   * @brief The actor ID of the up focusable control.
   * @details Name "upFocusableActorId", type Property::INTEGER.
   *
   */
  UP_FOCUSABLE_ACTOR_ID = PADDING + 6,

  /**
   * @brief The actor ID of the down focusable control.
   * @details Name "downFocusableActorId", type Property::INTEGER.
   *
   */
  DOWN_FOCUSABLE_ACTOR_ID = PADDING + 7,

  // PADDING + 8 reserved for SHADOW (Visual, excluded)

  /**
   * @brief The name of object visible in accessibility tree.
   * @details Name "accessibilityName", type Property::STRING.
   */
  ACCESSIBILITY_NAME = PADDING + 9,

  /**
   * @brief The description of object visible in accessibility tree.
   * @details Name "accessibilityDescription", type Property::STRING.
   */
  ACCESSIBILITY_DESCRIPTION,

  /**
   * @brief Deprecated. Current translation domain for accessibility clients.
   * @details Name "accessibilityTranslationDomain", type Property::STRING.
   */
  ACCESSIBILITY_TRANSLATION_DOMAIN,

  /**
   * @brief Role being performed in accessibility hierarchy.
   * @details Name "accessibilityRole", type Property::INTEGER.
   * @see Dali::Accessibility::Role
   */
  ACCESSIBILITY_ROLE,

  /**
   * @brief Mark of able to highlight object.
   * @details Name "accessibilityHighlightable", type Property::BOOLEAN.
   */
  ACCESSIBILITY_HIGHLIGHTABLE,

  /**
   * @brief Set of accessibility attributes describing object in accessibility hierarchy
   * @details Name "accessibilityAttributes", type Property::MAP
   */
  ACCESSIBILITY_ATTRIBUTES,

  /**
   * @brief Whether a Control and its descendants can emit key signals.
   * @details Name "dispatchKeyEvents", type Property::BOOLEAN
   * @note If a control's dispatchKeyEvents is set to false, then it's children will not emit a key event signal either.
   */
  DISPATCH_KEY_EVENTS,

  /**
   * @brief Marks the object as invisible to AT-SPI clients.
   * @details Name "accessibilityHidden", type Property::BOOLEAN.
   * @note The representative Accessible object will not appear in the AT-SPI tree.
   */
  ACCESSIBILITY_HIDDEN,

  /**
   * @brief The actor ID of the clockwise focusable control.
   * @details Name "clockwiseFocusableActorId", type Property::INTEGER.
   *
   */
  CLOCKWISE_FOCUSABLE_ACTOR_ID,

  /**
   * @brief The actor ID of the conter-clockwise focusable control.
   * @details Name "counterClockwiseFocusableActorId", type Property::INTEGER.
   *
   */
  COUNTER_CLOCKWISE_FOCUSABLE_ACTOR_ID,

  /**
   * @brief Identifier that allows the automation framework to find and interact with this element.
   * @details Name "automationId", type Property::STRING.
   * @note This is a string identifier (compared to @c Actor::Property::ID which is an integer).
   * It will also appear in the AT-SPI tree under the key "automationId".
   */
  AUTOMATION_ID,

  /**
   * @brief The accessibility value represented by the control. For example, "60%" for a slider object.
   * @details Name "accessibilityValue", type Property::STRING.
   */
  ACCESSIBILITY_VALUE,

  /**
   * @brief Indicates the accessibility services treat the control as scrollable.
   * @details Name "accessibilityScrollable", type Property::BOOLEAN.
   */
  ACCESSIBILITY_SCROLLABLE,

  /**
   * @brief Bitset integer of AccessibilityState which describes the current state of a control.
   * @details Name "accessibilityStates", type Property::INTEGER.
   */
  ACCESSIBILITY_STATES,

  /**
   * @brief Indicates the accessibility services treat the control as modal.
   * @details Name "accessibilityIsModal", type Property::BOOLEAN.
   */
  ACCESSIBILITY_IS_MODAL,
};

} // namespace Property

/**
 * @brief Set input method context.
 *
 * @param[in] control The control.
 * @param[in] inputMethodContext The input method context.
 */
DALI_UI_API void SetInputMethodContext(Internal::Control& control, InputMethodContext& inputMethodContext);

/**
 * @brief The signal is emmited as a succession of "activate" signal send by accessibility client.
 * @return The signal to connect to
 */
DALI_UI_API AccessibilityActivateSignalType& AccessibilityActivateSignal(UI::Control control);

/**
 * @brief The signal is emmited when text send via Dali::Accessibility::Bridge::Say
 * was placed in TTS queue but other text with higher priority prevented it from being read.
 *
 * @return The signal to connect to
 */
DALI_UI_API AccessibilityReadingSkippedSignalType& AccessibilityReadingSkippedSignal(UI::Control control);

/**
 * @brief
 *
 * @return The signal to connect to
 */
DALI_UI_API AccessibilityReadingPausedSignalType& AccessibilityReadingPausedSignal(UI::Control control);

/**
 * @brief
 *
 * @return The signal to connect to
 */
DALI_UI_API AccessibilityReadingResumedSignalType& AccessibilityReadingResumedSignal(UI::Control control);

/**
 * @brief The signal is emmited when text send via Dali::Accessibility::Bridge::Say
 * was placed in TTS queue and reading was started but other text with higher priority cancelled it.
 *
 * @return The signal to connect to
 */
DALI_UI_API AccessibilityReadingCancelledSignalType& AccessibilityReadingCancelledSignal(UI::Control control);

/**
 * @brief The signal is emmited when text send via Dali::Accessibility::Bridge::Say
 * was fully read by TTS module.
 *
 * @return The signal to connect to
 */
DALI_UI_API AccessibilityReadingStoppedSignalType& AccessibilityReadingStoppedSignal(UI::Control control);

/**
 * @brief The signal is emmited when accessibility client asks for object's name.
 * @return signal handler
 */
DALI_UI_API AccessibilityGetNameSignalType& AccessibilityGetNameSignal(UI::Control control);

/**
 * @brief The signal is emmited when accessibility client asks for object's description.
 * @return signal handler
 */
DALI_UI_API AccessibilityGetDescriptionSignalType& AccessibilityGetDescriptionSignal(UI::Control control);

/**
 * @brief The signal is emitted when accessibility client call "DoGesture" method via IPC mechanism.
 * @return signal handler
 */
DALI_UI_API AccessibilityDoGestureSignalType& AccessibilityDoGestureSignal(UI::Control control);

/**
 * @brief The signal is emitted when accessibility client call "DoAction" or "DoActionName" method via IPC mechanism.
 * @return The signal to connect to
 */
DALI_UI_API AccessibilityActionSignalType& AccessibilityActionSignal(UI::Control control);

/**
 * @brief The signal is emitted when accessibility highlight is added or removed from control.
 * @return The signal to connect to
 */
DALI_UI_API AccessibilityHighlightedSignalType& AccessibilityHighlightedSignal(UI::Control control);

/**
 * @brief The method allows connection with other actor with usage of concrete accessibility relation type.
 *
 * @param control object to append attribute to
 * @param destination Actor object
 * @param relation    enumerated value describing relation
 */
DALI_UI_API void AppendAccessibilityRelation(UI::Control control, Dali::Actor destination,
                                             Dali::Accessibility::RelationType relation);

/**
 * @brief The method allows removing relation
 *
 * @param control object to append attribute to
 * @param destination Actor object
 * @param relation    enumerated value describing relation
 */
DALI_UI_API void RemoveAccessibilityRelation(UI::Control control, Dali::Actor destination,
                                             Dali::Accessibility::RelationType relation);

/**
 * @brief Returns a collection of Accessible objects related to current object and grouped by relation type.
 *
 * @param control object to query
 * @return collection of relations
 *
 * @see Dali::Accessibility::Accessible::GetRelationSet()
 */
DALI_UI_API std::vector<Accessibility::Relation> GetAccessibilityRelations(UI::Control control);

/**
 * @brief The method removes all previously appended relations
 *
 * @param control object to append attribute to
 */
DALI_UI_API void ClearAccessibilityRelations(UI::Control control);

/**
 * @brief The method allows to add or modify value matched with given key.
 * Modification take place if key was previously set.
 *
 * @param control object to append attribute to
 * @param key   std::string value
 * @param value std::string value
 */
DALI_UI_API void AppendAccessibilityAttribute(UI::Control control, const std::string& key, const std::string& value);

/**
 * @brief The method erases key with its value from accessibility attributes
 * @param control object to append attribute to
 * @param key std::string value
 */
DALI_UI_API void RemoveAccessibilityAttribute(UI::Control control, const std::string& key);

/**
 * @brief The method clears accessibility attributes
 *
 * @param control object to append attribute to
 */
DALI_UI_API void ClearAccessibilityAttributes(UI::Control control);

/**
 * @brief The method inserts reading information of an accessible object into attributes
 *
 * @param control object to append attribute to
 * @param types Reading information types
 */
DALI_UI_API void SetAccessibilityReadingInfoType(UI::Control control,
                                                 const Dali::Accessibility::ReadingInfoTypes types);

/**
 * @brief The method returns reading information of an accessible object
 *
 * @param control object to append attribute to
 * @return Reading information types
 */
DALI_UI_API Dali::Accessibility::ReadingInfoTypes GetAccessibilityReadingInfoType(UI::Control control);

/**
 * @brief The method erases highlight.
 *
 * @param control object to append attribute to
 * @return bool value, false when it is not possible or something went wrong, at the other way true.
 */
DALI_UI_API bool ClearAccessibilityHighlight(UI::Control control);

/**
 * @brief The method grabs highlight.
 *
 * @param control object to append attribute to
 * @return bool value, false when it is not possible or something went wrong, at the other way true.
 */
DALI_UI_API bool GrabAccessibilityHighlight(UI::Control control);

/**
 * @brief The metod presents bitset of control's states.
 *
 * @param control object to append attribute to
 * @return Dali::Accessibility::States is vector of enumerated State.
 */
DALI_UI_API Dali::Accessibility::States GetAccessibilityStates(UI::Control control);

/**
 * @brief The method force sending notifications about current states to accessibility clients
 *
 * @param control object to append attribute to
 * @param states  mask with states expected to broadcast
 * @param recurse flag pointing if notifications of children's state would be sent
 */
DALI_UI_API void NotifyAccessibilityStateChange(UI::Control control, Dali::Accessibility::States states, bool recurse);

/**
 * @brief The method to get the control's accessibility created or not.
 *
 * @param[in] control object to append attribute to
 * @return True if accessible were created. False otherwise.
 */
DALI_UI_API bool IsAccessibleCreated(UI::Control control);

/**
 * @brief The method to set creatable of control's accessibility.
 * @note This method doesn't remove that already created accessible.
 *
 * @param[in] control object to append attribute to
 * @param[in] enable True if we allow to create accessible. False otherwise.
 */
DALI_UI_API void EnableCreateAccessible(UI::Control control, bool enable);

/**
 * @brief The method to get creatable of control's accessibility.
 *
 * @param[in] control object to append attribute to
 * @return True if we allow to create accessible. False otherwise.
 */
DALI_UI_API bool IsCreateAccessibleEnabled(UI::Control control);

/**
 * @brief The method to emit accessibility state-changed event to accessibility clients
 *
 * @param actor The actor that has accessibility state change.
 * @param state  The accessibility state.
 * @param newValue new value to set.
 */
DALI_UI_API void EmitAccessibilityStateChanged(Dali::Actor actor, Dali::Accessibility::State state, int newValue);

} // namespace DevelControl

} // namespace UI

} // namespace Dali

#endif // DALI_UI_CONTROL_DEVEL_H
