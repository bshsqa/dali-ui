#ifndef DALI_UI_INTERNAL_COLOR_VISUAL_H
#define DALI_UI_INTERNAL_COLOR_VISUAL_H

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
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/common/intrusive-ptr.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visuals/color-visual-properties-devel.h>
#include <dali-ui-foundation/internal/visuals/color/color-visual-shader-factory.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>

namespace Dali
{
namespace UI
{
namespace Internal
{

class ColorVisual;
typedef IntrusivePtr<ColorVisual> ColorVisualPtr;

class ColorVisual : public Visual::Base
{
public:
  static ColorVisualPtr New(VisualFactoryCache& factoryCache, ColorVisualShaderFactory& shaderFactory,
                            const Property::Map& properties);

public:
  void DoCreatePropertyMap(Property::Map& map) const override;
  void DoCreateInstancePropertyMap(Property::Map& map) const override;

protected:
  ColorVisual(VisualFactoryCache& factoryCache, ColorVisualShaderFactory& shaderFactory);
  ~ColorVisual() override;

  void DoSetProperties(const Property::Map& propertyMap) override;
  void DoSetOnScene(Actor& actor) override;
  void DoSetOffScene(Actor& actor) override;
  void OnSetTransform() override;
  void UpdateShader() override;
  Shader GenerateShader() const override;

protected:
  bool IsBlurRequired() const;
  bool IsCutoutRequired() const;

private:
  ColorVisual(const ColorVisual&) = delete;
  ColorVisual& operator=(const ColorVisual&) = delete;

private:
  float mBlurRadius;

  DevelColorVisual::CutoutPolicy::Type mCutoutPolicy;
  bool mAlwaysUsingBlurRadius;
  ColorVisualShaderFactory& mColorVisualShaderFactory;
};

} // namespace Internal
} // namespace UI
} // namespace Dali

#endif // DALI_UI_INTERNAL_COLOR_VISUAL_H
