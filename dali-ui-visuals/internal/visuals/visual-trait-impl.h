#ifndef DALI_UI_INTERNAL_VISUAL_TRAIT_IMPL_H
#define DALI_UI_INTERNAL_VISUAL_TRAIT_IMPL_H

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
#include <vector>
#include <utility>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/trait-impl.h>
#include <dali-ui-visuals/devel-api/visual-factory/visual-base.h>

namespace Dali
{
namespace UI
{
namespace Internal
{

/**
 * @brief internal implementation of VisualTrait.
 * Manages a list of visuals attached to a View.
 */
class VisualTraitImpl : public Integration::TraitImpl
{
public:
  VisualTraitImpl();
  ~VisualTraitImpl() override;

  // Trait Lifecycle Methods
  void OnBeforeAttached(Integration::TraitId id, View& view) override;
  void OnAttached(Integration::TraitId id, View& view) override;
  void OnDetached(Integration::TraitId id, View& view) override;
  void OnViewDestroying(Integration::ViewImpl* viewImpl) override;

  // Visual Management Methods
  void RegisterVisual(Property::Index index, const Property::Map& properties);
  void RegisterVisual(Property::Index index, Toolkit::Visual::Base& visual);
  void UnregisterVisual(Property::Index index);
  Toolkit::Visual::Base GetVisual(Property::Index index) const;
  void EnableVisual(Property::Index index, bool enable);
  bool IsVisualEnabled(Property::Index index) const;

private:
  struct VisualEntry
  {
    Property::Index index;
    Toolkit::Visual::Base visual;
    bool enabled;
  };

  using VisualContainer = std::vector<VisualEntry>;

  VisualContainer mVisuals;
  WeakHandle<View> mView; // Weak reference to the owner view to prevent cycles
  
  // Helpers
  VisualContainer::iterator FindVisual(Property::Index index);
  VisualContainer::const_iterator FindVisual(Property::Index index) const;
};

} // namespace Internal
} // namespace UI
} // namespace Dali

#endif // DALI_UI_INTERNAL_VISUAL_TRAIT_IMPL_H
