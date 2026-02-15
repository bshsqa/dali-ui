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
#include <dali-ui-visuals/internal/visuals/visual-trait-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali-ui-foundation/integration-api/view-impl.h>

// INTERNAL INCLUDES
#include <dali-ui-visuals/devel-api/visual-factory/visual-factory.h>

namespace Dali
{
namespace UI
{
namespace Internal
{

VisualTraitImpl::VisualTraitImpl()
{
}

VisualTraitImpl::~VisualTraitImpl()
{
  // Visuals will be released when vector is destroyed
}

void VisualTraitImpl::OnBeforeAttached(Integration::TraitId id, View& view)
{
}

void VisualTraitImpl::OnAttached(Integration::TraitId id, View& view)
{
  mView = view;
  
  // Attach all existing visuals to the new view actor
  Actor actor = view;
  for (auto& entry : mVisuals)
  {
    if (entry.visual && entry.enabled)
    {
      entry.visual.SetOnStage(actor);
    }
  }
}

void VisualTraitImpl::OnDetached(Integration::TraitId id, View& view)
{
  // Detach all visuals from the view actor
  Actor actor = view;
  if (actor)
  {
    for (auto& entry : mVisuals)
    {
      if (entry.visual && entry.enabled)
      {
        entry.visual.SetOffStage(actor);
      }
    }
  }
  mView.Reset();
}

void VisualTraitImpl::OnViewDestroying(Integration::ViewImpl* viewImpl)
{
  // View is dying, release our weak reference
  mView.Reset();
}

void VisualTraitImpl::RegisterVisual(Property::Index index, const Property::Map& properties)
{
  Toolkit::Visual::Base visual = Toolkit::VisualFactory::Get().CreateVisual(properties);
  RegisterVisual(index, visual);
}

void VisualTraitImpl::RegisterVisual(Property::Index index, Toolkit::Visual::Base& visual)
{
  if (!visual)
  {
    return;
  }

  auto iter = FindVisual(index);
  if (iter != mVisuals.end())
  {
    // Replace existing visual
    View view = mView.GetHandle();
    if (view && iter->enabled)
    {
      iter->visual.SetOffStage(view);
    }
    iter->visual = visual;
  }
  else
  {
    // Add new visual
    mVisuals.push_back({index, visual, true});
    iter = mVisuals.end() - 1;
  }

  // Attach to View if we are attached
  View view = mView.GetHandle();
  if (view && iter->enabled)
  {
    iter->visual.SetOnStage(view);
  }
}

void VisualTraitImpl::UnregisterVisual(Property::Index index)
{
  auto iter = FindVisual(index);
  if (iter != mVisuals.end())
  {
    View view = mView.GetHandle();
    if (view && iter->enabled)
    {
      iter->visual.SetOffStage(view);
    }
    mVisuals.erase(iter);
  }
}

Toolkit::Visual::Base VisualTraitImpl::GetVisual(Property::Index index) const
{
  auto iter = FindVisual(index);
  if (iter != mVisuals.end())
  {
    return iter->visual;
  }
  return Toolkit::Visual::Base();
}

void VisualTraitImpl::EnableVisual(Property::Index index, bool enable)
{
  auto iter = FindVisual(index);
  if (iter != mVisuals.end())
  {
    if (iter->enabled != enable)
    {
      iter->enabled = enable;
      View view = mView.GetHandle();
      if (view)
      {
        if (enable)
        {
          iter->visual.SetOnStage(view);
        }
        else
        {
          iter->visual.SetOffStage(view);
        }
      }
    }
  }
}

bool VisualTraitImpl::IsVisualEnabled(Property::Index index) const
{
  auto iter = FindVisual(index);
  if (iter != mVisuals.end())
  {
    return iter->enabled;
  }
  return false;
}

VisualTraitImpl::VisualContainer::iterator VisualTraitImpl::FindVisual(Property::Index index)
{
  for (auto it = mVisuals.begin(); it != mVisuals.end(); ++it)
  {
    if (it->index == index)
    {
      return it;
    }
  }
  return mVisuals.end();
}

VisualTraitImpl::VisualContainer::const_iterator VisualTraitImpl::FindVisual(Property::Index index) const
{
  for (auto it = mVisuals.begin(); it != mVisuals.end(); ++it)
  {
    if (it->index == index)
    {
      return it;
    }
  }
  return mVisuals.end();
}

} // namespace Internal
} // namespace UI
} // namespace Dali
