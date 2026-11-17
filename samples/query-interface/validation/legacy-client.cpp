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
#include <cstdio>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali/devel-api/adaptor-framework/application.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/dali-core.h>

using namespace Dali;
using namespace Dali::Ui;

class LegacyObject : public BaseObject
{
public:
  explicit LegacyObject(bool& destroyed)
  : mDestroyed(destroyed)
  {
  }
  ~LegacyObject() override
  {
    mDestroyed = true;
  }

private:
  bool& mDestroyed;
};

class LegacyViewImpl : public ViewImpl
{
public:
  LegacyViewImpl(bool& initialized, bool& destroyed)
  : mInitialized(initialized),
    mDestroyed(destroyed)
  {
  }
  ~LegacyViewImpl() override
  {
    mDestroyed = true;
  }

protected:
  void OnInitialize() override
  {
    ViewImpl::OnInitialize();
    mInitialized = true;
  }

private:
  bool& mInitialized;
  bool& mDestroyed;
};

class LegacyClient : public ConnectionTracker
{
public:
  explicit LegacyClient(Application app)
  : mApp(app)
  {
    mApp.InitSignal().Connect(this, &LegacyClient::Run);
  }
  void Run(Application)
  {
    bool           objectDestroyed = false;
    BaseHandle     object(new LegacyObject(objectDestroyed));
    WeakHandleBase weakObject(object);
    BaseHandle     copy(object);
    object.Reset();
    const bool retained = !objectDestroyed;
    copy.Reset();

    bool initialized = false, viewDestroyed = false;
    {
      IntrusivePtr<LegacyViewImpl> impl(new LegacyViewImpl(initialized, viewDestroyed));
      View                         view(*impl);
      impl->Initialize();
      view.SetRequestedWidth(123.0f);
      auto label = Label::New("old client");
      label.SetText("existing Label API");
      view.Add(label);
      auto animation = Animation::New(1.0f);
      animation.Play();
      animation.Pause();
      animation.Stop();
      mValid = label.GetText() == String("existing Label API") && animation.GetState() == Animation::STOPPED;
    }
    mValid = mValid && retained && objectDestroyed && !weakObject.GetBaseHandle() && initialized && viewDestroyed;
    std::printf("LAYOUT BaseHandle=%zu BaseObject=%zu CustomActorImpl=%zu ViewImpl=%zu View=%zu\n",
                sizeof(BaseHandle), sizeof(BaseObject), sizeof(CustomActorImpl), sizeof(ViewImpl), sizeof(View));
    std::printf("LEGACY ABI CLIENT %s\n", mValid ? "PASSED" : "FAILED");
    mTimer = Timer::New(100);
    mTimer.TickSignal().Connect(this, &LegacyClient::Quit);
    mTimer.Start();
  }
  bool Valid() const
  {
    return mValid;
  }

private:
  bool Quit()
  {
    mApp.Quit();
    return false;
  }
  Application mApp;
  Timer       mTimer;
  bool        mValid{false};
};

int main(int argc, char** argv)
{
  auto app = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  LegacyClient client(app);
  app.MainLoop();
  return client.Valid() ? 0 : 1;
}
