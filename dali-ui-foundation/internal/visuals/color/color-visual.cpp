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
#include <dali-ui-foundation/internal/visuals/color/color-visual.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/visuals/visual-base-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-string-constants.h>
#include <dali-ui-foundation/public-api/visuals/color-visual-properties.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace UI
{
namespace Internal
{

ColorVisualPtr ColorVisual::New(VisualFactoryCache& factoryCache, ColorVisualShaderFactory& shaderFactory,
                                const Property::Map& properties)
{
  ColorVisualPtr colorVisualPtr(new ColorVisual(factoryCache, shaderFactory));
  colorVisualPtr->SetProperties(properties);
  colorVisualPtr->ResourceReady(UI::Visual::ResourceStatus::READY);
  return colorVisualPtr;
}

ColorVisual::ColorVisual(VisualFactoryCache& factoryCache, ColorVisualShaderFactory& shaderFactory)
  : Visual::Base(factoryCache, DevelVisual::FittingMode::FILL, UI::Visual::COLOR),
    mBlurRadius(0.0f),
    mCutoutPolicy(DevelColorVisual::CutoutPolicy::NONE),
    mAlwaysUsingBlurRadius(false),
    mColorVisualShaderFactory(shaderFactory)
{
}

ColorVisual::~ColorVisual() = default;

void ColorVisual::DoSetProperties(const Property::Map& propertyMap)
{
  // Mix color (already handled by base class, but also check color-visual specific property)
  Property::Value* colorValue = propertyMap.Find(UI::ColorVisual::Property::MIX_COLOR);
  if (colorValue)
  {
    Vector4 color;
    if (colorValue->Get(color))
    {
      SetMixColor(color);
    }
  }

  // Blur radius
  Property::Value* blurValue = propertyMap.Find(DevelColorVisual::Property::BLUR_RADIUS);
  if (blurValue)
  {
    blurValue->Get(mBlurRadius);
  }

  // Cutout policy
  Property::Value* cutoutValue = propertyMap.Find(DevelColorVisual::Property::CUTOUT_POLICY);
  if (cutoutValue)
  {
    int cutout;
    if (cutoutValue->Get(cutout))
    {
      mCutoutPolicy = static_cast<DevelColorVisual::CutoutPolicy::Type>(cutout);
    }
  }
}

void ColorVisual::DoSetOnScene(Actor& actor)
{
  InitializeRenderer();

  // Register mix color
  if (mImpl->mRenderer)
  {
    mImpl->mRenderer.RegisterProperty(MIX_COLOR, Vector3(GetMixColor()));
    mImpl->mRenderer.SetProperty(DevelRenderer::Property::OPACITY, GetMixColor().a);

    // Register blur radius if needed
    if (IsBlurRequired())
    {
      mImpl->mRenderer.RegisterProperty(BLUR_RADIUS_NAME, mBlurRadius);
    }

    // Register decoration (corner radius, borderline)
    RegisterDecoration();

    actor.AddRenderer(mImpl->mRenderer);
  }

  ResourceReady(UI::Visual::ResourceStatus::READY);
}

void ColorVisual::DoSetOffScene(Actor& actor)
{
  if (mImpl->mRenderer)
  {
    actor.RemoveRenderer(mImpl->mRenderer);
  }
}

void ColorVisual::OnSetTransform()
{
  if (mImpl->mRenderer)
  {
    mImpl->mTransform.SetUniforms(mImpl->mRenderer, UI::Direction::LEFT_TO_RIGHT);
  }
}

void ColorVisual::DoCreatePropertyMap(Property::Map& map) const
{
  map.Insert(UI::ColorVisual::Property::MIX_COLOR, GetMixColor());
  if (mBlurRadius > 0.0f)
  {
    map.Insert(DevelColorVisual::Property::BLUR_RADIUS, mBlurRadius);
  }
  if (mCutoutPolicy != DevelColorVisual::CutoutPolicy::NONE)
  {
    map.Insert(DevelColorVisual::Property::CUTOUT_POLICY, static_cast<int>(mCutoutPolicy));
  }
}

void ColorVisual::DoCreateInstancePropertyMap(Property::Map& map) const
{
  // Nothing extra
}

Shader ColorVisual::GenerateShader() const
{
  bool roundedCorner = false;
  bool squircleCorner = false;
  bool borderline = false;
  bool blur = IsBlurRequired();
  bool cutout = IsCutoutRequired();

  if (HasDecorationData())
  {
    const auto& cr = mImpl->mDecorationData->cornerRadius;
    if (cr.cornerRadius != Vector4::ZERO)
    {
      if (cr.cornerSquareness != Vector4::ZERO)
      {
        squircleCorner = true;
      }
      else
      {
        roundedCorner = true;
      }
    }
    if (mImpl->mDecorationData->borderline.borderlineWidth > 0.0f)
    {
      borderline = true;
    }
  }

  VisualFactoryCache::ShaderType shaderType =
      ColorVisualShaderFactory::GetShaderType(roundedCorner, squircleCorner, borderline, blur, cutout);

  return mColorVisualShaderFactory.GetShader(mFactoryCache, shaderType);
}

void ColorVisual::UpdateShader()
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

bool ColorVisual::IsBlurRequired() const
{
  return mAlwaysUsingBlurRadius || mBlurRadius > 0.0f;
}

bool ColorVisual::IsCutoutRequired() const
{
  return mCutoutPolicy != DevelColorVisual::CutoutPolicy::NONE;
}

} // namespace Internal
} // namespace UI
} // namespace Dali
