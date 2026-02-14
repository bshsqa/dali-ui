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

#include <stdlib.h>
#include <iostream>
#include <dali.h>
#include <dali-test-suite-utils.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/input/keyinput-focus-manager.h>
#include <dali-ui-foundation/public-api/view.h>

using namespace Dali;
using namespace Dali::UI;

void utc_dali_key_input_focus_manager_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_key_input_focus_manager_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliKeyInputFocusManagerGetP(void)
{
  TestApplication application;
  KeyInputFocusManager manager = KeyInputFocusManager::Get();
  DALI_TEST_CHECK(manager);
  END_TEST;
}

int UtcDaliKeyInputFocusManagerSetFocusP(void)
{
  TestApplication application;
  KeyInputFocusManager manager = KeyInputFocusManager::Get();

  View view = View::New();
  application.GetScene().Add(view);

  manager.SetFocus(view);
  DALI_TEST_CHECK(manager.GetCurrentFocusView() == view);

  END_TEST;
}

int UtcDaliKeyInputFocusManagerRemoveFocusP(void)
{
  TestApplication application;
  KeyInputFocusManager manager = KeyInputFocusManager::Get();

  View view = View::New();
  application.GetScene().Add(view);

  manager.SetFocus(view);
  DALI_TEST_CHECK(manager.GetCurrentFocusView() == view);

  manager.RemoveFocus(view);
  DALI_TEST_CHECK(!manager.GetCurrentFocusView());

  END_TEST;
}

static bool gKeyEventSignalCalled = false;
static void OnKeyEvent(View view, const KeyEvent& event)
{
  gKeyEventSignalCalled = true;
}

int UtcDaliKeyInputFocusManagerKeySignalP(void)
{
  TestApplication application;
  KeyInputFocusManager manager = KeyInputFocusManager::Get();

  View view = View::New();
  application.GetScene().Add(view);
  view.KeyEventSignal().Connect(&OnKeyEvent);

  manager.SetFocus(view);

  gKeyEventSignalCalled = false;
  // Simulate key event
  Integration::KeyEvent event("a", "", "a", 0, 0, 0, Integration::KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(event);

  DALI_TEST_CHECK(gKeyEventSignalCalled);

  END_TEST;
}

static bool gFocusGainedCalled = false;
static void OnFocusGained(View view)
{
  gFocusGainedCalled = true;
}

static bool gFocusLostCalled = false;
static void OnFocusLost(View view)
{
  gFocusLostCalled = true;
}

int UtcDaliKeyInputFocusManagerFocusSignalsP(void)
{
  TestApplication application;
  KeyInputFocusManager manager = KeyInputFocusManager::Get();

  View view1 = View::New();
  view1.KeyInputFocusGainedSignal().Connect(&OnFocusGained);
  view1.KeyInputFocusLostSignal().Connect(&OnFocusLost);
  application.GetScene().Add(view1);

  gFocusGainedCalled = false;
  manager.SetFocus(view1);
  DALI_TEST_CHECK(gFocusGainedCalled);
  DALI_TEST_CHECK(manager.GetCurrentFocusView() == view1);

  gFocusLostCalled = false;
  View view2 = View::New();
  application.GetScene().Add(view2);
  manager.SetFocus(view2);

  DALI_TEST_CHECK(gFocusLostCalled);
  DALI_TEST_CHECK(manager.GetCurrentFocusView() == view2);

  END_TEST;
}
