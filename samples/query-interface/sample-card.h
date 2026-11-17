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
// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/interfaces/text-content.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include "interfaces/resettable.h"

namespace QueryInterfaceSample
{
/**
 * @brief Optional counters used to verify interface dispatch and destruction.
 */
struct CallCounts
{
  unsigned sets{0};      ///< Calls to SetText.
  unsigned gets{0};      ///< Calls to GetText.
  unsigned resets{0};    ///< Calls to ResetContent.
  unsigned destroyed{0}; ///< Completed implementation destructions.
};

/**
 * @brief Implements two independent interfaces on an app-owned component.
 *
 * The build generates its interface map. Direct construction needs no
 * ViewRegistration, TypeRegistry lookup or factory registration.
 */
class SampleCardImpl : public Dali::Ui::ViewImpl,
                       public Dali::Ui::ITextContent,
                       public IResettable
{
public:
  /**
   * @brief Constructs the implementation with optional observation counters.
   *
   * @param[in] counts Borrowed counters that must outlive the implementation,
   * or nullptr to disable counting
   */
  explicit SampleCardImpl(CallCounts* counts = nullptr)
  : mCounts(counts)
  {
  }

  /**
   * @brief Records destruction when the final owning reference is released.
   */
  ~SampleCardImpl() override
  {
    if(mCounts) ++mCounts->destroyed;
  }

  /**
   * @copydoc Dali::Ui::ITextContent::SetText
   */
  void SetText(const Dali::String& text) override
  {
    if(mCounts) ++mCounts->sets;
    mLabel.SetText(text);
  }

  /**
   * @copydoc Dali::Ui::ITextContent::GetText
   */
  Dali::String GetText() const override
  {
    if(mCounts) ++mCounts->gets;
    return mLabel.GetText();
  }

  /**
   * @copydoc IResettable::ResetContent
   */
  void ResetContent() override
  {
    if(mCounts) ++mCounts->resets;
    mLabel.SetText("Custom component: reset");
  }

protected:
  /**
   * @copydoc Dali::Ui::ViewImpl::OnInitialize
   */
  void OnInitialize() override
  {
    Dali::Ui::ViewImpl::OnInitialize();
    mLabel = Dali::Ui::Label::New("Custom component: ready");
    mLabel.SetRequestedWidth(440.0f);
    mLabel.SetRequestedHeight(60.0f);
    Self().Add(mLabel);
  }

private:
  Dali::Ui::Label mLabel;
  CallCounts*     mCounts;
};

/**
 * @brief Verifies that automatically inherited providers resolve the same
 * interface subobject as their base implementation.
 */
class DerivedCardImpl : public SampleCardImpl
{
public:
  using SampleCardImpl::SampleCardImpl;
};

/**
 * @brief App handle retaining SampleCardImpl through DALi's normal ownership.
 */
class SampleCard : public Dali::Ui::View
{
public:
  /**
   * @brief Creates an empty handle.
   */
  SampleCard() = default;

  /**
   * @brief Creates and initializes a component directly.
   *
   * @param[in] counts Optional borrowed counters; must outlive all component owners
   * @return A handle retaining the initialized component
   */
  static SampleCard New(CallCounts* counts = nullptr)
  {
    Dali::IntrusivePtr<SampleCardImpl> impl(new SampleCardImpl(counts));
    SampleCard                         card(*impl);
    impl->Initialize();
    return card;
  }

private:
  explicit SampleCard(SampleCardImpl& impl)
  : Dali::Ui::View(impl)
  {
  }
};

} //namespace QueryInterfaceSample
