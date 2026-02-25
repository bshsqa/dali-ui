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
#include <dali-ui-foundation/internal/visuals/visual-base-data-impl.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/visuals/visual-string-constants.h>
#include <dali-ui-foundation/public-api/align-enumerations.h>
#include <dali-ui-foundation/devel-api/direction-enums.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/public-api/object/property-map.h>

namespace Dali
{
namespace UI
{
namespace Internal
{
namespace Visual
{

// CustomShader
void CustomShader::SetPropertyMap(const Property::Map& propertyMap)
{
  Property::Value* vertexValue = propertyMap.Find(CUSTOM_VERTEX_SHADER);
  if (vertexValue)
  {
    vertexValue->Get(vertexShader);
  }

  Property::Value* fragmentValue = propertyMap.Find(CUSTOM_FRAGMENT_SHADER);
  if (fragmentValue)
  {
    fragmentValue->Get(fragmentShader);
  }

  Property::Value* gridXValue = propertyMap.Find(CUSTOM_SUBDIVIDE_GRID_X);
  if (gridXValue)
  {
    gridXValue->Get(subdivideGridX);
  }

  Property::Value* gridYValue = propertyMap.Find(CUSTOM_SUBDIVIDE_GRID_Y);
  if (gridYValue)
  {
    gridYValue->Get(subdivideGridY);
  }

  Property::Value* renderPassTagValue = propertyMap.Find(CUSTOM_RENDER_PASS_TAG);
  if (renderPassTagValue)
  {
    renderPassTagValue->Get(renderPassTag);
  }

  Property::Value* hintsValue = propertyMap.Find(CUSTOM_SHADER_HINTS);
  if (hintsValue)
  {
    int hintsInt = 0;
    if (hintsValue->Get(hintsInt))
    {
      hints = static_cast<Shader::Hint::Value>(hintsInt);
    }
  }

  Property::Value* nameValue = propertyMap.Find(CUSTOM_SHADER_NAME);
  if (nameValue)
  {
    nameValue->Get(name);
  }
}

void CustomShader::CreatePropertyMap(Property::Map& map) const
{
  if (!vertexShader.empty() || !fragmentShader.empty())
  {
    Property::Map shaderMap;
    if (!vertexShader.empty())
    {
      shaderMap.Insert(CUSTOM_VERTEX_SHADER, vertexShader);
    }
    if (!fragmentShader.empty())
    {
      shaderMap.Insert(CUSTOM_FRAGMENT_SHADER, fragmentShader);
    }
    if (subdivideGridX > 1)
    {
      shaderMap.Insert(CUSTOM_SUBDIVIDE_GRID_X, subdivideGridX);
    }
    if (subdivideGridY > 1)
    {
      shaderMap.Insert(CUSTOM_SUBDIVIDE_GRID_Y, subdivideGridY);
    }
    if (renderPassTag != 0)
    {
      shaderMap.Insert(CUSTOM_RENDER_PASS_TAG, renderPassTag);
    }
    if (hints != Shader::Hint::NONE)
    {
      shaderMap.Insert(CUSTOM_SHADER_HINTS, static_cast<int>(hints));
    }
    if (!name.empty())
    {
      shaderMap.Insert(CUSTOM_SHADER_NAME, name);
    }
    map.Insert(CUSTOM_SHADER, shaderMap);
  }
}

// Transform
Transform::Transform()
  : mOffset(Vector2::ZERO),
    mSize(Vector2::ONE),
    mExtraSize(Vector2::ZERO),
    mOffsetSizeMode(Vector4::ZERO),
    mOrigin(Align::TOP_BEGIN),
    mAnchorPoint(Align::TOP_BEGIN)
{
}

void Transform::SetPropertyMap(const Property::Map& propertyMap)
{
  // Offset
  Property::Value* offsetValue =
      propertyMap.Find(static_cast<Property::Index>(UI::Visual::Transform::Property::OFFSET));
  if (!offsetValue)
  {
    offsetValue = propertyMap.Find(OFFSET);
  }
  if (offsetValue)
  {
    offsetValue->Get(mOffset);
  }

  // Size
  Property::Value* sizeValue = propertyMap.Find(static_cast<Property::Index>(UI::Visual::Transform::Property::SIZE));
  if (!sizeValue)
  {
    sizeValue = propertyMap.Find(SIZE);
  }
  if (sizeValue)
  {
    sizeValue->Get(mSize);
  }

  // Origin
  Property::Value* originValue =
      propertyMap.Find(static_cast<Property::Index>(UI::Visual::Transform::Property::ORIGIN));
  if (!originValue)
  {
    originValue = propertyMap.Find(ORIGIN);
  }
  if (originValue)
  {
    int originInt;
    if (originValue->Get(originInt))
    {
      mOrigin = static_cast<Align::Type>(originInt);
    }
  }

  // Anchor point
  Property::Value* anchorValue =
      propertyMap.Find(static_cast<Property::Index>(UI::Visual::Transform::Property::ANCHOR_POINT));
  if (!anchorValue)
  {
    anchorValue = propertyMap.Find(ANCHOR_POINT);
  }
  if (anchorValue)
  {
    int anchorInt;
    if (anchorValue->Get(anchorInt))
    {
      mAnchorPoint = static_cast<Align::Type>(anchorInt);
    }
  }

  // Offset policy
  Property::Value* offsetPolicyValue =
      propertyMap.Find(static_cast<Property::Index>(UI::Visual::Transform::Property::OFFSET_POLICY));
  if (offsetPolicyValue)
  {
    Vector2 policy;
    if (offsetPolicyValue->Get(policy))
    {
      mOffsetSizeMode.x = policy.x;
      mOffsetSizeMode.y = policy.y;
    }
  }

  // Size policy
  Property::Value* sizePolicyValue =
      propertyMap.Find(static_cast<Property::Index>(UI::Visual::Transform::Property::SIZE_POLICY));
  if (sizePolicyValue)
  {
    Vector2 policy;
    if (sizePolicyValue->Get(policy))
    {
      mOffsetSizeMode.z = policy.x;
      mOffsetSizeMode.w = policy.y;
    }
  }

  // Extra size
  Property::Value* extraSizeValue = propertyMap.Find(EXTRA_SIZE);
  if (extraSizeValue)
  {
    extraSizeValue->Get(mExtraSize);
  }
}

void Transform::CreatePropertyMap(Property::Map& map) const
{
  map.Insert(static_cast<Property::Index>(UI::Visual::Transform::Property::OFFSET), mOffset);
  map.Insert(static_cast<Property::Index>(UI::Visual::Transform::Property::SIZE), mSize);
  map.Insert(static_cast<Property::Index>(UI::Visual::Transform::Property::ORIGIN), static_cast<int>(mOrigin));
  map.Insert(static_cast<Property::Index>(UI::Visual::Transform::Property::ANCHOR_POINT),
             static_cast<int>(mAnchorPoint));
  map.Insert(static_cast<Property::Index>(UI::Visual::Transform::Property::OFFSET_POLICY),
             Vector2(mOffsetSizeMode.x, mOffsetSizeMode.y));
  map.Insert(static_cast<Property::Index>(UI::Visual::Transform::Property::SIZE_POLICY),
             Vector2(mOffsetSizeMode.z, mOffsetSizeMode.w));
}

void Transform::UpdatePropertyMap(const Property::Map& propertyMap)
{
  SetPropertyMap(propertyMap);
}

void Transform::RegisterUniforms(Renderer renderer, UI::Visual::Transform::Policy::Type offsetXPolicy,
                                 UI::Visual::Transform::Policy::Type offsetYPolicy,
                                 UI::Visual::Transform::Policy::Type sizeXPolicy,
                                 UI::Visual::Transform::Policy::Type sizeYPolicy)
{
  renderer.RegisterProperty("offset", mOffset);
  renderer.RegisterProperty("size", mSize);
  renderer.RegisterProperty("extraSize", mExtraSize);
  renderer.RegisterProperty("offsetSizeMode", mOffsetSizeMode);
}

void Transform::SetUniforms(Renderer renderer, UI::Direction::Type direction)
{
  renderer.RegisterProperty("offset", mOffset);
  renderer.RegisterProperty("size", mSize);
  renderer.RegisterProperty("extraSize", mExtraSize);
  renderer.RegisterProperty("offsetSizeMode", mOffsetSizeMode);
}

// Base::Impl
Base::Impl::Impl(FittingMode fittingMode, UI::Visual::Type type)
  : mMixColor(Color::WHITE),
    mControlSize(Vector2::ZERO),
    mBorderlineWidth(0.0f),
    mBorderlineOffset(0.0f),
    mBorderlineColor(Color::BLACK),
    mDepthIndex(0),
    mFittingMode(fittingMode),
    mType(type),
    mResourceStatus(UI::Visual::ResourceStatus::PREPARING),
    mEventObserver(nullptr),
    mConstraintObserver(nullptr),
    mFlags(0)
{
}

Base::Impl::~Impl() = default;

} // namespace Visual
} // namespace Internal
} // namespace UI
} // namespace Dali
