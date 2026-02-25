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
#include <dali-ui-foundation/internal/visuals/visual-factory-impl.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/visuals/color/color-visual.h>
#include <dali-ui-foundation/internal/visuals/visual-string-constants.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace UI
{
namespace Internal
{

VisualFactory::VisualFactory(bool debugEnabled)
  : mDebugEnabled(debugEnabled),
    mPreMultiplyOnLoad(true)
{
}

VisualFactory::~VisualFactory() = default;

UI::Visual::Base VisualFactory::CreateVisual(const Property::Map& propertyMap)
{
  UI::Visual::Base visualHandle;

  Property::Value* typeValue = propertyMap.Find(UI::Visual::Property::TYPE);
  if (!typeValue)
  {
    typeValue = propertyMap.Find(VISUAL_TYPE);
  }

  UI::Visual::Type visualType = UI::Visual::COLOR;

  if (typeValue)
  {
    Scripting::GetEnumerationProperty(*typeValue, VISUAL_TYPE_TABLE, VISUAL_TYPE_TABLE_COUNT, visualType);
  }

  VisualFactoryCache& factoryCache = GetFactoryCache();

  switch (visualType)
  {
    case UI::Visual::COLOR:
    {
      visualHandle =
          UI::Visual::Base(ColorVisual::New(factoryCache, factoryCache.GetColorVisualShaderFactory(), propertyMap));
      break;
    }
    default:
    {
      DALI_LOG_ERROR("Visual type %d not supported in dali-ui\n", static_cast<int>(visualType));
      break;
    }
  }

  return visualHandle;
}

Dali::Geometry VisualFactory::GetDefaultQuadGeometry()
{
  VisualFactoryCache& factoryCache = GetFactoryCache();
  Geometry geometry = factoryCache.GetGeometry(VisualFactoryCache::QUAD_GEOMETRY);
  if (!geometry)
  {
    geometry = VisualFactoryCache::CreateQuadGeometry();
    factoryCache.SaveGeometry(VisualFactoryCache::QUAD_GEOMETRY, geometry);
  }
  return geometry;
}

void VisualFactory::SetPreMultiplyOnLoad(bool preMultiply)
{
  mPreMultiplyOnLoad = preMultiply;
  if (mFactoryCache)
  {
    mFactoryCache->SetPreMultiplyOnLoad(preMultiply);
  }
}

bool VisualFactory::GetPreMultiplyOnLoad() const
{
  return mPreMultiplyOnLoad;
}

void VisualFactory::DiscardVisual(UI::Visual::Base visual)
{
  // Simply let the visual go out of scope for now
  // In toolkit this queues for idle-time cleanup
}

VisualFactoryCache& VisualFactory::GetFactoryCache()
{
  if (!mFactoryCache)
  {
    mFactoryCache = std::make_unique<VisualFactoryCache>(mPreMultiplyOnLoad);
  }
  return *mFactoryCache;
}

} // namespace Internal
} // namespace UI
} // namespace Dali
