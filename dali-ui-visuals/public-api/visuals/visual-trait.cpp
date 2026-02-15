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
#include <dali-ui-visuals/public-api/visuals/visual-trait.h>

// INTERNAL INCLUDES
#include <dali-ui-visuals/internal/visuals/visual-trait-impl.h>
#include <dali-ui-foundation/integration-api/trait-id.h>

namespace Dali
{
namespace UI
{

VisualTrait::VisualTrait(Internal::VisualTraitImpl* impl)
  : Trait(impl)
{
}

VisualTrait VisualTrait::New()
{
  return VisualTrait(new Internal::VisualTraitImpl());
}

VisualTrait VisualTrait::DownCast(BaseHandle handle)
{
  return VisualTrait(dynamic_cast<Internal::VisualTraitImpl*>(handle.GetObjectPtr()));
}

// Static Helpers (Type-Safe Accessor Pattern)

void VisualTrait::RegisterVisual(View& view, Property::Index index, const Property::Map& properties)
{
  VisualTrait trait = DownCast(view.GetTrait(Integration::ReservedTraitId::VISUAL_TRAIT));
  if (!trait)
  {
    trait = VisualTrait::New();
    view.SetTrait(Integration::ReservedTraitId::VISUAL_TRAIT, trait);
  }

  GetImpl(trait).RegisterVisual(index, properties);
}

void VisualTrait::RegisterVisual(View& view, Property::Index index, Toolkit::Visual::Base& visual)
{
  VisualTrait trait = DownCast(view.GetTrait(Integration::ReservedTraitId::VISUAL_TRAIT));
  if (!trait)
  {
    trait = VisualTrait::New();
    view.SetTrait(Integration::ReservedTraitId::VISUAL_TRAIT, trait);
  }

  GetImpl(trait).RegisterVisual(index, visual);
}

void VisualTrait::UnregisterVisual(View& view, Property::Index index)
{
  VisualTrait trait = DownCast(view.GetTrait(Integration::ReservedTraitId::VISUAL_TRAIT));
  if (trait)
  {
    GetImpl(trait).UnregisterVisual(index);
  }
}

Toolkit::Visual::Base VisualTrait::GetVisual(View& view, Property::Index index)
{
  VisualTrait trait = DownCast(view.GetTrait(Integration::ReservedTraitId::VISUAL_TRAIT));
  if (trait)
  {
    return GetImpl(trait).GetVisual(index);
  }
  return Toolkit::Visual::Base();
}

void VisualTrait::EnableVisual(View& view, Property::Index index, bool enable)
{
  VisualTrait trait = DownCast(view.GetTrait(Integration::ReservedTraitId::VISUAL_TRAIT));
  if (trait)
  {
    GetImpl(trait).EnableVisual(index, enable);
  }
}

bool VisualTrait::IsVisualEnabled(View& view, Property::Index index)
{
  VisualTrait trait = DownCast(view.GetTrait(Integration::ReservedTraitId::VISUAL_TRAIT));
  if (trait)
  {
    return GetImpl(trait).IsVisualEnabled(index);
  }
  return false;
}

} // namespace UI
} // namespace Dali

// Internal helper to get Impl from Handle
namespace Dali
{
namespace UI
{
inline Internal::VisualTraitImpl& GetImpl(VisualTrait& trait)
{
  DALI_ASSERT_ALWAYS(trait);
  Dali::RefObject& handle = trait.GetImplementation();
  return static_cast<Internal::VisualTraitImpl&>(handle);
}

inline const Internal::VisualTraitImpl& GetImpl(const VisualTrait& trait)
{
  DALI_ASSERT_ALWAYS(trait);
  const Dali::RefObject& handle = trait.GetImplementation();
  return static_cast<const Internal::VisualTraitImpl&>(handle);
}
} // namespace UI
} // namespace Dali
