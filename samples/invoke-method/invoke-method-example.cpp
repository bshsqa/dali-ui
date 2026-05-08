/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float LAYER_SIZE   = 150.0f;

template<typename ValueType>
bool SetPropertyByName(Handle handle, const char* propertyName, const ValueType& value)
{
  Property::Index index = handle.GetPropertyIndex(Property::Key(propertyName));
  if(index == Property::INVALID_INDEX)
  {
    DALI_LOG_ERROR("[invoke-method-sample] SetProperty(%s): INVALID_INDEX\n", propertyName);
    return false;
  }

  handle.SetProperty(index, value);
  return true;
}

Layer CreateLayerByTypeRegistry()
{
  TypeInfo typeInfo = TypeRegistry::Get().GetTypeInfo("Layer");
  if(!typeInfo)
  {
    DALI_LOG_ERROR("[invoke-method-sample] TypeRegistry::GetTypeInfo(Layer): FAIL\n");
    return Layer();
  }

  Layer layer = Layer::DownCast(typeInfo.CreateInstance());
  if(!layer)
  {
    DALI_LOG_ERROR("[invoke-method-sample] TypeInfo::CreateInstance(Layer): FAIL\n");
  }

  return layer;
}

View NewLayerPanel(const Vector4& color)
{
  View view = View::New();
  SetPropertyByName(view, "position", Vector3::ZERO);
  SetPropertyByName(view, "size", Vector3(LAYER_SIZE, LAYER_SIZE, 0.0f));
  view.SetRequestedWidth(LAYER_SIZE);
  view.SetRequestedHeight(LAYER_SIZE);
  SetPropertyByName(view, "background", color);

  SetPropertyByName(view, "parentOrigin", ParentOrigin::TOP_LEFT);
  SetPropertyByName(view, "pivot", Pivot::TOP_LEFT);
  return view;
}

void SetLayerBounds(Layer layer, const Vector3& position)
{
  SetPropertyByName(layer, "parentOrigin", ParentOrigin::TOP_LEFT);
  SetPropertyByName(layer, "pivot", Pivot::TOP_LEFT);
  SetPropertyByName(layer, "position", position);
  SetPropertyByName(layer, "size", Vector3(LAYER_SIZE, LAYER_SIZE, 0.0f));
}

void PrintResult(const char* name, bool passed)
{
  DALI_LOG_ERROR("[invoke-method-sample] %s: %s\n", name, passed ? "PASS" : "FAIL");
}
} // namespace

class InvokeMethodController : public ConnectionTracker
{
public:
  explicit InvokeMethodController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &InvokeMethodController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    TestLayerInvokeMethod(window);
    TestLabelSetPropertyAndInvokeMethod(window);
  }

private:
  void TestLayerInvokeMethod(Window& window)
  {
    Layer layer1 = CreateLayerByTypeRegistry();
    Layer layer2 = CreateLayerByTypeRegistry();
    Layer layer3 = CreateLayerByTypeRegistry();

    if(!layer1 || !layer2 || !layer3)
    {
      return;
    }

    bool layer1DepthTestPassed = SetPropertyByName(layer1, "depthTest", false);
    bool layer2DepthTestPassed = SetPropertyByName(layer2, "depthTest", false);
    bool layer3DepthTestPassed = SetPropertyByName(layer3, "depthTest", false);

    SetLayerBounds(layer1, Vector3(40.0f, 50.0f, 0.0f));
    SetLayerBounds(layer2, Vector3(75.0f, 85.0f, 0.0f));
    SetLayerBounds(layer3, Vector3(110.0f, 120.0f, 0.0f));

    layer1.Add(NewLayerPanel(Color::RED));
    layer2.Add(NewLayerPanel(Color::BLUE));
    layer3.Add(NewLayerPanel(Color::GREEN));

    window.Add(layer3);
    window.Add(layer2);
    window.Add(layer1);

    InvokeArguments raiseAboveArguments;
    raiseAboveArguments.PushBack(Any(layer1));

    InvokeResult result;
    bool         raiseAbovePassed = layer2.InvokeMethod("RaiseAbove", raiseAboveArguments, result);
    bool         raiseToTopPassed = layer3.InvokeMethod("RaiseToTop", InvokeArguments(), result);

    PrintResult("Layer::TypeRegistry(CreateInstance)", true);
    PrintResult("Layer::SetProperty(depthTest by string)", layer1DepthTestPassed && layer2DepthTestPassed && layer3DepthTestPassed);
    PrintResult("Layer2::InvokeMethod(RaiseAbove Layer1)", raiseAbovePassed);
    PrintResult("Layer3::InvokeMethod(RaiseToTop)", raiseToTopPassed);
  }

  void TestLabelSetPropertyAndInvokeMethod(Window& window)
  {
    Label label = Label::New();

    bool layoutPropertiesPassed = true;
    layoutPropertiesPassed &= SetPropertyByName(label, "parentOrigin", ParentOrigin::CENTER);
    layoutPropertiesPassed &= SetPropertyByName(label, "pivot", Pivot::CENTER);
    layoutPropertiesPassed &= SetPropertyByName(label, "position", Vector3(0.0f, 220.0f, 0.0f));
    layoutPropertiesPassed &= SetPropertyByName(label, "size", Vector3(420.0f, 100.0f, 0.0f));

    bool labelPropertiesPassed = true;
    labelPropertiesPassed &= SetPropertyByName(label, "fontSize", 26.0f);
    labelPropertiesPassed &= SetPropertyByName(label, "textColor", Color::BLACK);

    InvokeArguments setTextArguments;
    setTextArguments.PushBack(Any(Dali::String("Label SetText by InvokeMethod")));

    InvokeResult result;
    bool         setTextPassed = label.InvokeMethod("SetText", setTextArguments, result);

    window.Add(label);

    PrintResult("Label::SetProperty(layout by string)", layoutPropertiesPassed);
    PrintResult("Label::SetProperty(fontSize/textColor by string)", labelPropertiesPassed);
    PrintResult("Label::InvokeMethod(SetText)", setTextPassed);
  }

private:
  Application& mApplication;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  InvokeMethodController controller(application);
  application.MainLoop();
  return 0;
}
