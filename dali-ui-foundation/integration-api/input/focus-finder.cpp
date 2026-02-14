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

/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Modified by joogab yun(joogab.yun@samsung.com)
 */

// CLASS HEADER
#include "focus-finder.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/public-api/actors/layer.h>
#include <algorithm>
#include <cmath>

namespace Dali
{
namespace UI
{
namespace Internal
{
namespace FocusFinder
{
namespace
{
static constexpr float FULLY_TRANSPARENT(0.01f);

static int MajorAxisDistanceRaw(UI::Focus::Direction direction, Dali::Rect<float> source, Dali::Rect<float> dest)
{
  switch(direction)
  {
    case UI::Focus::Direction::LEFT:
      return source.left - dest.right;
    case UI::Focus::Direction::RIGHT:
      return dest.left - source.right;
    case UI::Focus::Direction::UP:
      return source.top - dest.bottom;
    case UI::Focus::Direction::DOWN:
      return dest.top - source.bottom;
    default:
      return 0;
  }
}

static int MajorAxisDistance(UI::Focus::Direction direction, Dali::Rect<float> source, Dali::Rect<float> dest)
{
  return std::max(0, MajorAxisDistanceRaw(direction, source, dest));
}

static int MajorAxisDistanceToFarEdgeRaw(UI::Focus::Direction direction, Dali::Rect<float> source, Dali::Rect<float> dest)
{
  switch(direction)
  {
    case UI::Focus::Direction::LEFT:
      return source.left - dest.left;
    case UI::Focus::Direction::RIGHT:
      return dest.right - source.right;
    case UI::Focus::Direction::UP:
      return source.top - dest.top;
    case UI::Focus::Direction::DOWN:
      return dest.bottom - source.bottom;
    default:
      return 0;
  }
}

static int MajorAxisDistanceToFarEdge(UI::Focus::Direction direction, Dali::Rect<float> source, Dali::Rect<float> dest)
{
  return std::max(1, MajorAxisDistanceToFarEdgeRaw(direction, source, dest));
}

static int MinorAxisDistance(UI::Focus::Direction direction, Dali::Rect<float> source, Dali::Rect<float> dest)
{
  switch(direction)
  {
    case UI::Focus::Direction::LEFT:
    case UI::Focus::Direction::RIGHT:
    {
      return std::abs((source.top + (source.bottom - source.top) * 0.5f) -
                      (dest.top + (dest.bottom - dest.top) * 0.5f));
    }
    case UI::Focus::Direction::UP:
    case UI::Focus::Direction::DOWN:
    {
      return std::abs((source.left + (source.right - source.left) * 0.5f) -
                      (dest.left + (dest.right - dest.left) * 0.5f));
    }
    default:
      return 0;
  }
}

static uint64_t GetWeightedDistanceFor(int majorAxisDistance, int minorAxisDistance)
{
  return 13 * static_cast<int64_t>(majorAxisDistance) * static_cast<int64_t>(majorAxisDistance) +
         static_cast<int64_t>(minorAxisDistance) * static_cast<int64_t>(minorAxisDistance);
}

static void ConvertCoordinate(Dali::Rect<float>& rect)
{
  float left   = rect.x;
  float right  = rect.x + rect.width;
  float bottom = rect.y + rect.height;
  float top    = rect.y;

  rect.left   = left;
  rect.right  = right;
  rect.bottom = bottom;
  rect.top    = top;
}

static bool IsCandidate(Dali::Rect<float> srcRect, Dali::Rect<float> destRect, UI::Focus::Direction direction)
{
  switch(direction)
  {
    case UI::Focus::Direction::LEFT:
      return (srcRect.right > destRect.right || srcRect.left >= destRect.right) && srcRect.left > destRect.left;
    case UI::Focus::Direction::RIGHT:
      return (srcRect.left < destRect.left || srcRect.right <= destRect.left) && srcRect.right < destRect.right;
    case UI::Focus::Direction::UP:
      return (srcRect.bottom > destRect.bottom || srcRect.top >= destRect.bottom) && srcRect.top > destRect.top;
    case UI::Focus::Direction::DOWN:
      return (srcRect.top < destRect.top || srcRect.bottom <= destRect.top) && srcRect.bottom < destRect.bottom;
    default:
      return false;
  }
}

static bool IsToDirectionOf(UI::Focus::Direction direction, Dali::Rect<float> src, Dali::Rect<float> dest)
{
  switch(direction)
  {
    case UI::Focus::Direction::LEFT:
      return src.left >= dest.right;
    case UI::Focus::Direction::RIGHT:
      return src.right <= dest.left;
    case UI::Focus::Direction::UP:
      return src.top >= dest.bottom;
    case UI::Focus::Direction::DOWN:
      return src.bottom <= dest.top;
    default:
      return false;
  }
}

static bool BeamsOverlap(UI::Focus::Direction direction, Dali::Rect<float> rect1, Dali::Rect<float> rect2)
{
  switch(direction)
  {
    case UI::Focus::Direction::LEFT:
    case UI::Focus::Direction::RIGHT:
      return (rect2.bottom >= rect1.top) && (rect2.top <= rect1.bottom);
    case UI::Focus::Direction::UP:
    case UI::Focus::Direction::DOWN:
      return (rect2.right >= rect1.left) && (rect2.left <= rect1.right);
    default:
      return false;
  }
}

static bool BeamBeats(UI::Focus::Direction direction, Dali::Rect<float> source, Dali::Rect<float> rect1, Dali::Rect<float> rect2)
{
  const bool rect1InSrcBeam = BeamsOverlap(direction, source, rect1);
  const bool rect2InSrcBeam = BeamsOverlap(direction, source, rect2);

  if(rect2InSrcBeam || !rect1InSrcBeam)
  {
    return false;
  }

  if(!IsToDirectionOf(direction, source, rect2))
  {
    return true;
  }

  if(direction == UI::Focus::Direction::LEFT || direction == UI::Focus::Direction::RIGHT)
  {
    return true;
  }

  return (MajorAxisDistance(direction, source, rect1) < MajorAxisDistanceToFarEdge(direction, source, rect2));
}

bool IsBetterCandidate(UI::Focus::Direction direction, Rect<float>& focusedRect, Rect<float>& candidateRect, Rect<float>& bestCandidateRect)
{
  if(!IsCandidate(focusedRect, candidateRect, direction))
  {
    return false;
  }

  if(!IsCandidate(focusedRect, bestCandidateRect, direction))
  {
    return true;
  }

  if(BeamBeats(direction, focusedRect, candidateRect, bestCandidateRect))
  {
    return true;
  }

  if(BeamBeats(direction, focusedRect, bestCandidateRect, candidateRect))
  {
    return false;
  }

  return (GetWeightedDistanceFor(
            MajorAxisDistance(direction, focusedRect, candidateRect),
            MinorAxisDistance(direction, focusedRect, candidateRect)) <
          GetWeightedDistanceFor(
            MajorAxisDistance(direction, focusedRect, bestCandidateRect),
            MinorAxisDistance(direction, focusedRect, bestCandidateRect)));
}

bool IsFocusable(Actor& actor)
{
  return (actor.GetProperty<bool>(Actor::Property::KEYBOARD_FOCUSABLE) &&
          actor.GetProperty<bool>(DevelActor::Property::USER_INTERACTION_ENABLED) &&
          actor.GetProperty<bool>(Actor::Property::VISIBLE) &&
          !actor.GetCurrentProperty<bool>(DevelActor::Property::WORLD_IGNORED) &&
          actor.GetProperty<Vector4>(Actor::Property::WORLD_COLOR).a > FULLY_TRANSPARENT);
}

Actor FindNextFocus(Actor& actor, Actor& focusedActor, Rect<float>& focusedRect, Rect<float>& bestCandidateRect, UI::Focus::Direction direction)
{
  Actor nearestActor;
  if(actor &&
     actor.GetProperty<bool>(Actor::Property::VISIBLE) &&
     !actor.GetCurrentProperty<bool>(DevelActor::Property::WORLD_IGNORED) &&
     actor.GetProperty<bool>(DevelActor::Property::KEYBOARD_FOCUSABLE_CHILDREN))
  {
    const auto childCount = actor.GetChildCount();
    for(auto i = childCount; i > 0u; --i)
    {
      Dali::Actor child = actor.GetChildAt(i - 1);
      if(child && child != focusedActor && IsFocusable(child))
      {
        Rect<float> candidateRect = DevelActor::CalculateCurrentScreenExtents(child);
        ConvertCoordinate(candidateRect);

        if(IsBetterCandidate(direction, focusedRect, candidateRect, bestCandidateRect))
        {
          bestCandidateRect = candidateRect;
          nearestActor      = child;
        }
      }
      Actor nextActor = FindNextFocus(child, focusedActor, focusedRect, bestCandidateRect, direction);
      if(nextActor)
      {
        nearestActor = nextActor;
      }
    }
  }
  return nearestActor;
}

} // unnamed namespace

Actor GetNearestFocusableActor(Actor rootActor, Actor focusedActor, UI::Focus::Direction direction)
{
  Actor nearestActor;
  if(!rootActor)
  {
    return nearestActor;
  }

  Rect<float> focusedRect;
  if(!focusedActor)
  {
    Rect<float> rootRect = DevelActor::CalculateCurrentScreenExtents(rootActor);
    focusedRect          = Rect<float>(rootRect.x, rootRect.y, 0.f, 0.f);
  }
  else
  {
    focusedRect = DevelActor::CalculateCurrentScreenExtents(focusedActor);
  }

  Rect<float> bestCandidateRect = focusedRect;
  switch(direction)
  {
    case UI::Focus::Direction::LEFT:
      bestCandidateRect.x += 1;
      break;
    case UI::Focus::Direction::RIGHT:
      bestCandidateRect.x -= 1;
      break;
    case UI::Focus::Direction::UP:
      bestCandidateRect.y += 1;
      break;
    case UI::Focus::Direction::DOWN:
      bestCandidateRect.y -= 1;
      break;
    default:
      break;
  }

  ConvertCoordinate(bestCandidateRect);
  ConvertCoordinate(focusedRect);
  nearestActor = FindNextFocus(rootActor, focusedActor, focusedRect, bestCandidateRect, direction);
  return nearestActor;
}

} // namespace FocusFinder

} // namespace Internal

} // namespace UI

} // namespace Dali
