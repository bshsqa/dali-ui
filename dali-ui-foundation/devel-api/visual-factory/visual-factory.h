#ifndef DALI_UI_VISUAL_FACTORY_H
#define DALI_UI_VISUAL_FACTORY_H

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
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/rendering/geometry.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visual-factory/visual-base.h>

namespace Dali
{
namespace UI
{
namespace Internal DALI_INTERNAL
{
class VisualFactory;
}

class DALI_UI_API VisualFactory : public BaseHandle
{
public:
  static VisualFactory Get();

  VisualFactory();
  ~VisualFactory();
  VisualFactory(const VisualFactory& handle);
  VisualFactory& operator=(const VisualFactory& handle);

  Visual::Base CreateVisual(const Property::Map& propertyMap);

  Dali::Geometry GetDefaultQuadGeometry();

  void SetPreMultiplyOnLoad(bool preMultiply);
  bool GetPreMultiplyOnLoad() const;

  void DiscardVisual(Visual::Base visual);

private:
  explicit DALI_INTERNAL VisualFactory(Internal::VisualFactory* impl);
};

} // namespace UI
} // namespace Dali

#endif // DALI_UI_VISUAL_FACTORY_H
