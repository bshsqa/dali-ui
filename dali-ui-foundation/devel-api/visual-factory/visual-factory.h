#ifndef DALI_UI_VISUAL_FACTORY_H
#define DALI_UI_VISUAL_FACTORY_H

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
#include <dali/public-api/images/image-operations.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/rendering/geometry.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visual-factory/visual-base.h>

namespace Dali
{
namespace UI
{
namespace Internal DALI_INTERNAL
{
class VisualFactory;
}

/**
 * @brief VisualFactory is a singleton object that provides and shares visuals between controls
 *
 * By setting environment variable 'DALI_DEBUG_RENDERING', a debug visual is used which renders a quad wireframe.
 *
 * The visual type is required in the property map for requesting a visual.
 *
 * | %Property Name           | Type              |
 * |--------------------------|-------------------|
 * | visualType               | INTEGER or STRING |
 * | shader                   | MAP               |
 */
class DALI_UI_API VisualFactory : public BaseHandle
{
public:
  enum CreationOptions
  {
    NONE = 0,

    IMAGE_VISUAL_LOAD_STATIC_IMAGES_ONLY = 1 << 0, ///< Load static images only when we use the image visual.
  };

  /**
   * @brief Create or retrieve VisualFactory singleton.
   *
   * @return A handle to the VisualFactory control.
   */
  static VisualFactory Get();

  /**
   * @brief Create a VisualFactory handle.
   *
   * Calling member functions with an uninitialised handle is not allowed.
   */
  VisualFactory();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~VisualFactory();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param[in] handle A reference to the copied handle.
   */
  VisualFactory(const VisualFactory& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] handle  A reference to the copied handle.
   * @return A reference to this.
   */
  VisualFactory& operator=(const VisualFactory& handle);

  /**
   * @brief Request the visual
   *
   * @param[in] propertyMap The map contains the properties required by the visual.
   *            The content of the map determines the type of visual that will be returned.
   * @return The handle to the created visual
   */
  Visual::Base CreateVisual(const Property::Map& propertyMap);

  /**
   * @brief Get the default quad geometry which general visuals be used.
   *
   * @return The default quad geometry for visual factory.
   */
  Dali::Geometry GetDefaultQuadGeometry();

  /**
   * @brief Enable or disable premultiplying alpha in images and image visuals.
   *
   * The default is to enable pre-multiplication on load.
   *
   * Applications that have assets with pre-multiplied alpha already applied should turn this option off.
   *
   * @param[in] preMultiply True if loaded images for image visuals should have alpha multiplied into the color
   * channels.
   */
  void SetPreMultiplyOnLoad(bool preMultiply);

  /**
   * @brief Get the setting for automatically pre-multiplying image visual images on load.
   *
   * @return True if loaded images have pre-multiplied alpha applied on load, false otherwise.
   */
  bool GetPreMultiplyOnLoad() const;

  /**
   * @brief Discard visual base. It will keep reference of visual until idle callback called.
   *
   * @param[in] visual Discarded visual base.
   */
  void DiscardVisual(Visual::Base visual);

private:
  explicit DALI_INTERNAL VisualFactory(Internal::VisualFactory* impl);
};

} // namespace UI
} // namespace Dali

#endif // DALI_UI_VISUAL_FACTORY_H
