#pragma once

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
#include <vector>
#include <string>
#include <utility>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{

namespace UI
{

// Forward declarations
namespace Integration
{
class ViewImpl;
}

namespace Visual
{
class Base;
}

/**
 * @brief DevelView provides extended APIs for View that were previously
 * exposed via UI::DevelControl C-style free functions.
 *
 * This is a transitional header. These APIs will eventually be folded
 * directly into the View class as member functions.
 *
 * Migrated from: dali-toolkit/devel-api/controls/control-devel.h
 */
namespace DevelView
{

// =========================================================================
// Signal Types (Accessibility)
// =========================================================================

typedef Signal<void()>                                                    AccessibilityActivateSignalType;
typedef Signal<void()>                                                    AccessibilityReadingSkippedSignalType;
typedef Signal<void()>                                                    AccessibilityReadingPausedSignalType;
typedef Signal<void()>                                                    AccessibilityReadingResumedSignalType;
typedef Signal<void()>                                                    AccessibilityReadingCancelledSignalType;
typedef Signal<void()>                                                    AccessibilityReadingStoppedSignalType;
typedef Signal<void(std::string&)>                                        AccessibilityGetNameSignalType;
typedef Signal<void(std::string&)>                                        AccessibilityGetDescriptionSignalType;
typedef Signal<void(std::pair<Dali::Accessibility::GestureInfo, bool>&)>  AccessibilityDoGestureSignalType;
typedef Signal<bool(const Dali::Accessibility::ActionInfo&)>              AccessibilityActionSignalType;
typedef Signal<void(bool)>                                                AccessibilityHighlightedSignalType;

// =========================================================================
// Visual Event Signal
// =========================================================================

using VisualEventSignalType = Signal<void(View, Dali::Property::Index, Dali::Property::Index)>;

// =========================================================================
// Enumerations
// =========================================================================

/**
 * @brief OffScreen rendering type.
 */
enum OffScreenRenderingType
{
  NONE,
  REFRESH_ONCE,
  REFRESH_ALWAYS
};
constexpr unsigned int OffScreenRenderingTypeCount = 3u;

/**
 * @brief View state.
 */
enum State
{
  NORMAL,
  FOCUSED,
  DISABLED
};

// =========================================================================
// Properties
// =========================================================================

/**
 * @brief Extended properties for View.
 *
 * Migrated from DevelControl::Property in control-devel.h.
 */
namespace Property
{
enum
{
  /**
   * @brief Displays a tooltip when the view is hovered over.
   * @details Name "tooltip", type Property::STRING, Property::ARRAY or Property::MAP.
   */
  TOOLTIP = Dali::PROPERTY_REGISTRATION_START_INDEX + 100,

  /**
   * @brief The current state of the view.
   * @details Name "state", type DevelView::State (Property::INTEGER) or Property::STRING
   */
  STATE,

  /**
   * @brief The current sub state of the view.
   * @details Name "subState", type Property::INTEGER or Property::STRING.
   */
  SUB_STATE,

  /**
   * @brief The actor ID of the left focusable view.
   * @details Name "leftFocusableActorId", type Property::INTEGER.
   */
  LEFT_FOCUSABLE_ACTOR_ID,

  /**
   * @brief The actor ID of the right focusable view.
   * @details Name "rightFocusableActorId", type Property::INTEGER.
   */
  RIGHT_FOCUSABLE_ACTOR_ID,

  /**
   * @brief The actor ID of the up focusable view.
   * @details Name "upFocusableActorId", type Property::INTEGER.
   */
  UP_FOCUSABLE_ACTOR_ID,

  /**
   * @brief The actor ID of the down focusable view.
   * @details Name "downFocusableActorId", type Property::INTEGER.
   */
  DOWN_FOCUSABLE_ACTOR_ID,

  /**
   * @brief The shadow of the view.
   * @details Name "shadow", type Property::MAP.
   */
  SHADOW,

  /**
   * @brief The name of object visible in accessibility tree.
   * @details Name "accessibilityName", type Property::STRING.
   */
  ACCESSIBILITY_NAME,

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
   */
  ACCESSIBILITY_ROLE,

  /**
   * @brief Mark of able to highlight object.
   * @details Name "accessibilityHighlightable", type Property::BOOLEAN.
   */
  ACCESSIBILITY_HIGHLIGHTABLE,

  /**
   * @brief Set of accessibility attributes describing object.
   * @details Name "accessibilityAttributes", type Property::MAP.
   */
  ACCESSIBILITY_ATTRIBUTES,

  /**
   * @brief Whether a View and its descendants can emit key signals.
   * @details Name "dispatchKeyEvents", type Property::BOOLEAN.
   */
  DISPATCH_KEY_EVENTS,

  /**
   * @brief Marks the object as invisible to AT-SPI clients.
   * @details Name "accessibilityHidden", type Property::BOOLEAN.
   */
  ACCESSIBILITY_HIDDEN,

  /**
   * @brief The actor ID of the clockwise focusable view.
   * @details Name "clockwiseFocusableActorId", type Property::INTEGER.
   */
  CLOCKWISE_FOCUSABLE_ACTOR_ID,

  /**
   * @brief The actor ID of the counter-clockwise focusable view.
   * @details Name "counterClockwiseFocusableActorId", type Property::INTEGER.
   */
  COUNTER_CLOCKWISE_FOCUSABLE_ACTOR_ID,

  /**
   * @brief Identifier for the automation framework.
   * @details Name "automationId", type Property::STRING.
   */
  AUTOMATION_ID,

  /**
   * @brief The accessibility value represented by the view.
   * @details Name "accessibilityValue", type Property::STRING.
   */
  ACCESSIBILITY_VALUE,

  /**
   * @brief Indicates the accessibility services treat the view as scrollable.
   * @details Name "accessibilityScrollable", type Property::BOOLEAN.
   */
  ACCESSIBILITY_SCROLLABLE,

  /**
   * @brief Bitset integer of AccessibilityState.
   * @details Name "accessibilityStates", type Property::INTEGER.
   */
  ACCESSIBILITY_STATES,

  /**
   * @brief Indicates the accessibility services treat the view as modal.
   * @details Name "accessibilityIsModal", type Property::BOOLEAN.
   */
  ACCESSIBILITY_IS_MODAL,

  /**
   * @brief Whether to draw on offscreen or not.
   * @details Name "offscreenRendering", type Property::INTEGER.
   */
  OFFSCREEN_RENDERING,

  /**
   * @brief The inner shadow of the view.
   * @details Name "innerShadow", type Property::MAP.
   */
  INNER_SHADOW,

  /**
   * @brief The inset borderline of the view.
   * @details Name "borderline", type Property::MAP.
   */
  BORDERLINE,

  /**
   * @brief The radius for the rounded corners of the view.
   * @details Name "viewCornerRadius", type Property::VECTOR4 or Property::FLOAT.
   */
  CORNER_RADIUS = ANIMATABLE_PROPERTY_WITHOUT_UNIFORM_REGISTRATION_START_INDEX,

  /**
   * @brief Whether corner radius is relative or absolute.
   * @details Name "viewCornerRadiusPolicy", type Property::INTEGER.
   */
  CORNER_RADIUS_POLICY,

  /**
   * @brief The squareness for the rounded corners.
   * @details Name "viewCornerSquareness", type Property::VECTOR4 or Property::FLOAT.
   */
  CORNER_SQUARENESS,

  /**
   * @brief The width for the borderline.
   * @details Name "viewBorderlineWidth", type Property::FLOAT.
   */
  BORDERLINE_WIDTH,

  /**
   * @brief The color for the borderline.
   * @details Name "viewBorderlineColor", type Property::VECTOR4.
   */
  BORDERLINE_COLOR,

  /**
   * @brief The offset for the borderline.
   * @details Name "viewBorderlineOffset", type Property::FLOAT.
   */
  BORDERLINE_OFFSET,
};
} // namespace Property

// =========================================================================
// Visual Management (was DevelControl free functions)
// =========================================================================

/**
 * @brief Register a visual by Property Index.
 * @param[in] view The view
 * @param[in] index The Property index of the visual
 * @param[in] visual The visual to register
 */
DALI_UI_API void RegisterVisual(Integration::ViewImpl& view, Dali::Property::Index index, Visual::Base& visual);

/**
 * @brief Register a visual by Property Index with a depth index.
 */
DALI_UI_API void RegisterVisual(Integration::ViewImpl& view, Dali::Property::Index index, Visual::Base& visual, int depthIndex);

/**
 * @brief Register a visual with enable/disable option.
 */
DALI_UI_API void RegisterVisual(Integration::ViewImpl& view, Dali::Property::Index index, Visual::Base& visual, bool enabled);

/**
 * @brief Register a visual with depth index and enable/disable option.
 */
DALI_UI_API void RegisterVisual(Integration::ViewImpl& view, Dali::Property::Index index, Visual::Base& visual, bool enabled, int depthIndex);

/**
 * @brief Erase the entry matching the given index from the list of registered visuals.
 */
DALI_UI_API void UnregisterVisual(Integration::ViewImpl& view, Dali::Property::Index index);

/**
 * @brief Retrieve the visual associated with the given property index.
 * @return The registered visual if exists, otherwise empty handle.
 */
DALI_UI_API Visual::Base GetVisual(const Integration::ViewImpl& view, Dali::Property::Index index);

/**
 * @brief Sets the given visual to be displayed or not.
 */
DALI_UI_API void EnableVisual(Integration::ViewImpl& view, Dali::Property::Index index, bool enable);

/**
 * @brief Queries if the given visual is enabled.
 */
DALI_UI_API bool IsVisualEnabled(const Integration::ViewImpl& view, Dali::Property::Index index);

// =========================================================================
// Visual Actions
// =========================================================================

/**
 * @brief Perform an action on a visual registered to this view.
 */
DALI_UI_API void DoAction(View& view, Dali::Property::Index visualIndex, Dali::Property::Index actionId, const Dali::Property::Value& attributes);

/**
 * @brief Perform an action on a visual registered to this view (extended version).
 */
DALI_UI_API void DoActionExtension(View& view, Dali::Property::Index visualIndex, Dali::Property::Index actionId, const Dali::Any& attributes);

/**
 * @brief Apply corner properties of a view to a registered visual.
 */
DALI_UI_API void EnableCornerPropertiesOverridden(Integration::ViewImpl& view, Visual::Base& visual, bool enable, Dali::Constraint cornerRadiusConstraint = Dali::Constraint());

// =========================================================================
// Input
// =========================================================================

/**
 * @brief Set input method context.
 */
DALI_UI_API void SetInputMethodContext(Integration::ViewImpl& view, InputMethodContext& inputMethodContext);

// =========================================================================
// Visual Event Signal
// =========================================================================

/**
 * @brief This signal is emitted when a visual has an event to notify.
 * @return The signal to connect to
 */
DALI_UI_API VisualEventSignalType& VisualEventSignal(View view);

/**
 * @brief Retrieve the property object for a visual's property.
 */
DALI_UI_API Dali::Property GetVisualProperty(View view, Dali::Property::Index index, Dali::Property::Key visualPropertyKey);

// =========================================================================
// Accessibility Signals
// =========================================================================

DALI_UI_API AccessibilityActivateSignalType&          AccessibilityActivateSignal(View view);
DALI_UI_API AccessibilityReadingSkippedSignalType&    AccessibilityReadingSkippedSignal(View view);
DALI_UI_API AccessibilityReadingPausedSignalType&     AccessibilityReadingPausedSignal(View view);
DALI_UI_API AccessibilityReadingResumedSignalType&    AccessibilityReadingResumedSignal(View view);
DALI_UI_API AccessibilityReadingCancelledSignalType&  AccessibilityReadingCancelledSignal(View view);
DALI_UI_API AccessibilityReadingStoppedSignalType&    AccessibilityReadingStoppedSignal(View view);
DALI_UI_API AccessibilityGetNameSignalType&           AccessibilityGetNameSignal(View view);
DALI_UI_API AccessibilityGetDescriptionSignalType&    AccessibilityGetDescriptionSignal(View view);
DALI_UI_API AccessibilityDoGestureSignalType&         AccessibilityDoGestureSignal(View view);
DALI_UI_API AccessibilityActionSignalType&            AccessibilityActionSignal(View view);
DALI_UI_API AccessibilityHighlightedSignalType&       AccessibilityHighlightedSignal(View view);

// =========================================================================
// Accessibility Methods
// =========================================================================

DALI_UI_API void AppendAccessibilityRelation(View view, Dali::Actor destination, Dali::Accessibility::RelationType relation);
DALI_UI_API void RemoveAccessibilityRelation(View view, Dali::Actor destination, Dali::Accessibility::RelationType relation);
DALI_UI_API std::vector<Accessibility::Relation> GetAccessibilityRelations(View view);
DALI_UI_API void ClearAccessibilityRelations(View view);

DALI_UI_API void AppendAccessibilityAttribute(View view, const std::string& key, const std::string& value);
DALI_UI_API void RemoveAccessibilityAttribute(View view, const std::string& key);
DALI_UI_API void ClearAccessibilityAttributes(View view);

DALI_UI_API void SetAccessibilityReadingInfoType(View view, const Dali::Accessibility::ReadingInfoTypes types);
DALI_UI_API Dali::Accessibility::ReadingInfoTypes GetAccessibilityReadingInfoType(View view);

DALI_UI_API bool ClearAccessibilityHighlight(View view);
DALI_UI_API bool GrabAccessibilityHighlight(View view);

DALI_UI_API Dali::Accessibility::States GetAccessibilityStates(View view);
DALI_UI_API void NotifyAccessibilityStateChange(View view, Dali::Accessibility::States states, bool recurse);

DALI_UI_API bool IsAccessibleCreated(View view);
DALI_UI_API void EnableCreateAccessible(View view, bool enable);
DALI_UI_API bool IsCreateAccessibleEnabled(View view);
DALI_UI_API void EmitAccessibilityStateChanged(Dali::Actor actor, Dali::Accessibility::State state, int newValue);

} // namespace DevelView

} // namespace UI

} // namespace Dali
