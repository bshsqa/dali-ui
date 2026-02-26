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
 *
 * Note: The visual responds to the the Actor::COLOR by blending it with the 'Multiply' operator.
 */
class Base : public BaseObject
{
public:
  using FittingMode = DevelVisual::FittingMode;

  /**
   * Setting the properties of the visual, this API should only called by the VisualFactory
   * @param[in] propertyMap The properties for the requested Visual object.
   */
  void SetProperties(const Property::Map& propertyMap);

  /**
   * Set the visual on the actor (create renderers, etc)
   * @pre Impl->mGeometry must be created before this method is called
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
   * @param[in] mixColor The new mix color
   */
  void SetMixColor(const Vector4& color);

  /**
   * Get the mix color
   * @return The current mix color
   */
  const Vector4& GetMixColor() const;

  /**
   * Perform an action
   * @param[in] actionId The id of the action to perform
   * @param[in] attributes The list of attributes for the action
   */
  void DoAction(const Property::Index actionId, const Property::Value& attributes);

  /**
   * Perform an action extension
   * @param[in] actionId The id of the action to perform
   * @param[in] attributes The list of attributes for the action
   */
  void DoActionExtension(const Property::Index actionId, const Any& attributes);

  /**
   * Set the visual name
   * @param[in] name The name to give the visual
   */
  void SetName(const std::string& name);

  /**
   * Get the visual name
   * @return The name of the visual
   */
  const std::string& GetName() const;

  /**
   * Set the depth index of this visual
   * @param[in] index The depth index of this visual
   */
  void SetDepthIndex(int index);

  /**
   * Get the depth index of this visual
   * @return The depth index of this visual
   */
  int GetDepthIndex() const;

  /**
   * Enable or disable premultiplied alpha
   * @param[in] preMultiplied Whether alpha is pre-multiplied
   */
  void EnablePreMultipliedAlpha(bool preMultiplied);

  /**
   * Query whether alpha is pre-multiplied
   * @return True if alpha is pre-multiplied, false otherwise
   */
  bool IsPreMultipliedAlphaEnabled() const;

  /**
   * Get the natural size of the visual
   * @param[out] naturalSize The visual's natural size
   */
  virtual void GetNaturalSize(Vector2& naturalSize);

  /**
   * Get the height for a given width
   * @param[in] width Width to use
   * @return The height based on the width
   */
  float GetHeightForWidth(float width);

  /**
   * Get the width for a given height
   * @param[in] height Height to use
   * @return The width based on the height
   */
  float GetWidthForHeight(float height);

  /**
   * Set the transform and control size
   * @param[in] transform A property map describing the transform
   * @param[in] controlSize The size of the parent control for visuals that need to scale internally
   */
  void SetTransformAndSize(const Property::Map& transform, Size controlSize);

  /**
   * Get the type of this visual
   * @return The type of this visual
   */
  UI::Visual::Type GetType() const;

  /**
   * Retrieve the property object associated with the property key
   * @param[in] key The Property key of the visual
   * @return The Property object
   */
  Dali::Property GetPropertyObject(Dali::Property::Key key);

  /**
   * Check if the visual is on scene
   * @return True if this Visual is on the scene, false if it is off the scene
   */
  bool IsOnScene() const;

  /**
   * Check if the visual resources are ready
   * @return True if ready, false otherwise
   */
  bool IsResourceReady() const;

  /**
   * Get the resource status of the visual
   * @return The loading status of visual resource
   */
  UI::Visual::ResourceStatus GetResourceStatus() const;

  /**
   * Set the event observer
   * @param[in] observer The event observer to set
   */
  void SetEventObserver(Visual::EventObserver* observer);

  /**
   * Set the constraint observer
   * @param[in] observer The constraint observer to set
   */
  void SetConstraintObserver(Visual::ConstraintObserver* observer);

  /**
   * Get the fitting mode for the visual
   * @return The current fitting mode
   */
  FittingMode GetFittingMode() const;

  /**
   * Get the renderer
   * @return The currently staged renderer, or an empty handle if not staged
   */
  Renderer GetRenderer() const;

  /**
   * Check if the visual has decoration (borderline, corner radius)
   * @return True if the visual has decoration data
   */
  bool HasDecorationData() const;

protected:
  /**
   * Constructor
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] fittingMode The value that determines how the visual should be fit to the view
   * @param[in] type The type of this visual
   */
  Base(VisualFactoryCache& factoryCache, FittingMode fittingMode, UI::Visual::Type type);

  /**
   * Destructor
   * A reference counted object may only be deleted by calling Unreference().
   */
  ~Base() override;

  /**
   * Virtual methods for derived classes to set their properties
   * Called by SetProperties() allowing sub classes to set their properties
   * @param[in] propertyMap The properties for the requested Visual object
   */
  virtual void DoSetProperties(const Property::Map& propertyMap) = 0;

  /**
   * Called by SetOnScene() allowing sub classes to respond to the SetOnScene event
   * @note The derived class is required to create the renderer, and add it to the actor when all the resources are in
   * place
   * @param[in] actor The actor applying this visual
   */
  virtual void DoSetOnScene(Actor& actor) = 0;

  /**
   * Called by SetOffScene() allowing sub classes to respond to the SetOffScene event
   * @param[in] actor The actor applying this visual
   */
  virtual void DoSetOffScene(Actor& actor);

  /**
   * Called when transform or control size changes
   * (Of use to SVG and Text visuals)
   */
  virtual void OnSetTransform()
  {
  }

  /**
   * Called by CreatePropertyMap() allowing sub classes to respond to the CreatePropertyMap event
   * @param[out] map The visual property map
   */
  virtual void DoCreatePropertyMap(Property::Map& map) const = 0;

  /**
   * Called by CreateInstancePropertyMap() allowing derived classes to store instanced data
   * (separate to styled data) that needs copying between visuals on state change
   * @param[out] map The visual property map
   */
  virtual void DoCreateInstancePropertyMap(Property::Map& map) const = 0;

  /**
   * Called by DoAction() allowing sub classes to do the given action
   * @param[in] actionId The action to perform
   * @param[in] attributes The list of attributes for the action
   */
  virtual void OnDoAction(const Property::Index actionId, const Property::Value& attributes)
  {
  }

  /**
   * Called by DoActionExtension() allowing sub classes to do the given action
   * @param[in] actionId The action to perform
   * @param[in] attributes The list of attributes for the action
   */
  virtual void OnDoActionExtension(const Property::Index actionId, const Any& attributes)
  {
  }

  /**
   * Generate a shader by the current properties from each sub classes's own shader-generate logic
   * @return If the function defined, Generated shader by the current properties. Else, empty shader
   */
  virtual Shader GenerateShader() const;

  /**
   * Update the shader when some properties are changed
   */
  virtual void UpdateShader();

  /**
   * Notify resource ready to observer
   * Called when the visuals resources are loaded / ready
   * @param[in] resourceStatus The resource status
   */
  void ResourceReady(UI::Visual::ResourceStatus resourceStatus);

  /**
   * Initialise the renderer from the cache
   * If not available, create it and save it to the cache
   */
  void InitializeRenderer();

  /**
   * Register visual properties (mix color, etc)
   * Register the uniform on the DecoratedVisualRenderer and store the property index if necessary
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
