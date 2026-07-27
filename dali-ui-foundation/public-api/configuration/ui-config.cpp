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
#include <dali-ui-foundation/public-api/configuration/ui-config.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/lifecycle-controller.h>
#include <dali/devel-api/common/singleton-service.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/connection-tracker.h>
#include <utility>

// INTERNAL INCLUDES
#include <dali-ui-foundation/extension-api/ui-config-impl.h>
#include <dali-ui-foundation/integration-api/ui-theme-manager-impl.h>
#include <dali-ui-foundation/public-api/configuration/ui-theme-manager.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{

namespace Ui
{

namespace
{
const char* const UICONFIG_NOT_APPLIED_MESSAGE =
  "UiConfig has not been applied. "
  "Call UiConfig::New().Apply() in main() before the application main loop starts. "
  "UiConfig provides global settings for the entire dali-ui framework. "
  "Do NOT access UiConfig-dependent features in static/global variable initializers.";

class UiConfigRuntime : public BaseObject, public ConnectionTracker
{
public:
  UiConfigRuntime()
  {
    RegisterLifecycleControllerCallback();
  }

  void Apply(const UiConfig& config)
  {
    DALI_ASSERT_ALWAYS(!mApplied && "UiConfig::Apply() must be called only once");

    mConfig = config;
    Extension::GetImpl(mConfig).Freeze();
    mApplied = true;

    RegisterLifecycleControllerCallback();

    UiThemeManager themeManager = UiThemeManager::Get();
    GetImpl(themeManager).EnsureThemeLoader();

    Extension::GetImpl(mConfig).OnApplied();

    if(mApplicationCreated)
    {
      Extension::GetImpl(mConfig).OnApplicationCreated();
    }
  }

  bool HasCurrent() const
  {
    return mApplied;
  }

  UiConfig GetCurrent() const
  {
    DALI_ASSERT_ALWAYS(mApplied && UICONFIG_NOT_APPLIED_MESSAGE);
    return mConfig;
  }

  bool RegisterLifecycleControllerCallback()
  {
    if(!mLifecycleControllerCallbackConnected)
    {
      Dali::LifecycleController lifecycleController = Dali::LifecycleController::Get();
      if(DALI_LIKELY(lifecycleController))
      {
        mLifecycleControllerCallbackConnected = true;
        lifecycleController.PreInitSignal().Connect(this, &UiConfigRuntime::OnApplicationCreated);
      }
    }
    return mLifecycleControllerCallbackConnected;
  }

protected:
  ~UiConfigRuntime() override = default;

private:
  UiConfigRuntime(const UiConfigRuntime&)            = delete;
  UiConfigRuntime(UiConfigRuntime&&)                 = delete;
  UiConfigRuntime& operator=(const UiConfigRuntime&) = delete;
  UiConfigRuntime& operator=(UiConfigRuntime&&)      = delete;

  void OnApplicationCreated()
  {
    if(mApplicationCreated)
    {
      return;
    }

    mApplicationCreated = true;

    if(mApplied)
    {
      Extension::GetImpl(mConfig).OnApplicationCreated();
    }
  }

private:
  UiConfig mConfig;
  bool     mApplied{false};
  bool     mApplicationCreated{false};
  bool     mLifecycleControllerCallbackConnected{false};
};

BaseHandle gPreInitializedUiConfigRuntime;

BaseHandle CreateRuntime()
{
  return BaseHandle(new UiConfigRuntime());
}

UiConfigRuntime& GetRuntimeObject(BaseHandle& handle)
{
  return static_cast<UiConfigRuntime&>(handle.GetBaseObject());
}

BaseHandle GetRuntime()
{
  BaseHandle runtime;

  SingletonService singletonService(SingletonService::Get());
  if(singletonService)
  {
    BaseHandle handle = singletonService.GetSingleton(typeid(UiConfigRuntime));
    if(handle)
    {
      runtime = handle;
    }

    if(!runtime)
    {
      if(gPreInitializedUiConfigRuntime)
      {
        runtime = std::move(gPreInitializedUiConfigRuntime);
        gPreInitializedUiConfigRuntime.Reset();

        GetRuntimeObject(runtime).RegisterLifecycleControllerCallback();
      }
      else
      {
        runtime = CreateRuntime();
      }
      singletonService.Register(typeid(UiConfigRuntime), runtime);
    }
  }
  else
  {
    if(!gPreInitializedUiConfigRuntime)
    {
      gPreInitializedUiConfigRuntime = CreateRuntime();
    }
    runtime = gPreInitializedUiConfigRuntime;
  }

  return runtime;
}

} // unnamed namespace

UiConfig::UiConfig(Extension::UiConfigImpl* impl)
: BaseHandle(impl)
{
}

UiConfig UiConfig::New()
{
  Extension::UiConfigImplPtr impl = Extension::UiConfigImpl::New();

  // Pass ownership to handle
  UiConfig handle(impl.Get());

  return handle;
}

UiConfig UiConfig::DownCast(BaseHandle handle)
{
  return UiConfig(dynamic_cast<Extension::UiConfigImpl*>(handle.GetObjectPtr()));
}

void UiConfig::Apply()
{
  // Do not self-move: Apply() freezes the underlying implementation, but the handle
  // should remain usable (e.g. for getters) to match user expectations.
  BaseHandle runtime = GetRuntime();
  GetRuntimeObject(runtime).Apply(*this);
}

bool UiConfig::HasCurrent()
{
  BaseHandle runtime = GetRuntime();
  return GetRuntimeObject(runtime).HasCurrent();
}

UiConfig UiConfig::GetCurrent()
{
  BaseHandle runtime = GetRuntime();
  return GetRuntimeObject(runtime).GetCurrent();
}

void UiConfig::SetScalingFactor(float scalingFactor)
{
  Extension::GetImpl(*this).SetScalingFactor(scalingFactor);
}

float UiConfig::GetScalingFactor() const
{
  return Extension::GetImpl(*this).GetScalingFactor();
}

float UiConfig::GetDpiFactor() const
{
  return Extension::GetImpl(*this).GetDpiFactor();
}

float UiConfig::GetScaledDpiFactor() const
{
  return Extension::GetImpl(*this).GetScaledDpiFactor();
}

void UiConfig::SetDpi(int dpi)
{
  Extension::GetImpl(*this).SetDpi(dpi);
}

int UiConfig::GetDpi() const
{
  return Extension::GetImpl(*this).GetDpi();
}

void UiConfig::SetBaselineDpi(int baselineDpi)
{
  Extension::GetImpl(*this).SetBaselineDpi(baselineDpi);
}

int UiConfig::GetBaselineDpi() const
{
  return Extension::GetImpl(*this).GetBaselineDpi();
}

void UiConfig::SetKeyClickPolicy(KeyClickPolicy policy)
{
  Extension::GetImpl(*this).SetKeyClickPolicy(policy);
}

KeyClickPolicy UiConfig::GetKeyClickPolicy() const
{
  return Extension::GetImpl(*this).GetKeyClickPolicy();
}

void UiConfig::SetExecutionKeyPredicate(ExecutionKeyPredicate predicate)
{
  Extension::GetImpl(*this).SetExecutionKeyPredicate(predicate);
}

ExecutionKeyPredicate UiConfig::GetExecutionKeyPredicate() const
{
  return Extension::GetImpl(*this).GetExecutionKeyPredicate();
}

void UiConfig::SetKeyLongPressThreshold(uint32_t count)
{
  Extension::GetImpl(*this).SetKeyLongPressThreshold(count);
}

uint32_t UiConfig::GetKeyLongPressThreshold() const
{
  return Extension::GetImpl(*this).GetKeyLongPressThreshold();
}

void UiConfig::SetTapRecognizerTime(uint32_t timeMs)
{
  Extension::GetImpl(*this).SetTapRecognizerTime(timeMs);
}

uint32_t UiConfig::GetTapRecognizerTime() const
{
  return Extension::GetImpl(*this).GetTapRecognizerTime();
}

void UiConfig::SetBrokenImageUrl(BrokenImageType brokenImageType, const Dali::String& brokenImageUrl)
{
  Extension::GetImpl(*this).SetBrokenImageUrl(brokenImageType, brokenImageUrl);
}

const Dali::String& UiConfig::GetBrokenImageUrl(BrokenImageType brokenImageType) const
{
  return Extension::GetImpl(*this).GetBrokenImageUrl(brokenImageType);
}

void UiConfig::SetClearFocusOnEscapeEnabled(bool enable)
{
  Extension::GetImpl(*this).SetClearFocusOnEscapeEnabled(enable);
}

bool UiConfig::IsClearFocusOnEscapeEnabled() const
{
  return Extension::GetImpl(*this).IsClearFocusOnEscapeEnabled();
}

void UiConfig::SetClearFocusIndicationOnTouch(bool clear)
{
  Extension::GetImpl(*this).SetClearFocusIndicationOnTouch(clear);
}

bool UiConfig::IsClearFocusIndicationOnTouchEnabled() const
{
  return Extension::GetImpl(*this).IsClearFocusIndicationOnTouchEnabled();
}

void UiConfig::SetClearFocusIndicationOnHover(bool clear)
{
  Extension::GetImpl(*this).SetClearFocusIndicationOnHover(clear);
}

bool UiConfig::IsClearFocusIndicationOnHoverEnabled() const
{
  return Extension::GetImpl(*this).IsClearFocusIndicationOnHoverEnabled();
}

void UiConfig::SetDefaultFocusIndicatorEnabled(bool enabled)
{
  Extension::GetImpl(*this).SetDefaultFocusIndicatorEnabled(enabled);
}

bool UiConfig::IsDefaultFocusIndicatorEnabled() const
{
  return Extension::GetImpl(*this).IsDefaultFocusIndicatorEnabled();
}

void UiConfig::SetDefaultFontSize(float fontSize)
{
  Extension::GetImpl(*this).SetDefaultFontSize(fontSize);
}

float UiConfig::GetDefaultFontSize() const
{
  return Extension::GetImpl(*this).GetDefaultFontSize();
}

void UiConfig::SetDefaultSystemFontSizeScaleEnabled(bool enabled)
{
  Extension::GetImpl(*this).SetDefaultSystemFontSizeScaleEnabled(enabled);
}

bool UiConfig::IsDefaultSystemFontSizeScaleEnabled() const
{
  return Extension::GetImpl(*this).IsDefaultSystemFontSizeScaleEnabled();
}

void UiConfig::SetDefaultMinimumFontSizeScale(float scale)
{
  Extension::GetImpl(*this).SetDefaultMinimumFontSizeScale(scale);
}

float UiConfig::GetDefaultMinimumFontSizeScale() const
{
  return Extension::GetImpl(*this).GetDefaultMinimumFontSizeScale();
}

void UiConfig::SetDefaultMaximumFontSizeScale(float scale)
{
  Extension::GetImpl(*this).SetDefaultMaximumFontSizeScale(scale);
}

float UiConfig::GetDefaultMaximumFontSizeScale() const
{
  return Extension::GetImpl(*this).GetDefaultMaximumFontSizeScale();
}

void UiConfig::SetScaleForSystemFontSize(SystemFontSize fontSize, float scale)
{
  Extension::GetImpl(*this).SetScaleForSystemFontSize(fontSize, scale);
}

float UiConfig::GetScaleForSystemFontSize(SystemFontSize fontSize) const
{
  return Extension::GetImpl(*this).GetScaleForSystemFontSize(fontSize);
}

void UiConfig::SetDefaultTextColor(const Vector4& color)
{
  Extension::GetImpl(*this).SetDefaultTextColor(color);
}

Vector4 UiConfig::GetDefaultTextColor() const
{
  return Extension::GetImpl(*this).GetDefaultTextColor();
}

void UiConfig::SetDefaultPlaceholderTextColor(const Vector4& color)
{
  Extension::GetImpl(*this).SetDefaultPlaceholderTextColor(color);
}

Vector4 UiConfig::GetDefaultPlaceholderTextColor() const
{
  return Extension::GetImpl(*this).GetDefaultPlaceholderTextColor();
}

void UiConfig::SetShowPlaceholderTextOnFocus(bool enabled)
{
  Extension::GetImpl(*this).SetShowPlaceholderTextOnFocus(enabled);
}

bool UiConfig::IsPlaceholderTextShownOnFocus() const
{
  return Extension::GetImpl(*this).IsPlaceholderTextShownOnFocus();
}

void UiConfig::SetMarqueeSpeed(int speed)
{
  Extension::GetImpl(*this).SetMarqueeSpeed(speed);
}

int UiConfig::GetMarqueeSpeed() const
{
  return Extension::GetImpl(*this).GetMarqueeSpeed();
}

void UiConfig::SetMarqueeLoopCount(int loopCount)
{
  Extension::GetImpl(*this).SetMarqueeLoopCount(loopCount);
}

int UiConfig::GetMarqueeLoopCount() const
{
  return Extension::GetImpl(*this).GetMarqueeLoopCount();
}

void UiConfig::SetMarqueeLoopDelay(float delay)
{
  Extension::GetImpl(*this).SetMarqueeLoopDelay(delay);
}

float UiConfig::GetMarqueeLoopDelay() const
{
  return Extension::GetImpl(*this).GetMarqueeLoopDelay();
}

void UiConfig::SetMarqueeGap(float gap)
{
  Extension::GetImpl(*this).SetMarqueeGap(gap);
}

float UiConfig::GetMarqueeGap() const
{
  return Extension::GetImpl(*this).GetMarqueeGap();
}

void UiConfig::SetMarqueeStopMode(Text::MarqueeStopMode stopMode)
{
  Extension::GetImpl(*this).SetMarqueeStopMode(stopMode);
}

Text::MarqueeStopMode UiConfig::GetMarqueeStopMode() const
{
  return Extension::GetImpl(*this).GetMarqueeStopMode();
}

void UiConfig::SetMarqueeOrientation(Text::MarqueeOrientation orientation)
{
  Extension::GetImpl(*this).SetMarqueeOrientation(orientation);
}

Text::MarqueeOrientation UiConfig::GetMarqueeOrientation() const
{
  return Extension::GetImpl(*this).GetMarqueeOrientation();
}

void UiConfig::SetTextLayoutDirectionMode(Text::LayoutDirectionMode mode)
{
  Extension::GetImpl(*this).SetTextLayoutDirectionMode(mode);
}

Text::LayoutDirectionMode UiConfig::GetTextLayoutDirectionMode() const
{
  return Extension::GetImpl(*this).GetTextLayoutDirectionMode();
}

void UiConfig::SetLabelAsyncRendering(bool asyncRendering)
{
  Extension::GetImpl(*this).SetLabelAsyncRendering(asyncRendering);
}

bool UiConfig::IsLabelAsyncRendering() const
{
  return Extension::GetImpl(*this).IsLabelAsyncRendering();
}

void UiConfig::SetViewInitializer(ViewInitializer initializer)
{
  Extension::GetImpl(*this).SetViewInitializer(initializer);
}

ViewInitializer UiConfig::GetViewInitializer() const
{
  return Extension::GetImpl(*this).GetViewInitializer();
}

void UiConfig::SetDefaultStateEffectForInteractive(StateEffect effect)
{
  Extension::GetImpl(*this).SetDefaultStateEffectForInteractive(effect);
}

StateEffect UiConfig::GetDefaultStateEffectForInteractive() const
{
  return Extension::GetImpl(*this).GetDefaultStateEffectForInteractive();
}

void UiConfig::SetAmbiguousPressDelay(uint32_t timeMs)
{
  Extension::GetImpl(*this).SetAmbiguousPressDelay(timeMs);
}

uint32_t UiConfig::GetAmbiguousPressDelay() const
{
  return Extension::GetImpl(*this).GetAmbiguousPressDelay();
}

void UiConfig::SetAmbiguousPressDuration(uint32_t timeMs)
{
  Extension::GetImpl(*this).SetAmbiguousPressDuration(timeMs);
}

uint32_t UiConfig::GetAmbiguousPressDuration() const
{
  return Extension::GetImpl(*this).GetAmbiguousPressDuration();
}

void UiConfig::ResetStyleSheet(UiStyleSheet styleSheet)
{
  Extension::GetImpl(*this).ResetStyleSheet(styleSheet);
}

UiStyleSheet UiConfig::StyleSheet() const
{
  return Extension::GetImpl(*this).StyleSheet();
}

void UiConfig::DefaultViewInitializer(View view)
{
}

} // namespace Ui
} // namespace Dali
