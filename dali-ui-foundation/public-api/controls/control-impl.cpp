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
#include <dali-ui-foundation/public-api/controls/control-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/size-negotiation/relayout-container.h>
#include <cstring> // for strcmp
#include <limits>
#include <stack>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/controls/control-devel.h>
#include <dali-ui-foundation/internal/controls/control/control-data-impl.h>
#include <dali-ui-foundation/public-api/controls/control.h>

// TODO: These dali-toolkit includes should be replaced when their code is moved to dali-ui
#include <dali-toolkit/devel-api/focus-manager/keyinput-focus-manager.h>
#include <dali-toolkit/public-api/focus-manager/keyboard-focus-manager.h>

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

/**
 * @brief Register external accessible getter function only 1 times per each programs.
 * @note We could call this API only for main thread. So use static bool flag is enough than std::once_flag
 */
void RegisterControlAccessibleGetter()
{
  static bool onceFlag = false;
  if (DALI_UNLIKELY(!onceFlag))
  {
    onceFlag = true;
    Accessibility::Accessible::RegisterExternalAccessibleGetter(
        [](Dali::Actor actor) -> std::pair<std::shared_ptr<Accessibility::Accessible>, bool>
        {
          auto control = UI::Control::DownCast(actor);
          if (!control)
          {
            return {nullptr, true};
          }

          if (UI::DevelControl::IsCreateAccessibleEnabled(control))
          {
            auto& controlImpl = UI::Internal::GetImplementation(control);
            return {std::shared_ptr<DevelControl::ControlAccessible>(controlImpl.CreateAccessibleObject()), true};
          }

          return {nullptr, false};
        });
  }
}
} // unnamed namespace

UI::Control Control::New()
{
  return New(ControlBehaviour::CONTROL_BEHAVIOUR_DEFAULT);
}

UI::Control Control::New(ControlBehaviour additionalBehaviour)
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr<Control> controlImpl = new Control(ControlBehaviour(CONTROL_BEHAVIOUR_DEFAULT | additionalBehaviour));

  // Pass ownership to handle
  UI::Control handle(*controlImpl);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  controlImpl->Initialize();

  return handle;
}

void Control::SetStyleName(const std::string& styleName)
{
  if (styleName != mImpl->mStyleName)
  {
    mImpl->mStyleName = styleName;
  }
}

const std::string& Control::GetStyleName() const
{
  return mImpl->mStyleName;
}

std::shared_ptr<UI::DevelControl::ControlAccessible> Control::GetAccessibleObject()
{
  return mImpl->GetAccessibleObject();
}

void Control::EnableGestureDetection(GestureType::Value type)
{
  if ((type & GestureType::PINCH) && !mImpl->mPinchGestureDetector)
  {
    mImpl->mPinchGestureDetector = PinchGestureDetector::New();
    mImpl->mPinchGestureDetector.DetectedSignal().Connect(mImpl, &Impl::PinchDetected);
    mImpl->mPinchGestureDetector.Attach(Self());
  }

  if ((type & GestureType::PAN) && !mImpl->mPanGestureDetector)
  {
    mImpl->mPanGestureDetector = PanGestureDetector::New();
    mImpl->mPanGestureDetector.SetMaximumTouchesRequired(2);
    mImpl->mPanGestureDetector.DetectedSignal().Connect(mImpl, &Impl::PanDetected);
    mImpl->mPanGestureDetector.Attach(Self());
  }

  if ((type & GestureType::TAP) && !mImpl->mTapGestureDetector)
  {
    mImpl->mTapGestureDetector = TapGestureDetector::New();
    mImpl->mTapGestureDetector.DetectedSignal().Connect(mImpl, &Impl::TapDetected);
    mImpl->mTapGestureDetector.Attach(Self());
  }

  if ((type & GestureType::LONG_PRESS) && !mImpl->mLongPressGestureDetector)
  {
    mImpl->mLongPressGestureDetector = LongPressGestureDetector::New();
    mImpl->mLongPressGestureDetector.DetectedSignal().Connect(mImpl, &Impl::LongPressDetected);
    mImpl->mLongPressGestureDetector.Attach(Self());
  }
}

void Control::DisableGestureDetection(GestureType::Value type)
{
  if ((type & GestureType::PINCH) && mImpl->mPinchGestureDetector)
  {
    mImpl->mPinchGestureDetector.Detach(Self());
    mImpl->mPinchGestureDetector.Reset();
  }

  if ((type & GestureType::PAN) && mImpl->mPanGestureDetector)
  {
    mImpl->mPanGestureDetector.Detach(Self());
    mImpl->mPanGestureDetector.Reset();
  }

  if ((type & GestureType::TAP) && mImpl->mTapGestureDetector)
  {
    mImpl->mTapGestureDetector.Detach(Self());
    mImpl->mTapGestureDetector.Reset();
  }

  if ((type & GestureType::LONG_PRESS) && mImpl->mLongPressGestureDetector)
  {
    mImpl->mLongPressGestureDetector.Detach(Self());
    mImpl->mLongPressGestureDetector.Reset();
  }
}

PinchGestureDetector Control::GetPinchGestureDetector() const
{
  return mImpl->mPinchGestureDetector;
}

PanGestureDetector Control::GetPanGestureDetector() const
{
  return mImpl->mPanGestureDetector;
}

TapGestureDetector Control::GetTapGestureDetector() const
{
  return mImpl->mTapGestureDetector;
}

LongPressGestureDetector Control::GetLongPressGestureDetector() const
{
  return mImpl->mLongPressGestureDetector;
}

void Control::SetKeyboardNavigationSupport(bool isSupported)
{
  mImpl->mIsKeyboardNavigationSupported = isSupported;
}

bool Control::IsKeyboardNavigationSupported()
{
  return mImpl->mIsKeyboardNavigationSupported;
}

void Control::SetKeyInputFocus()
{
  if (Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    Toolkit::KeyInputFocusManager::Get().SetFocus(Toolkit::Control::DownCast(Self()));
  }
}

bool Control::HasKeyInputFocus()
{
  bool result = false;
  if (Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    Toolkit::Control control = Toolkit::KeyInputFocusManager::Get().GetCurrentFocusControl();
    if (Self() == control)
    {
      result = true;
    }
  }
  return result;
}

void Control::ClearKeyInputFocus()
{
  if (Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    Toolkit::KeyInputFocusManager::Get().RemoveFocus(Toolkit::Control::DownCast(Self()));
  }
}

void Control::SetAsKeyboardFocusGroup(bool isFocusGroup)
{
  mImpl->mIsKeyboardFocusGroup = isFocusGroup;

  // The following line will be removed when the deprecated API in KeyboardFocusManager is deleted
  Toolkit::KeyboardFocusManager::Get().SetAsFocusGroup(Self(), isFocusGroup);
}

bool Control::IsKeyboardFocusGroup()
{
  return Toolkit::KeyboardFocusManager::Get().IsFocusGroup(Self());
}

void Control::KeyboardEnter()
{
  // Inform deriving classes
  OnKeyboardEnter();
}

bool Control::OnAccessibilityActivated()
{
  if (Toolkit::KeyboardFocusManager::Get().SetCurrentFocusActor(Self()))
  {
    return OnKeyboardEnter();
  }
  return false;
}

bool Control::OnKeyboardEnter()
{
  return false; // Keyboard enter is not handled by default
}

bool Control::OnAccessibilityPan(PanGesture gesture)
{
  return false; // Accessibility pan gesture is not handled by default
}

bool Control::OnAccessibilityValueChange(bool isIncrease)
{
  return false; // Accessibility value change action is not handled by default
}

bool Control::OnAccessibilityZoom()
{
  return false; // Accessibility zoom action is not handled by default
}

DevelControl::ControlAccessible* Control::CreateAccessibleObject()
{
  return new DevelControl::ControlAccessible(Self());
}

Actor Control::GetNextKeyboardFocusableActor(Actor currentFocusedActor, UI::Control::KeyboardFocus::Direction direction,
                                             bool loopEnabled)
{
  return Actor();
}

void Control::OnKeyboardFocusChangeCommitted(Actor commitedFocusableActor)
{
}

UI::Control::KeyEventSignalType& Control::KeyEventSignal()
{
  return mImpl->mKeyEventSignal;
}

UI::Control::KeyInputFocusSignalType& Control::KeyInputFocusGainedSignal()
{
  return mImpl->mKeyInputFocusGainedSignal;
}

UI::Control::KeyInputFocusSignalType& Control::KeyInputFocusLostSignal()
{
  return mImpl->mKeyInputFocusLostSignal;
}

bool Control::EmitKeyEventSignal(const KeyEvent& event)
{
  // Guard against destruction during signal emission
  Dali::UI::Control handle(GetOwner());

  bool consumed = false;

  consumed = mImpl->FilterKeyEvent(event);

  // signals are allocated dynamically when someone connects
  if (!consumed && !mImpl->mKeyEventSignal.Empty())
  {
    consumed = mImpl->mKeyEventSignal.Emit(handle, event);
  }

  if (!consumed)
  {
    // Notification for derived classes
    consumed = OnKeyEvent(event);
  }

  return consumed;
}

Control::Control(ControlBehaviour behaviourFlags)
  : CustomActorImpl(static_cast<ActorFlags>(behaviourFlags)),
    mImpl(new Impl(*this))
{
  mImpl->mFlags = behaviourFlags;
}

Control::~Control()
{
  delete mImpl;
}

void Control::Initialize()
{
  RegisterControlAccessibleGetter();

  // Call deriving classes so initialised before styling is applied to them.
  OnInitialize();

  if (mImpl->mFlags & REQUIRES_KEYBOARD_NAVIGATION_SUPPORT)
  {
    SetKeyboardNavigationSupport(true);
  }
}

void Control::OnInitialize()
{
}

void Control::OnPinch(const PinchGesture& pinch)
{
  if (!(mImpl->mStartingPinchScale))
  {
    // lazy allocate
    mImpl->mStartingPinchScale = new Vector3;
  }

  if (pinch.GetState() == GestureState::STARTED)
  {
    *(mImpl->mStartingPinchScale) = Self().GetCurrentProperty<Vector3>(Actor::Property::SCALE);
  }

  Self().SetProperty(Actor::Property::SCALE, *(mImpl->mStartingPinchScale) * pinch.GetScale());
}

void Control::OnPan(const PanGesture& pan)
{
}

void Control::OnTap(const TapGesture& tap)
{
}

void Control::OnLongPress(const LongPressGesture& longPress)
{
}

void Control::EmitKeyInputFocusSignal(bool focusGained)
{
  Dali::UI::Control handle(GetOwner());

  if (Accessibility::IsUp())
  {
    auto accessible = GetAccessibleObject();
    if (DALI_LIKELY(accessible))
    {
      accessible->EmitFocused(focusGained);
      auto parent = dynamic_cast<Dali::Accessibility::ActorAccessible*>(accessible->GetParent());
      if (parent && !accessible->GetStates()[Dali::Accessibility::State::MANAGES_DESCENDANTS])
      {
        parent->EmitActiveDescendantChanged(accessible.get());
      }
    }
  }

  if (focusGained)
  {
    // signals are allocated dynamically when someone connects
    if (!mImpl->mKeyInputFocusGainedSignal.Empty())
    {
      mImpl->mKeyInputFocusGainedSignal.Emit(handle);
    }
  }
  else
  {
    // signals are allocated dynamically when someone connects
    if (!mImpl->mKeyInputFocusLostSignal.Empty())
    {
      mImpl->mKeyInputFocusLostSignal.Emit(handle);
    }
  }
}

void Control::OnSceneConnection(int depth)
{
  mImpl->OnSceneConnection();
}

void Control::OnSceneDisconnection()
{
  mImpl->OnSceneDisconnection();
}

void Control::OnKeyInputFocusGained()
{
  EmitKeyInputFocusSignal(true);
}

void Control::OnKeyInputFocusLost()
{
  EmitKeyInputFocusSignal(false);
}

void Control::OnChildAdd(Actor& child)
{
}

void Control::OnChildRemove(Actor& child)
{
}

void Control::OnPropertySet(Property::Index index, const Property::Value& propertyValue)
{
  switch (index)
  {
    case DevelActor::Property::USER_INTERACTION_ENABLED:
    {
      const bool enabled = propertyValue.Get<bool>();
      if (!enabled && Self() == Dali::Toolkit::KeyboardFocusManager::Get().GetCurrentFocusActor())
      {
        Dali::Toolkit::KeyboardFocusManager::Get().ClearFocus();
      }
      break;
    }
  }
}

void Control::OnSizeSet(const Vector3& targetSize)
{
  mImpl->mSize = Vector2(targetSize);
}

void Control::OnSizeAnimation(Animation& animation, const Vector3& targetSize)
{
}

bool Control::OnKeyEvent(const KeyEvent& event)
{
  return false; // Do not consume
}

void Control::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  // When set the padding or margin on the control, child should be resized and repositioned.
  if ((mImpl->mPadding.start != 0) || (mImpl->mPadding.end != 0) || (mImpl->mPadding.top != 0) ||
      (mImpl->mPadding.bottom != 0) || (mImpl->mMargin.start != 0) || (mImpl->mMargin.end != 0) ||
      (mImpl->mMargin.top != 0) || (mImpl->mMargin.bottom != 0))
  {
    for (unsigned int i = 0, numChildren = Self().GetChildCount(); i < numChildren; ++i)
    {
      Actor child = Self().GetChildAt(i);
      Vector2 newChildSize(size);

      Extents padding = mImpl->mPadding;

      Dali::CustomActor ownerActor(GetOwner());
      Dali::LayoutDirection::Type layoutDirection = static_cast<Dali::LayoutDirection::Type>(
          ownerActor.GetProperty(Dali::Actor::Property::LAYOUT_DIRECTION).Get<int>());

      if (Dali::LayoutDirection::RIGHT_TO_LEFT == layoutDirection)
      {
        std::swap(padding.start, padding.end);
      }

      newChildSize.width = size.width - (padding.start + padding.end);
      newChildSize.height = size.height - (padding.top + padding.bottom);

      // Cannot use childs Position property as it can already have padding and margin applied on it,
      // so we end up cumulatively applying them over and over again.
      Vector2 childOffset(0.f, 0.f);
      childOffset.x += (mImpl->mMargin.start + padding.start);
      childOffset.y += (mImpl->mMargin.top + padding.top);

      child.SetProperty(Actor::Property::POSITION, Vector2(childOffset.x, childOffset.y));

      container.Add(child, newChildSize);
    }
  }

  if (Accessibility::IsUp())
  {
    auto accessible = GetAccessibleObject();
    if (DALI_LIKELY(accessible))
    {
      auto highlightFrame = accessible->GetHighlightActor();
      if (accessible->GetCurrentlyHighlightedActor() == this->Self() &&
          highlightFrame.GetProperty<Vector3>(Dali::Actor::Property::SIZE).GetVectorXY() != size)
      {
        // TODO : Need to consider how we can reduce this cost
        highlightFrame.SetProperty(Actor::Property::SIZE, size);
        container.Add(highlightFrame, size);
      }
    }
  }
}

void Control::OnSetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension)
{
}

Vector3 Control::GetNaturalSize()
{
  return Vector3::ZERO;
}

float Control::CalculateChildSize(const Dali::Actor& child, Dimension::Type dimension)
{
  return CalculateChildSizeBase(child, dimension);
}

float Control::GetHeightForWidth(float width)
{
  return GetHeightForWidthBase(width);
}

float Control::GetWidthForHeight(float height)
{
  return GetWidthForHeightBase(height);
}

bool Control::RelayoutDependentOnChildren(Dimension::Type dimension)
{
  return RelayoutDependentOnChildrenBase(dimension);
}

void Control::OnCalculateRelayoutSize(Dimension::Type dimension)
{
}

void Control::OnLayoutNegotiated(float size, Dimension::Type dimension)
{
}

void Control::GetOffScreenRenderTasks(Dali::Vector<Dali::RenderTask>& tasks, bool isForward)
{
  // No offscreen rendering in dali-ui foundation controls
}

void Control::SignalConnected(SlotObserver* slotObserver, CallbackBase* callback)
{
  mImpl->SignalConnected(slotObserver, callback);
}

void Control::SignalDisconnected(SlotObserver* slotObserver, CallbackBase* callback)
{
  mImpl->SignalDisconnected(slotObserver, callback);
}

Control& GetImplementation(Dali::UI::Control& handle)
{
  CustomActorImpl& customInterface = handle.GetImplementation();
  // downcast to control
  Control& impl = dynamic_cast<Internal::Control&>(customInterface);
  return impl;
}

const Control& GetImplementation(const Dali::UI::Control& handle)
{
  const CustomActorImpl& customInterface = handle.GetImplementation();
  // downcast to control
  const Control& impl = dynamic_cast<const Internal::Control&>(customInterface);
  return impl;
}

} // namespace Internal

} // namespace UI

} // namespace Dali
