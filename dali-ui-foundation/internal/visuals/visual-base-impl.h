#ifndef DALI_UI_INTERNAL_VISUAL_BASE_IMPL_H
#define DALI_UI_INTERNAL_VISUAL_BASE_IMPL_H

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
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/rendering/renderer.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/devel-api/visuals/visual-properties-devel.h>
#include <dali-ui-foundation/internal/visuals/visual-constraint-observer.h>
#include <dali-ui-foundation/internal/visuals/visual-event-observer.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-cache.h>

namespace Dali
{
namespace UI
{
namespace Internal
{
namespace Visual
{

/**
 * Base class for all Visual implementations.
 * Stripped of transition/animation code for dali-ui.
 */
class Base : public BaseObject
{
public:
  using FittingMode = DevelVisual::FittingMode;

  /**
   * Setting the properties of the visual, this will
   * be called by the factory with the properties in the map
   */
  void SetProperties(const Property::Map& propertyMap);

  /**
   * Set the visual on the actor (create renderers, etc)
   */
  void SetOnScene(Actor& actor);

  /**
   * Remove the visual from the actor (cleanup)
   */
  void SetOffScene(Actor& actor);

  /**
   * Create the property map for this visual
   */
  void CreatePropertyMap(Property::Map& map) const;

  /**
   * Set the mix color (tint)
   */
  void SetMixColor(const Vector4& color);

  /**
   * Get the mix color
   */
  const Vector4& GetMixColor() const;

  /**
   * Perform an action
   */
  void DoAction(const Property::Index actionId, const Property::Value& attributes);
  void DoActionExtension(const Property::Index actionId, const Any& attributes);

  /**
   * Set/Get name
   */
  void SetName(const std::string& name);
  const std::string& GetName() const;

  /**
   * Set/Get depth index
   */
  void SetDepthIndex(int index);
  int GetDepthIndex() const;

  /**
   * Enable/disable premultiplied alpha
   */
  void EnablePreMultipliedAlpha(bool preMultiplied);
  bool IsPreMultipliedAlphaEnabled() const;

  /**
   * Natural size
   */
  virtual void GetNaturalSize(Vector2& naturalSize);
  float GetHeightForWidth(float width);
  float GetWidthForHeight(float height);

  /**
   * Transform
   */
  void SetTransformAndSize(const Property::Map& transform, Size controlSize);

  /**
   * Type
   */
  UI::Visual::Type GetType() const;

  /**
   * Property queries
   */
  Dali::Property GetPropertyObject(Dali::Property::Key key);

  /**
   * Is on scene
   */
  bool IsOnScene() const;

  /**
   * Resource status
   */
  bool IsResourceReady() const;
  UI::Visual::ResourceStatus GetResourceStatus() const;

  /**
   * Observer management
   */
  void SetEventObserver(Visual::EventObserver* observer);
  void SetConstraintObserver(Visual::ConstraintObserver* observer);

  /**
   * Fitting mode
   */
  FittingMode GetFittingMode() const;

  /**
   * Get the renderer
   */
  Renderer GetRenderer() const;

  /**
   * @brief Whether the visual has decoration (borderline, corner radius)
   */
  bool HasDecorationData() const;

protected:
  /**
   * Constructor
   */
  Base(VisualFactoryCache& factoryCache, FittingMode fittingMode, UI::Visual::Type type);

  /**
   * Destructor
   */
  ~Base() override;

  /**
   * Virtual methods for derived classes
   */
  virtual void DoSetProperties(const Property::Map& propertyMap) = 0;
  virtual void DoSetOnScene(Actor& actor) = 0;
  virtual void DoSetOffScene(Actor& actor);
  virtual void OnSetTransform()
  {
  }
  virtual void DoCreatePropertyMap(Property::Map& map) const = 0;
  virtual void DoCreateInstancePropertyMap(Property::Map& map) const = 0;
  virtual void OnDoAction(const Property::Index actionId, const Property::Value& attributes)
  {
  }
  virtual void OnDoActionExtension(const Property::Index actionId, const Any& attributes)
  {
  }
  virtual Shader GenerateShader() const;
  virtual void UpdateShader();

  /**
   * @brief Notify resource ready to observer
   */
  void ResourceReady(UI::Visual::ResourceStatus resourceStatus);

  /**
   * @brief Initialise the renderer from the cache,
   * if not available, create it and save it to the cache.
   */
  void InitializeRenderer();

  /**
   * Register visual properties (mix color, etc)
   */
  void RegisterDecoration();

protected:
  struct Impl;
  Impl* mImpl;
  VisualFactoryCache& mFactoryCache;
};

} // namespace Visual

// Helpers for public-api forwarding
inline Visual::Base& GetImplementation(UI::Visual::Base& handle)
{
  DALI_ASSERT_ALWAYS(handle && "Visual handle is empty");
  BaseObject& object = handle.GetBaseObject();
  return static_cast<Visual::Base&>(object);
}

inline const Visual::Base& GetImplementation(const UI::Visual::Base& handle)
{
  DALI_ASSERT_ALWAYS(handle && "Visual handle is empty");
  const BaseObject& object = handle.GetBaseObject();
  return static_cast<const Visual::Base&>(object);
}

} // namespace Internal
} // namespace UI
} // namespace Dali

#endif // DALI_UI_INTERNAL_VISUAL_BASE_IMPL_H
