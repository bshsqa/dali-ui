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
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/visuals/visual-base-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-string-constants.h>
#include <dali-ui-foundation/devel-api/controls/control-depth-index-ranges.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/rendering/shader.h>
#include <dali/public-api/rendering/geometry.h>

namespace Dali
{
namespace UI
{
namespace Internal
{
namespace Visual
{

Base::Base(VisualFactoryCache& factoryCache, FittingMode fittingMode, UI::Visual::Type type)
  : mImpl(new Impl(fittingMode, type)),
    mFactoryCache(factoryCache)
{
}

Base::~Base()
{
  delete mImpl;
}

void Base::SetProperties(const Property::Map& propertyMap)
{
  // Common properties
  Property::Value* customShaderValue = propertyMap.Find(UI::Visual::Property::SHADER);
  if (customShaderValue)
  {
    Property::Map shaderMap;
    if (customShaderValue->Get(shaderMap))
    {
      if (!mImpl->mCustomShader)
      {
        mImpl->mCustomShader = std::make_unique<CustomShader>();
      }
      mImpl->mCustomShader->SetPropertyMap(shaderMap);
    }
  }

  // Transform
  Property::Value* transformValue = propertyMap.Find(UI::Visual::Property::TRANSFORM);
  if (transformValue)
  {
    Property::Map transformMap;
    if (transformValue->Get(transformMap))
    {
      mImpl->mTransform.SetPropertyMap(transformMap);
    }
  }

  // Premultiplied alpha
  Property::Value* premultipliedAlphaValue = propertyMap.Find(UI::Visual::Property::PREMULTIPLIED_ALPHA);
  if (premultipliedAlphaValue)
  {
    bool premultiplied = false;
    if (premultipliedAlphaValue->Get(premultiplied))
    {
      EnablePreMultipliedAlpha(premultiplied);
    }
  }

  // Mix color
  Property::Value* mixColorValue = propertyMap.Find(UI::Visual::Property::MIX_COLOR);
  if (mixColorValue)
  {
    Vector4 color;
    if (mixColorValue->Get(color))
    {
      mImpl->mMixColor = color;
    }
    else
    {
      Vector3 color3;
      if (mixColorValue->Get(color3))
      {
        mImpl->mMixColor.r = color3.r;
        mImpl->mMixColor.g = color3.g;
        mImpl->mMixColor.b = color3.b;
      }
    }
  }

  // Opacity
  Property::Value* opacityValue = propertyMap.Find(UI::Visual::Property::OPACITY);
  if (opacityValue)
  {
    float opacity;
    if (opacityValue->Get(opacity))
    {
      mImpl->mMixColor.a = opacity;
    }
  }

  // Fitting mode
  Property::Value* fittingValue = propertyMap.Find(DevelVisual::Property::VISUAL_FITTING_MODE);
  if (fittingValue)
  {
    int fittingMode;
    if (fittingValue->Get(fittingMode))
    {
      mImpl->mFittingMode = static_cast<FittingMode>(fittingMode);
    }
  }

  // Corner radius
  Property::Value* cornerRadiusValue = propertyMap.Find(DevelVisual::Property::CORNER_RADIUS);
  if (cornerRadiusValue)
  {
    if (!mImpl->mDecorationData)
    {
      mImpl->mDecorationData = std::make_unique<DecorationData>();
    }

    float radius;
    if (cornerRadiusValue->Get(radius))
    {
      mImpl->mDecorationData->cornerRadius.cornerRadius = Vector4(radius, radius, radius, radius);
    }
    else
    {
      cornerRadiusValue->Get(mImpl->mDecorationData->cornerRadius.cornerRadius);
    }
  }

  // Corner radius policy
  Property::Value* cornerRadiusPolicyValue = propertyMap.Find(DevelVisual::Property::CORNER_RADIUS_POLICY);
  if (cornerRadiusPolicyValue)
  {
    if (!mImpl->mDecorationData)
    {
      mImpl->mDecorationData = std::make_unique<DecorationData>();
    }
    int policy;
    if (cornerRadiusPolicyValue->Get(policy))
    {
      mImpl->mDecorationData->cornerRadius.cornerRadiusPolicy = policy;
    }
  }

  // Corner squareness
  Property::Value* cornerSquarenessValue = propertyMap.Find(DevelVisual::Property::CORNER_SQUARENESS);
  if (cornerSquarenessValue)
  {
    if (!mImpl->mDecorationData)
    {
      mImpl->mDecorationData = std::make_unique<DecorationData>();
    }
    float squareness;
    if (cornerSquarenessValue->Get(squareness))
    {
      mImpl->mDecorationData->cornerRadius.cornerSquareness = Vector4(squareness, squareness, squareness, squareness);
    }
    else
    {
      cornerSquarenessValue->Get(mImpl->mDecorationData->cornerRadius.cornerSquareness);
    }
  }

  // Borderline width
  Property::Value* borderlineWidthValue = propertyMap.Find(DevelVisual::Property::BORDERLINE_WIDTH);
  if (borderlineWidthValue)
  {
    if (!mImpl->mDecorationData)
    {
      mImpl->mDecorationData = std::make_unique<DecorationData>();
    }
    borderlineWidthValue->Get(mImpl->mDecorationData->borderline.borderlineWidth);
  }

  // Borderline color
  Property::Value* borderlineColorValue = propertyMap.Find(DevelVisual::Property::BORDERLINE_COLOR);
  if (borderlineColorValue)
  {
    if (!mImpl->mDecorationData)
    {
      mImpl->mDecorationData = std::make_unique<DecorationData>();
    }
    borderlineColorValue->Get(mImpl->mDecorationData->borderline.borderlineColor);
  }

  // Borderline offset
  Property::Value* borderlineOffsetValue = propertyMap.Find(DevelVisual::Property::BORDERLINE_OFFSET);
  if (borderlineOffsetValue)
  {
    if (!mImpl->mDecorationData)
    {
      mImpl->mDecorationData = std::make_unique<DecorationData>();
    }
    borderlineOffsetValue->Get(mImpl->mDecorationData->borderline.borderlineOffset);
  }

  // Derived class properties
  DoSetProperties(propertyMap);
}

void Base::SetOnScene(Actor& actor)
{
  if (!mImpl->IsOnScene())
  {
    DoSetOnScene(actor);
    mImpl->mFlags |= Impl::IS_ON_SCENE;
  }
}

void Base::SetOffScene(Actor& actor)
{
  if (mImpl->IsOnScene())
  {
    DoSetOffScene(actor);
    mImpl->mFlags &= ~Impl::IS_ON_SCENE;
    mImpl->mRenderer.Reset();
  }
}

void Base::DoSetOffScene(Actor& actor)
{
  actor.RemoveRenderer(mImpl->mRenderer);
}

void Base::CreatePropertyMap(Property::Map& map) const
{
  // Common properties
  map.Insert(UI::Visual::Property::TYPE, static_cast<int>(mImpl->mType));

  if (mImpl->mCustomShader)
  {
    mImpl->mCustomShader->CreatePropertyMap(map);
  }

  // Transform
  Property::Map transformMap;
  mImpl->mTransform.CreatePropertyMap(transformMap);
  map.Insert(UI::Visual::Property::TRANSFORM, transformMap);

  // Premultiplied alpha
  map.Insert(UI::Visual::Property::PREMULTIPLIED_ALPHA, mImpl->IsPremultipliedAlpha());

  // Mix color
  map.Insert(UI::Visual::Property::MIX_COLOR, Vector3(mImpl->mMixColor));
  map.Insert(UI::Visual::Property::OPACITY, mImpl->mMixColor.a);

  // Fitting mode
  map.Insert(DevelVisual::Property::VISUAL_FITTING_MODE, static_cast<int>(mImpl->mFittingMode));

  // Decoration
  if (mImpl->mDecorationData)
  {
    map.Insert(DevelVisual::Property::CORNER_RADIUS, mImpl->mDecorationData->cornerRadius.cornerRadius);
    map.Insert(DevelVisual::Property::CORNER_RADIUS_POLICY, mImpl->mDecorationData->cornerRadius.cornerRadiusPolicy);
    map.Insert(DevelVisual::Property::CORNER_SQUARENESS, mImpl->mDecorationData->cornerRadius.cornerSquareness);
    map.Insert(DevelVisual::Property::BORDERLINE_WIDTH, mImpl->mDecorationData->borderline.borderlineWidth);
    map.Insert(DevelVisual::Property::BORDERLINE_COLOR, mImpl->mDecorationData->borderline.borderlineColor);
    map.Insert(DevelVisual::Property::BORDERLINE_OFFSET, mImpl->mDecorationData->borderline.borderlineOffset);
  }

  // Derived properties
  DoCreatePropertyMap(map);
}

void Base::SetMixColor(const Vector4& color)
{
  mImpl->mMixColor = color;

  if (mImpl->mRenderer)
  {
    mImpl->mRenderer.SetProperty(DevelRenderer::Property::OPACITY, color.a);
    mImpl->mRenderer.RegisterProperty(MIX_COLOR, Vector3(color));
  }
}

const Vector4& Base::GetMixColor() const
{
  return mImpl->mMixColor;
}

void Base::DoAction(const Property::Index actionId, const Property::Value& attributes)
{
  OnDoAction(actionId, attributes);
}

void Base::DoActionExtension(const Property::Index actionId, const Any& attributes)
{
  OnDoActionExtension(actionId, attributes);
}

void Base::SetName(const std::string& name)
{
  mImpl->mName = name;
}

const std::string& Base::GetName() const
{
  return mImpl->mName;
}

void Base::SetDepthIndex(int index)
{
  mImpl->mDepthIndex = index;
  if (mImpl->mRenderer)
  {
    mImpl->mRenderer.SetProperty(Dali::Renderer::Property::DEPTH_INDEX, mImpl->mDepthIndex);
  }
}

int Base::GetDepthIndex() const
{
  return mImpl->mDepthIndex;
}

void Base::EnablePreMultipliedAlpha(bool preMultiplied)
{
  if (preMultiplied)
  {
    mImpl->mFlags |= Impl::IS_PREMULTIPLIED_ALPHA;
  }
  else
  {
    mImpl->mFlags &= ~Impl::IS_PREMULTIPLIED_ALPHA;
  }

  if (mImpl->mRenderer)
  {
    mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, preMultiplied);
  }
}

bool Base::IsPreMultipliedAlphaEnabled() const
{
  return mImpl->IsPremultipliedAlpha();
}

void Base::GetNaturalSize(Vector2& naturalSize)
{
  naturalSize = Vector2::ZERO;
}

float Base::GetHeightForWidth(float width)
{
  return 0.0f;
}

float Base::GetWidthForHeight(float height)
{
  return 0.0f;
}

void Base::SetTransformAndSize(const Property::Map& transform, Size controlSize)
{
  mImpl->mControlSize = controlSize;
  mImpl->mTransform.UpdatePropertyMap(transform);

  OnSetTransform();
}

UI::Visual::Type Base::GetType() const
{
  return mImpl->mType;
}

Dali::Property Base::GetPropertyObject(Dali::Property::Key key)
{
  // Basic property object support
  if (mImpl->mRenderer)
  {
    Property::Index index = DevelRenderer::GetPropertyIndex(mImpl->mRenderer, key);
    if (index != Property::INVALID_INDEX)
    {
      return Dali::Property(mImpl->mRenderer, index);
    }
  }
  DALI_LOG_ERROR("Visual::Base::GetPropertyObject - key not found\n");
  return Dali::Property(mImpl->mRenderer, Property::INVALID_INDEX);
}

bool Base::IsOnScene() const
{
  return mImpl->IsOnScene();
}

bool Base::IsResourceReady() const
{
  return mImpl->mResourceStatus == UI::Visual::ResourceStatus::READY;
}

UI::Visual::ResourceStatus Base::GetResourceStatus() const
{
  return mImpl->mResourceStatus;
}

void Base::SetEventObserver(Visual::EventObserver* observer)
{
  mImpl->mEventObserver = observer;
}

void Base::SetConstraintObserver(Visual::ConstraintObserver* observer)
{
  mImpl->mConstraintObserver = observer;
}

Base::FittingMode Base::GetFittingMode() const
{
  return mImpl->mFittingMode;
}

Renderer Base::GetRenderer() const
{
  return mImpl->mRenderer;
}

bool Base::HasDecorationData() const
{
  return mImpl->mDecorationData != nullptr;
}

void Base::ResourceReady(UI::Visual::ResourceStatus resourceStatus)
{
  mImpl->mResourceStatus = resourceStatus;
  if (mImpl->mEventObserver)
  {
    mImpl->mEventObserver->ResourceReady(*this);
  }
}

void Base::InitializeRenderer()
{
  if (!mImpl->mRenderer)
  {
    Geometry geometry = mFactoryCache.GetGeometry(VisualFactoryCache::QUAD_GEOMETRY);
    if (!geometry)
    {
      geometry = VisualFactoryCache::CreateQuadGeometry();
      mFactoryCache.SaveGeometry(VisualFactoryCache::QUAD_GEOMETRY, geometry);
    }

    Shader shader = GenerateShader();
    mImpl->mRenderer = Renderer::New(geometry, shader);
    mImpl->mRenderer.SetProperty(Dali::Renderer::Property::DEPTH_INDEX, mImpl->mDepthIndex);
    mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, mImpl->IsPremultipliedAlpha());
  }
}

void Base::RegisterDecoration()
{
  if (mImpl->mRenderer && mImpl->mDecorationData)
  {
    const auto& cr = mImpl->mDecorationData->cornerRadius;
    if (cr.cornerRadius != Vector4::ZERO)
    {
      mImpl->mRenderer.RegisterProperty(CORNER_RADIUS, cr.cornerRadius);
      mImpl->mRenderer.RegisterProperty(CORNER_RADIUS_POLICY, static_cast<float>(cr.cornerRadiusPolicy));
    }
    if (cr.cornerSquareness != Vector4::ZERO)
    {
      mImpl->mRenderer.RegisterProperty(CORNER_SQUARENESS, cr.cornerSquareness);
    }

    const auto& bl = mImpl->mDecorationData->borderline;
    if (bl.borderlineWidth > 0.0f)
    {
      mImpl->mRenderer.RegisterProperty(BORDERLINE_WIDTH, bl.borderlineWidth);
      mImpl->mRenderer.RegisterProperty(BORDERLINE_COLOR, bl.borderlineColor);
      mImpl->mRenderer.RegisterProperty(BORDERLINE_OFFSET, bl.borderlineOffset);
    }
  }
}

Shader Base::GenerateShader() const
{
  // Default: simple shader (derived classes override)
  return Shader();
}

void Base::UpdateShader()
{
  if (mImpl->mRenderer)
  {
    Shader newShader = GenerateShader();
    if (newShader)
    {
      mImpl->mRenderer.SetShader(newShader);
    }
  }
}

} // namespace Visual
} // namespace Internal
} // namespace UI
} // namespace Dali
