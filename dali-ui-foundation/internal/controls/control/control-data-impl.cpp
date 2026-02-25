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
#include "control-data-impl.h"
#include "control-accessibility-data.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/accessibility.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/object/handle-devel.h>
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <cstring>
#include <limits>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/controls/control-devel.h>
#include <dali-ui-foundation/public-api/controls/control-impl.h>
#include <dali-ui-foundation/public-api/controls/control.h>

namespace Dali
{
namespace UI
{
namespace Internal
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_CONTROL");
#endif

extern const Dali::Scripting::StringEnum ControlStateTable[];
extern const unsigned int ControlStateTableCount;

// Not static or anonymous - shared with other translation units
const Scripting::StringEnum ControlStateTable[] = {
    {"NORMAL", DevelControl::NORMAL},
    {"FOCUSED", DevelControl::FOCUSED},
    {"DISABLED", DevelControl::DISABLED},
};
const unsigned int ControlStateTableCount = sizeof(ControlStateTable) / sizeof(ControlStateTable[0]);

// Accessibility action names
constexpr const char* ACTION_ACCESSIBILITY_ACTIVATE = "activate";
constexpr const char* ACTION_ACCESSIBILITY_ESCAPE = "escape";
constexpr const char* ACTION_ACCESSIBILITY_INCREMENT = "increment";
constexpr const char* ACTION_ACCESSIBILITY_DECREMENT = "decrement";

// Legacy actions
constexpr const char* ACTION_ACCESSIBILITY_READING_CANCELLED = "ReadingCancelled";
constexpr const char* ACTION_ACCESSIBILITY_READING_PAUSED = "ReadingPaused";
constexpr const char* ACTION_ACCESSIBILITY_READING_RESUMED = "ReadingResumed";
constexpr const char* ACTION_ACCESSIBILITY_READING_SKIPPED = "ReadingSkipped";
constexpr const char* ACTION_ACCESSIBILITY_READING_STOPPED = "ReadingStopped";

bool PerformAccessibilityAction(UI::Control control, const std::string& actionName, const Property::Map& attributes)
{
  using Dali::Accessibility::ActionType;
  DALI_ASSERT_DEBUG(control);
  DALI_ASSERT_DEBUG(!DevelControl::AccessibilityActionSignal(control).Empty());

  ActionType action = ActionType::MAX_COUNT;
  if (actionName == ACTION_ACCESSIBILITY_ACTIVATE)
  {
    action = ActionType::ACTIVATE;
  }
  else if (actionName == ACTION_ACCESSIBILITY_ESCAPE)
  {
    action = ActionType::ESCAPE;
  }
  else if (actionName == ACTION_ACCESSIBILITY_INCREMENT)
  {
    action = ActionType::INCREMENT;
  }
  else if (actionName == ACTION_ACCESSIBILITY_DECREMENT)
  {
    action = ActionType::DECREMENT;
  }

  if (action != ActionType::MAX_COUNT)
  {
    bool success = DevelControl::AccessibilityActionSignal(control).Emit({action, Dali::Actor{}});
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Performed AccessibilityAction: %s, success : %d\n", actionName.c_str(),
                  success);
    return success;
  }

  return false;
}

bool PerformLegacyAccessibilityAction(UI::Control control, const std::string& actionName)
{
  bool ret = true;
  if (0 == strcmp(actionName.c_str(), ACTION_ACCESSIBILITY_ACTIVATE))
  {
    if (!DevelControl::AccessibilityActivateSignal(control).Empty())
    {
      DevelControl::AccessibilityActivateSignal(control).Emit();
    }
    else
    {
      ret = Internal::GetImplementation(control).OnAccessibilityActivated();
    }
  }
  else if (0 == strcmp(actionName.c_str(), ACTION_ACCESSIBILITY_READING_SKIPPED))
  {
    if (!DevelControl::AccessibilityReadingSkippedSignal(control).Empty())
    {
      DevelControl::AccessibilityReadingSkippedSignal(control).Emit();
    }
  }
  else if (0 == strcmp(actionName.c_str(), ACTION_ACCESSIBILITY_READING_PAUSED))
  {
    if (!DevelControl::AccessibilityReadingPausedSignal(control).Empty())
    {
      DevelControl::AccessibilityReadingPausedSignal(control).Emit();
    }
  }
  else if (0 == strcmp(actionName.c_str(), ACTION_ACCESSIBILITY_READING_RESUMED))
  {
    if (!DevelControl::AccessibilityReadingResumedSignal(control).Empty())
    {
      DevelControl::AccessibilityReadingResumedSignal(control).Emit();
    }
  }
  else if (0 == strcmp(actionName.c_str(), ACTION_ACCESSIBILITY_READING_CANCELLED))
  {
    if (!DevelControl::AccessibilityReadingCancelledSignal(control).Empty())
    {
      DevelControl::AccessibilityReadingCancelledSignal(control).Emit();
    }
  }
  else if (0 == strcmp(actionName.c_str(), ACTION_ACCESSIBILITY_READING_STOPPED))
  {
    if (!DevelControl::AccessibilityReadingStoppedSignal(control).Empty())
    {
      DevelControl::AccessibilityReadingStoppedSignal(control).Emit();
    }
  }
  else
  {
    ret = false;
  }

  if (ret)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Performed Legacy AccessibilityAction: %s\n", actionName.c_str());
  }
  return ret;
}

bool DoAccessibilityAction(BaseObject* object, const std::string& actionName, const Property::Map& attributes)
{
  Dali::BaseHandle handle(object);

  UI::Control control = UI::Control::DownCast(handle);

  DALI_ASSERT_ALWAYS(control);

  if (!DevelControl::AccessibilityActionSignal(control).Empty())
  {
    return PerformAccessibilityAction(control, actionName, attributes);
  }

  // Fall back to legacy action if no ActionSignal is connected
  return PerformLegacyAccessibilityAction(control, actionName);
}

bool DoLegacyAccessibilityAction(BaseObject* object, const std::string& actionName, const Property::Map& attributes)
{
  Dali::BaseHandle handle(object);

  UI::Control control = UI::Control::DownCast(handle);

  DALI_ASSERT_ALWAYS(control);

  return PerformLegacyAccessibilityAction(control, actionName);
}

const char* SIGNAL_KEY_EVENT = "keyEvent";
const char* SIGNAL_KEY_INPUT_FOCUS_GAINED = "keyInputFocusGained";
const char* SIGNAL_KEY_INPUT_FOCUS_LOST = "keyInputFocusLost";
const char* SIGNAL_TAPPED = "tapped";
const char* SIGNAL_PANNED = "panned";
const char* SIGNAL_PINCHED = "pinched";
const char* SIGNAL_LONG_PRESSED = "longPressed";
const char* SIGNAL_GET_NAME = "getName";
const char* SIGNAL_GET_DESCRIPTION = "getDescription";
const char* SIGNAL_DO_GESTURE = "doGesture";

/**
 * Connects a callback function with the object's signals.
 */
static bool DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName,
                            FunctorDelegate* functor)
{
  Dali::BaseHandle handle(object);

  bool connected(false);
  UI::Control control = UI::Control::DownCast(handle);
  if (control)
  {
    Internal::Control& controlImpl(Internal::GetImplementation(control));
    connected = true;

    if (0 == strcmp(signalName.c_str(), SIGNAL_KEY_EVENT))
    {
      controlImpl.KeyEventSignal().Connect(tracker, functor);
    }
    else if (0 == strcmp(signalName.c_str(), SIGNAL_KEY_INPUT_FOCUS_GAINED))
    {
      controlImpl.KeyInputFocusGainedSignal().Connect(tracker, functor);
    }
    else if (0 == strcmp(signalName.c_str(), SIGNAL_KEY_INPUT_FOCUS_LOST))
    {
      controlImpl.KeyInputFocusLostSignal().Connect(tracker, functor);
    }
    else if (0 == strcmp(signalName.c_str(), SIGNAL_TAPPED))
    {
      controlImpl.EnableGestureDetection(GestureType::TAP);
      controlImpl.GetTapGestureDetector().DetectedSignal().Connect(tracker, functor);
    }
    else if (0 == strcmp(signalName.c_str(), SIGNAL_PANNED))
    {
      controlImpl.EnableGestureDetection(GestureType::PAN);
      controlImpl.GetPanGestureDetector().DetectedSignal().Connect(tracker, functor);
    }
    else if (0 == strcmp(signalName.c_str(), SIGNAL_PINCHED))
    {
      controlImpl.EnableGestureDetection(GestureType::PINCH);
      controlImpl.GetPinchGestureDetector().DetectedSignal().Connect(tracker, functor);
    }
    else if (0 == strcmp(signalName.c_str(), SIGNAL_LONG_PRESSED))
    {
      controlImpl.EnableGestureDetection(GestureType::LONG_PRESS);
      controlImpl.GetLongPressGestureDetector().DetectedSignal().Connect(tracker, functor);
    }
    else if (0 == strcmp(signalName.c_str(), SIGNAL_GET_NAME))
    {
      DevelControl::AccessibilityGetNameSignal(control).Connect(tracker, functor);
    }
    else if (0 == strcmp(signalName.c_str(), SIGNAL_GET_DESCRIPTION))
    {
      DevelControl::AccessibilityGetDescriptionSignal(control).Connect(tracker, functor);
    }
    else if (0 == strcmp(signalName.c_str(), SIGNAL_DO_GESTURE))
    {
      DevelControl::AccessibilityDoGestureSignal(control).Connect(tracker, functor);
    }
  }
  return connected;
}

/**
 * Creates control through type registry
 */
BaseHandle Create()
{
  return Internal::Control::New();
}

// Setup signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(Control, CustomActor, Create);

// Note: Properties are registered separately below.

SignalConnectorType registerSignal1(typeRegistration, SIGNAL_KEY_EVENT, &DoConnectSignal);
SignalConnectorType registerSignal2(typeRegistration, SIGNAL_KEY_INPUT_FOCUS_GAINED, &DoConnectSignal);
SignalConnectorType registerSignal3(typeRegistration, SIGNAL_KEY_INPUT_FOCUS_LOST, &DoConnectSignal);
SignalConnectorType registerSignal4(typeRegistration, SIGNAL_TAPPED, &DoConnectSignal);
SignalConnectorType registerSignal5(typeRegistration, SIGNAL_PANNED, &DoConnectSignal);
SignalConnectorType registerSignal6(typeRegistration, SIGNAL_PINCHED, &DoConnectSignal);
SignalConnectorType registerSignal7(typeRegistration, SIGNAL_LONG_PRESSED, &DoConnectSignal);
SignalConnectorType registerSignal8(typeRegistration, SIGNAL_GET_NAME, &DoConnectSignal);
SignalConnectorType registerSignal9(typeRegistration, SIGNAL_GET_DESCRIPTION, &DoConnectSignal);
SignalConnectorType registerSignal10(typeRegistration, SIGNAL_DO_GESTURE, &DoConnectSignal);

// === Accessibility Actions === START
TypeAction registerAction1(typeRegistration, ACTION_ACCESSIBILITY_ACTIVATE, &DoAccessibilityAction);
TypeAction registerAction2(typeRegistration, ACTION_ACCESSIBILITY_ESCAPE, &DoAccessibilityAction);
TypeAction registerAction3(typeRegistration, ACTION_ACCESSIBILITY_INCREMENT, &DoAccessibilityAction);
TypeAction registerAction4(typeRegistration, ACTION_ACCESSIBILITY_DECREMENT, &DoAccessibilityAction);
// === Accessibility Actions === END

// === Legacy Accessibility Actions === START
TypeAction registerAction5(typeRegistration, ACTION_ACCESSIBILITY_READING_SKIPPED, &DoLegacyAccessibilityAction);
TypeAction registerAction6(typeRegistration, ACTION_ACCESSIBILITY_READING_CANCELLED, &DoLegacyAccessibilityAction);
TypeAction registerAction7(typeRegistration, ACTION_ACCESSIBILITY_READING_STOPPED, &DoLegacyAccessibilityAction);
TypeAction registerAction8(typeRegistration, ACTION_ACCESSIBILITY_READING_PAUSED, &DoLegacyAccessibilityAction);
TypeAction registerAction9(typeRegistration, ACTION_ACCESSIBILITY_READING_RESUMED, &DoLegacyAccessibilityAction);
// === Legacy Accessibility Actions === END

DALI_TYPE_REGISTRATION_END()

} // unnamed namespace

// clang-format off
// Properties registered without macro to use specific member variables.
// Note: PROPERTY_2 (background) and PROPERTY_3 (tooltip) removed - Visual/Tooltip related
const PropertyRegistration Control::Impl::PROPERTY_1(typeRegistration,  "styleName",                      UI::Control::Property::STYLE_NAME,                            Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_2(typeRegistration,  "keyInputFocus",                  UI::Control::Property::KEY_INPUT_FOCUS,                        Property::BOOLEAN, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_3(typeRegistration,  "margin",                         UI::Control::Property::MARGIN,                                 Property::EXTENTS, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_4(typeRegistration,  "padding",                        UI::Control::Property::PADDING,                                Property::EXTENTS, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_5(typeRegistration,  "state",                          UI::DevelControl::Property::STATE,                             Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_6(typeRegistration,  "subState",                       UI::DevelControl::Property::SUB_STATE,                         Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_7(typeRegistration,  "leftFocusableActorId",           UI::DevelControl::Property::LEFT_FOCUSABLE_ACTOR_ID,           Property::INTEGER, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_8(typeRegistration,  "rightFocusableActorId",          UI::DevelControl::Property::RIGHT_FOCUSABLE_ACTOR_ID,          Property::INTEGER, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_9(typeRegistration,  "upFocusableActorId",             UI::DevelControl::Property::UP_FOCUSABLE_ACTOR_ID,             Property::INTEGER, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_10(typeRegistration, "downFocusableActorId",           UI::DevelControl::Property::DOWN_FOCUSABLE_ACTOR_ID,           Property::INTEGER, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_11(typeRegistration, "accessibilityName",              UI::DevelControl::Property::ACCESSIBILITY_NAME,                Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_12(typeRegistration, "accessibilityDescription",       UI::DevelControl::Property::ACCESSIBILITY_DESCRIPTION,         Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_13(typeRegistration, "accessibilityRole",              UI::DevelControl::Property::ACCESSIBILITY_ROLE,                Property::INTEGER, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_14(typeRegistration, "accessibilityHighlightable",     UI::DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE,       Property::BOOLEAN, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_15(typeRegistration, "accessibilityAttributes",        UI::DevelControl::Property::ACCESSIBILITY_ATTRIBUTES,          Property::MAP,     &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_16(typeRegistration, "dispatchKeyEvents",              UI::DevelControl::Property::DISPATCH_KEY_EVENTS,               Property::BOOLEAN, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_17(typeRegistration, "accessibilityHidden",            UI::DevelControl::Property::ACCESSIBILITY_HIDDEN,              Property::BOOLEAN, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_18(typeRegistration, "clockwiseFocusableActorId",      UI::DevelControl::Property::CLOCKWISE_FOCUSABLE_ACTOR_ID,      Property::INTEGER, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_19(typeRegistration, "counterClockwiseFocusableActorId", UI::DevelControl::Property::COUNTER_CLOCKWISE_FOCUSABLE_ACTOR_ID, Property::INTEGER, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_20(typeRegistration, "automationId",                   UI::DevelControl::Property::AUTOMATION_ID,                     Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_21(typeRegistration, "accessibilityValue",             UI::DevelControl::Property::ACCESSIBILITY_VALUE,               Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_22(typeRegistration, "accessibilityScrollable",        UI::DevelControl::Property::ACCESSIBILITY_SCROLLABLE,          Property::BOOLEAN, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_23(typeRegistration, "accessibilityStates",            UI::DevelControl::Property::ACCESSIBILITY_STATES,              Property::INTEGER, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
// clang-format on

Control::Impl::Impl(Control& controlImpl)
  : mControlImpl(controlImpl),
    mState(DevelControl::NORMAL),
    mSubStateName(""),
    mAccessibilityData(nullptr),
    mLeftFocusableActorId(-1),
    mRightFocusableActorId(-1),
    mUpFocusableActorId(-1),
    mDownFocusableActorId(-1),
    mClockwiseFocusableActorId(-1),
    mCounterClockwiseFocusableActorId(-1),
    mStyleName(""),
    mStartingPinchScale(nullptr),
    mMargin(0, 0, 0, 0),
    mPadding(0, 0, 0, 0),
    mSize(0, 0),
    mKeyEventSignal(),
    mKeyInputFocusGainedSignal(),
    mKeyInputFocusLostSignal(),
    mPinchGestureDetector(),
    mPanGestureDetector(),
    mTapGestureDetector(),
    mLongPressGestureDetector(),
    mInputMethodContext(),
    mIdleCallback(nullptr),
    mFlags(Control::ControlBehaviour(CONTROL_BEHAVIOUR_DEFAULT)),
    mAccessibilityRole{static_cast<int32_t>(DevelControl::AccessibilityRole::NONE)},
    mIsKeyboardNavigationSupported(false),
    mIsKeyboardFocusGroup(false),
    mDispatchKeyEvents(true),
    mAccessibleCreatable(true),
    mProcessorRegistered(false)
{
}

Control::Impl::~Impl()
{
  // All gesture detectors will be destroyed so no need to disconnect.
  delete mStartingPinchScale;

  if (mIdleCallback && Adaptor::IsAvailable())
  {
    // Removes the callback from the callback manager in case the control is destroyed before the callback is executed.
    Adaptor::Get().RemoveIdle(mIdleCallback);
  }
}

Control::Impl& Control::Impl::Get(Internal::Control& internalControl)
{
  DALI_ASSERT_ALWAYS(Stage::IsCoreThread() && "Core is not installed. Might call this API from worker thread?");

  return *internalControl.mImpl;
}

const Control::Impl& Control::Impl::Get(const Internal::Control& internalControl)
{
  DALI_ASSERT_ALWAYS(Stage::IsCoreThread() && "Core is not installed. Might call this API from worker thread?");

  return *internalControl.mImpl;
}

// Gesture Detection Methods
void Control::Impl::PinchDetected(Actor actor, const PinchGesture& pinch)
{
  mControlImpl.OnPinch(pinch);
}

void Control::Impl::PanDetected(Actor actor, const PanGesture& pan)
{
  mControlImpl.OnPan(pan);
}

void Control::Impl::TapDetected(Actor actor, const TapGesture& tap)
{
  mControlImpl.OnTap(tap);
}

void Control::Impl::LongPressDetected(Actor actor, const LongPressGesture& longPress)
{
  mControlImpl.OnLongPress(longPress);
}

void Control::Impl::OnSceneConnection()
{
  // No visual data to connect in dali-ui foundation
}

void Control::Impl::OnSceneDisconnection()
{
  // No visual data to disconnect in dali-ui foundation
}

void Control::Impl::SetProperty(BaseObject* object, Property::Index index, const Property::Value& value)
{
  DALI_ASSERT_ALWAYS(Stage::IsCoreThread() && "Core is not installed. Might call this API from worker thread?");

  UI::Control control = UI::Control::DownCast(BaseHandle(object));

  if (control)
  {
    Control& controlImpl(GetImplementation(control));

    switch (index)
    {
      case UI::Control::Property::STYLE_NAME:
      {
        controlImpl.SetStyleName(value.Get<std::string>());
        break;
      }

      case UI::DevelControl::Property::STATE:
      {
        bool withTransitions = true;
        const Property::Value* valuePtr = &value;
        const Property::Map* map = value.GetMap();
        if (map)
        {
          Property::Value* value2 = map->Find("withTransitions");
          if (value2)
          {
            withTransitions = value2->Get<bool>();
          }

          valuePtr = map->Find("state");
        }

        if (valuePtr)
        {
          DevelControl::State state(controlImpl.mImpl->mState);
          if (Scripting::GetEnumerationProperty<DevelControl::State>(*valuePtr, ControlStateTable,
                                                                     ControlStateTableCount, state))
          {
            controlImpl.mImpl->SetState(state, withTransitions);
          }
        }
      }
      break;

      case UI::DevelControl::Property::SUB_STATE:
      {
        std::string subState;
        if (value.Get(subState))
        {
          controlImpl.mImpl->SetSubState(subState);
        }
      }
      break;

      case UI::DevelControl::Property::LEFT_FOCUSABLE_ACTOR_ID:
      {
        int focusId;
        if (value.Get(focusId))
        {
          controlImpl.mImpl->mLeftFocusableActorId = focusId;
        }
      }
      break;

      case UI::DevelControl::Property::RIGHT_FOCUSABLE_ACTOR_ID:
      {
        int focusId;
        if (value.Get(focusId))
        {
          controlImpl.mImpl->mRightFocusableActorId = focusId;
        }
      }
      break;

      case UI::DevelControl::Property::UP_FOCUSABLE_ACTOR_ID:
      {
        int focusId;
        if (value.Get(focusId))
        {
          controlImpl.mImpl->mUpFocusableActorId = focusId;
        }
      }
      break;

      case UI::DevelControl::Property::DOWN_FOCUSABLE_ACTOR_ID:
      {
        int focusId;
        if (value.Get(focusId))
        {
          controlImpl.mImpl->mDownFocusableActorId = focusId;
        }
      }
      break;

      case UI::Control::Property::KEY_INPUT_FOCUS:
      {
        if (value.Get<bool>())
        {
          controlImpl.SetKeyInputFocus();
        }
        else
        {
          controlImpl.ClearKeyInputFocus();
        }
        break;
      }

      case UI::Control::Property::MARGIN:
      {
        Extents margin;
        if (value.Get(margin))
        {
          controlImpl.mImpl->SetMargin(margin);
        }
        break;
      }

      case UI::Control::Property::PADDING:
      {
        Extents padding;
        if (value.Get(padding))
        {
          controlImpl.mImpl->SetPadding(padding);
        }
        break;
      }

      case UI::DevelControl::Property::ACCESSIBILITY_NAME:
      {
        std::string name;
        if (value.Get(name))
        {
          if (DALI_LIKELY(controlImpl.mImpl->GetAccessibilityData()) || !name.empty())
          {
            controlImpl.mImpl->GetOrCreateAccessibilityData().mAccessibilityProps.name = std::move(name);
          }
        }
        break;
      }

      case UI::DevelControl::Property::ACCESSIBILITY_DESCRIPTION:
      {
        std::string text;
        if (value.Get(text))
        {
          if (DALI_LIKELY(controlImpl.mImpl->GetAccessibilityData()) || !text.empty())
          {
            controlImpl.mImpl->GetOrCreateAccessibilityData().mAccessibilityProps.description = std::move(text);
          }
        }
        break;
      }

      case UI::DevelControl::Property::ACCESSIBILITY_ROLE:
      {
        int32_t role;
        if (value.Get(role))
        {
          controlImpl.mImpl->mAccessibilityRole = role;
        }
        break;
      }

      case UI::DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE:
      {
        bool highlightable;
        if (value.Get(highlightable))
        {
          controlImpl.mImpl->GetOrCreateAccessibilityData().mAccessibilityProps.isHighlightable =
              highlightable ? TriStateProperty::TRUE : TriStateProperty::FALSE;
        }
        break;
      }

      case UI::DevelControl::Property::ACCESSIBILITY_ATTRIBUTES:
      {
        const Property::Map* map = value.GetMap();
        if (map)
        {
          if (DALI_LIKELY(controlImpl.mImpl->GetAccessibilityData()) || !map->Empty())
          {
            controlImpl.mImpl->GetOrCreateAccessibilityData().mAccessibilityProps.extraAttributes = *map;
          }
        }
        break;
      }

      case UI::DevelControl::Property::DISPATCH_KEY_EVENTS:
      {
        bool dispatch;
        if (value.Get(dispatch))
        {
          controlImpl.mImpl->mDispatchKeyEvents = dispatch;
        }
        break;
      }

      case UI::DevelControl::Property::ACCESSIBILITY_HIDDEN:
      {
        bool hidden;
        if (value.Get(hidden))
        {
          const auto* accessibilityData = controlImpl.mImpl->GetAccessibilityData();
          const bool originalHidden =
              DALI_LIKELY(accessibilityData) ? accessibilityData->mAccessibilityProps.isHidden : false;
          if (originalHidden != hidden)
          {
            controlImpl.mImpl->GetOrCreateAccessibilityData().mAccessibilityProps.isHidden = hidden;

            auto accessible = controlImpl.GetAccessibleObject();
            if (DALI_LIKELY(accessible))
            {
              auto* parent = dynamic_cast<Dali::Accessibility::ActorAccessible*>(accessible->GetParent());
              if (parent)
              {
                parent->OnChildrenChanged();
              }
            }
          }
        }
        break;
      }

      case UI::DevelControl::Property::CLOCKWISE_FOCUSABLE_ACTOR_ID:
      {
        int focusId;
        if (value.Get(focusId))
        {
          controlImpl.mImpl->mClockwiseFocusableActorId = focusId;
        }
        break;
      }

      case UI::DevelControl::Property::COUNTER_CLOCKWISE_FOCUSABLE_ACTOR_ID:
      {
        int focusId;
        if (value.Get(focusId))
        {
          controlImpl.mImpl->mCounterClockwiseFocusableActorId = focusId;
        }
        break;
      }

      case UI::DevelControl::Property::AUTOMATION_ID:
      {
        std::string automationId;
        if (value.Get(automationId))
        {
          if (DALI_LIKELY(controlImpl.mImpl->GetAccessibilityData()) || !automationId.empty())
          {
            controlImpl.mImpl->GetOrCreateAccessibilityData().mAccessibilityProps.automationId =
                std::move(automationId);
          }
        }
        break;
      }

      case UI::DevelControl::Property::ACCESSIBILITY_VALUE:
      {
        std::string accessibilityValue;
        if (value.Get(accessibilityValue))
        {
          if (DALI_LIKELY(controlImpl.mImpl->GetAccessibilityData()) || !accessibilityValue.empty())
          {
            controlImpl.mImpl->GetOrCreateAccessibilityData().mAccessibilityProps.value = std::move(accessibilityValue);
          }
        }
        break;
      }

      case UI::DevelControl::Property::ACCESSIBILITY_SCROLLABLE:
      {
        bool isScrollable;
        if (value.Get(isScrollable))
        {
          if (DALI_LIKELY(controlImpl.mImpl->GetAccessibilityData()) || isScrollable)
          {
            controlImpl.mImpl->GetOrCreateAccessibilityData().mAccessibilityProps.isScrollable = isScrollable;
          }
        }
        break;
      }

      case UI::DevelControl::Property::ACCESSIBILITY_STATES:
      {
        int32_t states;
        if (value.Get(states))
        {
          if (DALI_LIKELY(controlImpl.mImpl->GetAccessibilityData()) ||
              states != static_cast<int32_t>(AccessibilityData::GetDefaultControlAccessibilityStates().GetRawData32()))
          {
            controlImpl.mImpl->GetOrCreateAccessibilityData().mAccessibilityProps.states =
                DevelControl::AccessibilityStates{static_cast<uint32_t>(states)};
          }
        }
        break;
      }

      case UI::DevelControl::Property::ACCESSIBILITY_IS_MODAL:
      {
        bool isModal;
        if (value.Get(isModal))
        {
          if (DALI_LIKELY(controlImpl.mImpl->GetAccessibilityData()) || isModal)
          {
            controlImpl.mImpl->GetOrCreateAccessibilityData().mAccessibilityProps.isModal = isModal;
          }
        }
        break;
      }
    }
  }
}

Property::Value Control::Impl::GetProperty(BaseObject* object, Property::Index index)
{
  DALI_ASSERT_ALWAYS(Stage::IsCoreThread() && "Core is not installed. Might call this API from worker thread?");

  Property::Value value;

  UI::Control control = UI::Control::DownCast(BaseHandle(object));

  if (control)
  {
    Control& controlImpl(GetImplementation(control));

    switch (index)
    {
      case UI::Control::Property::STYLE_NAME:
      {
        value = controlImpl.GetStyleName();
        break;
      }

      case UI::DevelControl::Property::STATE:
      {
        value = controlImpl.mImpl->mState;
        break;
      }

      case UI::DevelControl::Property::SUB_STATE:
      {
        value = controlImpl.mImpl->mSubStateName;
        break;
      }

      case UI::DevelControl::Property::LEFT_FOCUSABLE_ACTOR_ID:
      {
        value = controlImpl.mImpl->mLeftFocusableActorId;
        break;
      }

      case UI::DevelControl::Property::RIGHT_FOCUSABLE_ACTOR_ID:
      {
        value = controlImpl.mImpl->mRightFocusableActorId;
        break;
      }

      case UI::DevelControl::Property::UP_FOCUSABLE_ACTOR_ID:
      {
        value = controlImpl.mImpl->mUpFocusableActorId;
        break;
      }

      case UI::DevelControl::Property::DOWN_FOCUSABLE_ACTOR_ID:
      {
        value = controlImpl.mImpl->mDownFocusableActorId;
        break;
      }

      case UI::Control::Property::KEY_INPUT_FOCUS:
      {
        value = controlImpl.HasKeyInputFocus();
        break;
      }

      case UI::Control::Property::MARGIN:
      {
        value = controlImpl.mImpl->GetMargin();
        break;
      }

      case UI::Control::Property::PADDING:
      {
        value = controlImpl.mImpl->GetPadding();
        break;
      }

      case UI::DevelControl::Property::ACCESSIBILITY_NAME:
      {
        const auto* accessibilityData = controlImpl.mImpl->GetAccessibilityData();
        value = DALI_LIKELY(accessibilityData) ? accessibilityData->mAccessibilityProps.name : "";
        break;
      }

      case UI::DevelControl::Property::ACCESSIBILITY_DESCRIPTION:
      {
        const auto* accessibilityData = controlImpl.mImpl->GetAccessibilityData();
        value = DALI_LIKELY(accessibilityData) ? accessibilityData->mAccessibilityProps.description : "";
        break;
      }

      case UI::DevelControl::Property::ACCESSIBILITY_ROLE:
      {
        value = controlImpl.mImpl->mAccessibilityRole;
        break;
      }

      case UI::DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE:
      {
        const auto* accessibilityData = controlImpl.mImpl->GetAccessibilityData();
        value = (DALI_LIKELY(accessibilityData) &&
                 accessibilityData->mAccessibilityProps.isHighlightable == TriStateProperty::TRUE)
                    ? true
                    : false;
        break;
      }

      case UI::DevelControl::Property::ACCESSIBILITY_ATTRIBUTES:
      {
        const auto* accessibilityData = controlImpl.mImpl->GetAccessibilityData();
        value =
            DALI_LIKELY(accessibilityData) ? accessibilityData->mAccessibilityProps.extraAttributes : Property::Map();
        break;
      }

      case UI::DevelControl::Property::DISPATCH_KEY_EVENTS:
      {
        value = controlImpl.mImpl->mDispatchKeyEvents;
        break;
      }

      case UI::DevelControl::Property::ACCESSIBILITY_HIDDEN:
      {
        const auto* accessibilityData = controlImpl.mImpl->GetAccessibilityData();
        value = DALI_LIKELY(accessibilityData) ? accessibilityData->mAccessibilityProps.isHidden : false;
        break;
      }

      case UI::DevelControl::Property::CLOCKWISE_FOCUSABLE_ACTOR_ID:
      {
        value = controlImpl.mImpl->mClockwiseFocusableActorId;
        break;
      }

      case UI::DevelControl::Property::COUNTER_CLOCKWISE_FOCUSABLE_ACTOR_ID:
      {
        value = controlImpl.mImpl->mCounterClockwiseFocusableActorId;
        break;
      }

      case UI::DevelControl::Property::AUTOMATION_ID:
      {
        const auto* accessibilityData = controlImpl.mImpl->GetAccessibilityData();
        value = DALI_LIKELY(accessibilityData) ? accessibilityData->mAccessibilityProps.automationId : "";
        break;
      }

      case UI::DevelControl::Property::ACCESSIBILITY_VALUE:
      {
        const auto* accessibilityData = controlImpl.mImpl->GetAccessibilityData();
        value = DALI_LIKELY(accessibilityData) ? accessibilityData->mAccessibilityProps.value : "";
        break;
      }

      case UI::DevelControl::Property::ACCESSIBILITY_SCROLLABLE:
      {
        const auto* accessibilityData = controlImpl.mImpl->GetAccessibilityData();
        value = DALI_LIKELY(accessibilityData) ? accessibilityData->mAccessibilityProps.isScrollable : false;
        break;
      }

      case UI::DevelControl::Property::ACCESSIBILITY_STATES:
      {
        const auto* accessibilityData = controlImpl.mImpl->GetAccessibilityData();
        value = static_cast<int32_t>((DALI_LIKELY(accessibilityData)
                                          ? accessibilityData->mAccessibilityProps.states
                                          : AccessibilityData::GetDefaultControlAccessibilityStates())
                                         .GetRawData32());
        break;
      }

      case UI::DevelControl::Property::ACCESSIBILITY_IS_MODAL:
      {
        const auto* accessibilityData = controlImpl.mImpl->GetAccessibilityData();
        value = DALI_LIKELY(accessibilityData) ? accessibilityData->mAccessibilityProps.isModal : false;
        break;
      }
    }
  }

  return value;
}

void Control::Impl::SetState(DevelControl::State newState, bool withTransitions)
{
  Dali::CustomActor handle(mControlImpl.GetOwner());
  DALI_LOG_INFO(
      gLogFilter, Debug::Concise, "Control::Impl::SetState: %s\n",
      (mState == DevelControl::NORMAL
           ? "NORMAL"
           : (mState == DevelControl::FOCUSED ? "FOCUSED" : (mState == DevelControl::DISABLED ? "DISABLED" : "NONE"))));

  if (mState != newState)
  {
    mState = newState;
    // Note: Visual/StyleManager state transitions removed in dali-ui
  }
}

void Control::Impl::SetSubState(const std::string& subStateName, bool withTransitions)
{
  if (mSubStateName != subStateName)
  {
    mSubStateName = subStateName;
    // Note: Visual/StyleManager sub-state transitions removed in dali-ui
  }
}

void Control::Impl::SetMargin(Extents margin)
{
  mControlImpl.mImpl->mMargin = margin;

  // Trigger a size negotiation request that may be needed when setting a margin.
  mControlImpl.RelayoutRequest();
}

Extents Control::Impl::GetMargin() const
{
  return mControlImpl.mImpl->mMargin;
}

void Control::Impl::SetPadding(Extents padding)
{
  mControlImpl.mImpl->mPadding = padding;

  // Trigger a size negotiation request that may be needed when setting a padding.
  mControlImpl.RelayoutRequest();
}

Extents Control::Impl::GetPadding() const
{
  return mControlImpl.mImpl->mPadding;
}

void Control::Impl::SetInputMethodContext(InputMethodContext& inputMethodContext)
{
  mInputMethodContext = inputMethodContext;
}

bool Control::Impl::FilterKeyEvent(const KeyEvent& event)
{
  bool consumed(false);

  if (mInputMethodContext)
  {
    consumed = mInputMethodContext.FilterEventKey(event);
  }
  return consumed;
}

Control::Impl::AccessibilityData& Control::Impl::GetOrCreateAccessibilityData()
{
  if (DALI_UNLIKELY(!mAccessibilityData))
  {
    // Create only 1 times.
    mAccessibilityData = std::make_unique<AccessibilityData>(mControlImpl);
  }
  DALI_ASSERT_DEBUG(mAccessibilityData && "AccessibilityData not created!");
  return *mAccessibilityData;
}

Control::Impl::AccessibilityData* Control::Impl::GetAccessibilityData() const
{
  return mAccessibilityData.get();
}

void Control::Impl::AppendAccessibilityAttribute(const std::string& key, const std::string value)
{
  GetOrCreateAccessibilityData().AppendAccessibilityAttribute(key, value);
}

void Control::Impl::RemoveAccessibilityAttribute(const std::string& key)
{
  auto* accessibilityData = GetAccessibilityData();
  if (DALI_LIKELY(accessibilityData))
  {
    accessibilityData->RemoveAccessibilityAttribute(key);
  }
}

void Control::Impl::ClearAccessibilityAttributes()
{
  auto* accessibilityData = GetAccessibilityData();
  if (DALI_LIKELY(accessibilityData))
  {
    accessibilityData->ClearAccessibilityAttributes();
  }
}

void Control::Impl::SetAccessibilityReadingInfoType(const Dali::Accessibility::ReadingInfoTypes types)
{
  GetOrCreateAccessibilityData().SetAccessibilityReadingInfoType(types);
}

Dali::Accessibility::ReadingInfoTypes Control::Impl::GetAccessibilityReadingInfoType() const
{
  const auto* accessibilityData = GetAccessibilityData();
  if (DALI_LIKELY(accessibilityData))
  {
    return accessibilityData->GetAccessibilityReadingInfoType();
  }
  else
  {
    // Return default ReadingInfoTypes
    return AccessibilityData::GetDefaultReadingInfoTypes();
  }
}

bool Control::Impl::IsAccessibleCreated() const
{
  auto bridge = Accessibility::Bridge::GetCurrentBridge();
  return DALI_LIKELY(bridge) ? !!bridge->GetAccessible(mControlImpl.Self()) : false;
}

void Control::Impl::EnableCreateAccessible(bool enable)
{
  mAccessibleCreatable = enable;
}

bool Control::Impl::IsCreateAccessibleEnabled() const
{
  return mAccessibleCreatable;
}

std::shared_ptr<UI::DevelControl::ControlAccessible> Control::Impl::GetAccessibleObject()
{
  return GetOrCreateAccessibilityData().GetAccessibleObject();
}

bool Control::Impl::OnIdleCallback()
{
  // No visual resources to handle in dali-ui foundation
  mIdleCallback = nullptr;
  return false;
}

void Control::Impl::Process(bool postProcessor)
{
  // No visual processing in dali-ui foundation
  mProcessorRegistered = false;
}

} // namespace Internal

} // namespace UI

} // namespace Dali
