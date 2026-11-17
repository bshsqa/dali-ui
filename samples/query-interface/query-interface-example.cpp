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
#include <cmath>
#include <cstdio>
#include <cstring>
#include <stdexcept>

// INTERNAL INCLUDES
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali/devel-api/adaptor-framework/application.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/object/query-interface.h>
#include "component-checks.h"
#include "sample-card.h"

using namespace Dali;
using namespace Dali::Ui;
using namespace QueryInterfaceSample;

class QueryInterfaceExample : public ConnectionTracker
{
public:
  QueryInterfaceExample(Application app, bool test)
  : mApp(app),
    mSelfTest(test)
  {
    mApp.InitSignal().Connect(this, &QueryInterfaceExample::Create);
  }
  int Result() const
  {
    return mResult;
  }

private:
  void Create(Application app)
  {
    auto window = app.GetWindow();
    window.SetBackgroundColor(Color::WHITE);
    window.KeyEventSignal().Connect(this, &QueryInterfaceExample::OnKey);
    auto heading = Label::New("QueryInterface / DALi");
    heading.SetRequestedWidth(600.0f);
    heading.SetRequestedHeight(60.0f);
    heading.SetRequestedX(24.0f);
    heading.SetRequestedY(24.0f);
    window.Add(heading);

    mStatus = Label::New("SPACE: Play   P: Pause   S: Stop\nT: SetText   R: ResetContent   ESC: Exit");
    mStatus.SetMultiLine(true);
    mStatus.SetRequestedWidth(620.0f);
    mStatus.SetRequestedHeight(110.0f);
    mStatus.SetRequestedX(24.0f);
    mStatus.SetRequestedY(90.0f);
    window.Add(mStatus);
    mStatusText = QueryInterface<ITextContent>(mStatus);

    mCard = SampleCard::New();
    mCard.SetRequestedWidth(440.0f);
    mCard.SetRequestedHeight(80.0f);
    mCard.SetRequestedX(24.0f);
    mCard.SetRequestedY(230.0f);
    mCard.SetBackgroundColor(UiColor(0xD8E9FF));
    window.Add(mCard);
    mText      = QueryInterface<ITextContent>(mCard);
    mReset     = QueryInterface<IResettable>(mCard);
    mAnimation = Animation::New(1.0f);
    mAnimation.AnimateTo(Property(mCard, Actor::Property::OPACITY), 0.3f);
    mAnimation.SetLoopCount(Animation::INFINITE_LOOP);
    mAnimation.SetLoopingMode(Animation::AUTO_REVERSE);
    mPlayback = QueryInterface<IPlayback>(mAnimation);

    if(mSelfTest)
    {
      try
      {
        RunComponentChecks();
        mProbe = Actor::New();
        mProbe.SetProperty(Actor::Property::POSITION_X, 0.0f);
        window.Add(mProbe);
        mAnimation = Animation::New(2.0f);
        mAnimation.AnimateTo(Property(mProbe, Actor::Property::POSITION_X), 200.0f);
        mPlayback = QueryInterface<IPlayback>(mAnimation);
        mPlayback->Play();
        mTimer = Timer::New(150);
        mTimer.TickSignal().Connect(this, &QueryInterfaceExample::VerifyFrames);
        mTimer.Start();
      }
      catch(const std::exception& error)
      {
        Fail(error.what());
      }
    }
  }
  void OnKey(Window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::DOWN) return;
    const auto& key = event.GetKeyString();
    if(key == "Escape")
      mApp.Quit();
    else if(key == "space")
    {
      mPlayback->Play();
      mStatusText->SetText("IPlayback::Play() - running\nP: Pause   S: Stop   T: SetText   R: Reset");
    }
    else if(key == "p" || key == "P")
    {
      mPlayback->Pause();
      mStatusText->SetText("IPlayback::Pause() - paused\nSPACE: Resume   S: Stop");
    }
    else if(key == "s" || key == "S")
    {
      mPlayback->Stop();
      mStatusText->SetText("IPlayback::Stop() - stopped\nSPACE: Play");
    }
    else if(key == "t" || key == "T")
      mText->SetText("ITextContent: changed by the app");
    else if(key == "r" || key == "R")
      mReset->ResetContent();
  }
  bool VerifyFrames()
  {
    try
    {
      const float value = mProbe.GetCurrentProperty<float>(Actor::Property::POSITION_X);
      ++mFramePhase;
      if(mFramePhase == 2)
      {
        if(value <= 0.0f) throw std::runtime_error("Play did not advance animated property");
        std::printf("PASS rendered Play advances property: %.3f\n", value);
        mPlayback->Pause();
      }
      else if(mFramePhase == 3)
        mPausedPosition = value;
      else if(mFramePhase == 4)
      {
        if(std::fabs(value - mPausedPosition) > 0.01f) throw std::runtime_error("Pause did not freeze animated property");
        std::printf("PASS rendered Pause freezes property: %.3f\n", value);
        mPlayback->Play();
      }
      else if(mFramePhase == 6)
      {
        if(value <= mPausedPosition + 1.0f) throw std::runtime_error("Resume did not advance animated property");
        std::printf("PASS rendered resume advances property: %.3f\n", value);
        mPlayback->Stop();
      }
      else if(mFramePhase == 7)
        mStoppedPosition = value;
      else if(mFramePhase == 8)
      {
        if(std::fabs(value - mStoppedPosition) > 0.01f || mAnimation.GetState() != Animation::STOPPED)
          throw std::runtime_error("Stop did not stop animated property");
        std::printf("PASS rendered Stop freezes property: %.3f\nSELF-TEST PASSED\n", value);
        mResult = 0;
        mApp.Quit();
        return false;
      }
    }
    catch(const std::exception& error)
    {
      Fail(error.what());
      return false;
    }
    return true;
  }
  void Fail(const char* message)
  {
    std::fprintf(stderr, "SELF-TEST FAILED: %s\n", message);
    mResult = 1;
    if(mTimer)
    {
      mApp.Quit();
    }
    else
    {
      // A failure during Init must quit after the native loop has started.
      mTimer = Timer::New(1);
      mTimer.TickSignal().Connect(this, &QueryInterfaceExample::QuitAfterFailure);
      mTimer.Start();
    }
  }
  bool QuitAfterFailure()
  {
    mApp.Quit();
    return false;
  }
  Application                mApp;
  bool                       mSelfTest;
  int                        mResult{1};
  Label                      mStatus;
  SampleCard                 mCard;
  InterfacePtr<ITextContent> mStatusText, mText;
  InterfacePtr<IResettable>  mReset;
  Animation                  mAnimation;
  InterfacePtr<IPlayback>    mPlayback;
  Timer                      mTimer;
  Actor                      mProbe;
  unsigned                   mFramePhase{0};
  float                      mPausedPosition{0.0f}, mStoppedPosition{0.0f};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  bool selfTest = false;
  int  args     = 1;
  for(int i = 1; i < argc; ++i)
  {
    if(std::strcmp(argv[i], "--self-test") == 0)
      selfTest = true;
    else
      argv[args++] = argv[i];
  }
  argc             = args;
  argv[args]       = nullptr;
  auto application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  QueryInterfaceExample example(application, selfTest);
  application.MainLoop();
  return selfTest ? example.Result() : 0;
}
