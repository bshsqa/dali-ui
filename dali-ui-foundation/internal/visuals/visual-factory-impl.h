#ifndef DALI_UI_INTERNAL_VISUAL_FACTORY_IMPL_H
#define DALI_UI_INTERNAL_VISUAL_FACTORY_IMPL_H

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
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/devel-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-cache.h>

#include <memory>

namespace Dali
{
namespace UI
{
namespace Internal
{

/**
 * VisualFactory implementation. Currently only supports Color Visual.
 */
class VisualFactory : public BaseObject
{
public:
  VisualFactory(bool debugEnabled);
  ~VisualFactory() override;

  UI::Visual::Base CreateVisual(const Property::Map& propertyMap);

  Dali::Geometry GetDefaultQuadGeometry();

  void SetPreMultiplyOnLoad(bool preMultiply);
  bool GetPreMultiplyOnLoad() const;

  void DiscardVisual(UI::Visual::Base visual);

private:
  VisualFactoryCache& GetFactoryCache();

  std::unique_ptr<VisualFactoryCache> mFactoryCache;
  bool mDebugEnabled;
  bool mPreMultiplyOnLoad;
};

// Helpers for forwarding
inline VisualFactory& GetImplementation(UI::VisualFactory& handle)
{
  DALI_ASSERT_ALWAYS(handle && "VisualFactory handle is empty");
  BaseObject& object = handle.GetBaseObject();
  return static_cast<VisualFactory&>(object);
}

inline const VisualFactory& GetImplementation(const UI::VisualFactory& handle)
{
  DALI_ASSERT_ALWAYS(handle && "VisualFactory handle is empty");
  const BaseObject& object = handle.GetBaseObject();
  return static_cast<const VisualFactory&>(object);
}

} // namespace Internal
} // namespace UI
} // namespace Dali

#endif // DALI_UI_INTERNAL_VISUAL_FACTORY_IMPL_H
