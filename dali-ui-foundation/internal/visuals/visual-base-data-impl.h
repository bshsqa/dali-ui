#ifndef DALI_UI_INTERNAL_VISUAL_BASE_DATA_IMPL_H
#define DALI_UI_INTERNAL_VISUAL_BASE_DATA_IMPL_H

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
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/rendering/shader.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visuals/visual-properties-devel.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/public-api/align-enumerations.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

#include <memory>
#include <string>

namespace Dali
{
namespace UI
{
namespace Internal
{
namespace Visual
{

/**
 * Custom shader data
 */
struct CustomShader
{
  CustomShader()
    : subdivideGridX(1),
      subdivideGridY(1),
      renderPassTag(0),
      hints(Shader::Hint::NONE)
  {
  }

  void SetPropertyMap(const Property::Map& propertyMap);
  void CreatePropertyMap(Property::Map& map) const;

  std::string vertexShader;
  std::string fragmentShader;
  int subdivideGridX;
  int subdivideGridY;
  int renderPassTag;
  Shader::Hint::Value hints;
  std::string name;
};

/**
 * Transform data
 */
struct Transform
{
  Transform();

  void SetPropertyMap(const Property::Map& propertyMap);
  void CreatePropertyMap(Property::Map& map) const;
  void UpdatePropertyMap(const Property::Map& propertyMap);

  void RegisterUniforms(Renderer renderer, UI::Visual::Transform::Policy::Type offsetXPolicy,
                        UI::Visual::Transform::Policy::Type offsetYPolicy,
                        UI::Visual::Transform::Policy::Type sizeXPolicy,
                        UI::Visual::Transform::Policy::Type sizeYPolicy);

  void SetUniforms(Renderer renderer, UI::Direction::Type direction);

  Vector2 mOffset;
  Vector2 mSize;
  Vector2 mExtraSize;
  Vector4 mOffsetSizeMode;
  Align::Type mOrigin;
  Align::Type mAnchorPoint;
};

/**
 * Corner radius data
 */
struct CornerRadiusData
{
  CornerRadiusData()
    : cornerRadius(Vector4::ZERO),
      cornerSquareness(Vector4::ZERO),
      cornerRadiusPolicy(static_cast<int>(UI::Visual::Transform::Policy::ABSOLUTE))
  {
  }

  Vector4 cornerRadius;
  Vector4 cornerSquareness;
  int cornerRadiusPolicy;
};

/**
 * Borderline data
 */
struct BorderlineData
{
  BorderlineData()
    : borderlineWidth(0.0f),
      borderlineColor(Color::BLACK),
      borderlineOffset(0.0f)
  {
  }

  float borderlineWidth;
  Vector4 borderlineColor;
  float borderlineOffset;
};

/**
 * Decoration data (corner radius + borderline combined)
 */
struct DecorationData
{
  CornerRadiusData cornerRadius;
  BorderlineData borderline;
};

/**
 * Internal data of Visual::Base
 * Stripped of transition-related data
 */
struct Base::Impl
{
  enum Flags
  {
    IS_ON_SCENE = 1 << 0,
    IS_PREMULTIPLIED_ALPHA = 1 << 1,
    IS_SYNCHRONOUS_RESOURCE_LOADING = 1 << 2,
  };

  Impl(FittingMode fittingMode, UI::Visual::Type type);
  ~Impl();

  std::string mName;
  Transform mTransform;
  Vector4 mMixColor;
  Size mControlSize;
  float mBorderlineWidth;
  float mBorderlineOffset;
  Vector4 mBorderlineColor;
  int mDepthIndex;
  Renderer mRenderer;

  FittingMode mFittingMode;
  UI::Visual::Type mType;
  UI::Visual::ResourceStatus mResourceStatus;

  std::unique_ptr<CustomShader> mCustomShader;
  std::unique_ptr<DecorationData> mDecorationData;

  Visual::EventObserver* mEventObserver;
  Visual::ConstraintObserver* mConstraintObserver;

  int mFlags;

  bool IsOnScene() const
  {
    return mFlags & IS_ON_SCENE;
  }
  bool IsPremultipliedAlpha() const
  {
    return mFlags & IS_PREMULTIPLIED_ALPHA;
  }
};

} // namespace Visual
} // namespace Internal
} // namespace UI
} // namespace Dali

#endif // DALI_UI_INTERNAL_VISUAL_BASE_DATA_IMPL_H
