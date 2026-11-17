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
#include <dali-ui-test-suite-utils.h>
#include <dali/public-api/object/query-interface.h>
#include <dali/public-api/object/weak-handle.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_ui_query_interface_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_ui_query_interface_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliUiQueryInterfaceLabelDispatch(void)
{
  UiTestApplication application;
  auto              label = Label::New("before");
  auto              text  = QueryInterface<ITextContent>(label);
  DALI_TEST_CHECK(text);
  text->SetText("interface UTF-8 한글");
  DALI_TEST_CHECK(label.GetText() == String("interface UTF-8 한글"));
  label.SetText("existing API");
  DALI_TEST_CHECK(text->GetText() == String("existing API"));
  DALI_TEST_CHECK(QueryInterface<const ITextContent>(label)->GetText() == label.GetText());
  text->SetText("");
  DALI_TEST_CHECK(text->GetText().Empty());
  DALI_TEST_CHECK(!QueryInterface<ITextContent>(View::New()));
  END_TEST;
}

int UtcDaliUiQueryInterfaceLabelLifetime(void)
{
  UiTestApplication application;
  auto              label = Label::New("before");
  WeakHandleBase    weak(label);
  auto              text = QueryInterface<ITextContent>(label);
  label.Reset();
  DALI_TEST_CHECK(weak.GetBaseHandle());
  text->SetText("alive");
  DALI_TEST_CHECK(text->GetText() == String("alive"));
  text.Reset();
  DALI_TEST_CHECK(!weak.GetBaseHandle());
  END_TEST;
}
