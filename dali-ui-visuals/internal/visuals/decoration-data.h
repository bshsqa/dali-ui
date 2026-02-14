#ifndef DALI_UI_INTERNAL_DECORATION_DATA_H
#define DALI_UI_INTERNAL_DECORATION_DATA_H

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
#include <dali/public-api/math/vector4.h>
#include <dali-ui-visuals/devel-api/visuals/visual-properties-devel.h>

namespace Dali
{
namespace UI
{
namespace Internal
{
/**
 * @brief This struct is used to pass data from control to visual.
 * To reduce memory consumption, we will allocate this struct only if control has decoration.
 */
struct DecorationData
{
  DecorationData()
  : mBorderlineColor(Color::BLACK),
    mBorderlineWidth(0.0f),
    mBorderlineOffset(0.0f),
    mCornerRadius(Vector4::ZERO),
    mCornerSquareness(Vector4::ZERO),
    mCornerRadiusPolicy(UI::Visual::Transform::Policy::ABSOLUTE)
  {
  }

  // Borderline
  Vector4 mBorderlineColor;
  float   mBorderlineWidth;
  float   mBorderlineOffset;

  // Corner Radius
  Vector4 mCornerRadius;
  Vector4 mCornerSquareness;
  int     mCornerRadiusPolicy;

  static float GetBorderlineWidth(const DecorationData* data)
  {
    return (data) ? data->mBorderlineWidth : 0.0f;
  }

  static void SetBorderlineWidth(DecorationData*& data, float value)
  {
    if(DALI_LIKELY(data))
    {
      data->mBorderlineWidth = value;
    }
    else if(!Dali::EqualsZero(value))
    {
      data                   = new DecorationData();
      data->mBorderlineWidth = value;
    }
  }

  static Vector4 GetBorderlineColor(const DecorationData* data)
  {
    return (data) ? data->mBorderlineColor : Color::BLACK;
  }

  static void SetBorderlineColor(DecorationData*& data, Vector4 value)
  {
    if(DALI_LIKELY(data))
    {
      data->mBorderlineColor = value;
    }
    else if(value != Color::BLACK)
    {
      data                   = new DecorationData();
      data->mBorderlineColor = value;
    }
  }

  static float GetBorderlineOffset(const DecorationData* data)
  {
    return (data) ? data->mBorderlineOffset : 0.0f;
  }

  static void SetBorderlineOffset(DecorationData*& data, float value)
  {
    if(DALI_LIKELY(data))
    {
      data->mBorderlineOffset = value;
    }
    else if(!Dali::EqualsZero(value))
    {
      data                    = new DecorationData();
      data->mBorderlineOffset = value;
    }
  }

  static Vector4 GetCornerRadius(const DecorationData* data)
  {
    return (data) ? data->mCornerRadius : Vector4::ZERO;
  }

  static void SetCornerRadius(DecorationData*& data, Vector4 value)
  {
    if(DALI_LIKELY(data))
    {
      data->mCornerRadius = value;
    }
    else if(value != Vector4::ZERO)
    {
      data                = new DecorationData();
      data->mCornerRadius = value;
    }
  }

  static int GetCornerRadiusPolicy(const DecorationData* data)
  {
    return (data) ? data->mCornerRadiusPolicy : static_cast<int>(UI::Visual::Transform::Policy::ABSOLUTE);
  }

  static void SetCornerRadiusPolicy(DecorationData*& data, int value)
  {
    if(DALI_LIKELY(data))
    {
      data->mCornerRadiusPolicy = value;
    }
    else if(value != static_cast<int>(UI::Visual::Transform::Policy::ABSOLUTE))
    {
      data                      = new DecorationData();
      data->mCornerRadiusPolicy = value;
    }
  }

  static Vector4 GetCornerSquareness(const DecorationData* data)
  {
    return (data) ? data->mCornerSquareness : Vector4::ZERO;
  }

  static void SetCornerSquareness(DecorationData*& data, Vector4 value)
  {
    if(DALI_LIKELY(data))
    {
      data->mCornerSquareness = value;
    }
    else if(value != Vector4::ZERO)
    {
      data                    = new DecorationData();
      data->mCornerSquareness = value;
    }
  }
};

} // namespace Internal

} // namespace UI

} // namespace Dali

#endif // DALI_UI_INTERNAL_DECORATION_DATA_H
