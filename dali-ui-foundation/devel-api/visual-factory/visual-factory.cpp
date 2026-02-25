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
#include <dali-ui-foundation/devel-api/visual-factory/visual-factory.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/environment-variable.h>
#include <dali/devel-api/common/singleton-service.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/visuals/visual-factory-impl.h>

namespace Dali
{
namespace UI
{
namespace
{
const char* const DALI_DEBUG_RENDERING("DALI_DEBUG_RENDERING");
}

VisualFactory VisualFactory::Get()
{
  VisualFactory factory;

  SingletonService singletonService(SingletonService::Get());
  if (singletonService)
  {
    BaseHandle handle = singletonService.GetSingleton(typeid(VisualFactory));
    if (handle)
    {
      factory = VisualFactory(dynamic_cast<Internal::VisualFactory*>(handle.GetObjectPtr()));
    }

    if (!factory)
    {
      bool debugRendering = EnvironmentVariable::GetEnvironmentVariable(DALI_DEBUG_RENDERING) != nullptr;
      factory = VisualFactory(new Internal::VisualFactory(debugRendering));
      singletonService.Register(typeid(VisualFactory), factory);
    }
  }

  return factory;
}

VisualFactory::VisualFactory()
{
}

VisualFactory::~VisualFactory()
{
}

VisualFactory::VisualFactory(const VisualFactory& handle)
  : BaseHandle(handle)
{
}

VisualFactory& VisualFactory::operator=(const VisualFactory& handle)
{
  BaseHandle::operator=(handle);
  return *this;
}

VisualFactory::VisualFactory(Internal::VisualFactory* impl)
  : BaseHandle(impl)
{
}

Visual::Base VisualFactory::CreateVisual(const Property::Map& propertyMap)
{
  return GetImplementation(*this).CreateVisual(propertyMap);
}

Dali::Geometry VisualFactory::GetDefaultQuadGeometry()
{
  return GetImplementation(*this).GetDefaultQuadGeometry();
}

void VisualFactory::SetPreMultiplyOnLoad(bool preMultiply)
{
  GetImplementation(*this).SetPreMultiplyOnLoad(preMultiply);
}

bool VisualFactory::GetPreMultiplyOnLoad() const
{
  return GetImplementation(*this).GetPreMultiplyOnLoad();
}

void VisualFactory::DiscardVisual(Visual::Base visual)
{
  GetImplementation(*this).DiscardVisual(visual);
}

} // namespace UI
} // namespace Dali
