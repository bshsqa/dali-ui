#ifndef DALI_UI_VISUALS_VISUAL_TRAIT_H
#define DALI_UI_VISUALS_VISUAL_TRAIT_H

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
#include <dali-ui-foundation/public-api/view.h>
#include <dali-ui-foundation/public-api/trait.h>
#include <dali/public-api/object/property-index.h>
#include <dali/public-api/object/property-map.h>

// INTERNAL INCLUDES
#include <dali-ui-visuals/public-api/dali-ui-visuals-api.h>
#include <dali-ui-visuals/devel-api/visual-factory/visual-base.h>

namespace Dali
{
namespace UI
{
namespace Internal
{
class VisualTraitImpl;
}

/**
 * @brief VisualTrait provides visual management capabilities to a View.
 *
 * It allows registering, unregistering, and retrieving Visuals associated with a View.
 * This class follows the Trait pattern, allowing View to remain lightweight while
 * offering powerful visual capabilities when needed.
 */
class DALI_UI_VISUALS_API VisualTrait : public Trait
{
public:
  /**
   * @brief Creates a new VisualTrait handle.
   *
   * @return A handle to a new VisualTrait
   */
  static VisualTrait New();

  /**
   * @brief Creates an empty handle.
   */
  VisualTrait() = default;

  /**
   * @brief Destructor.
   */
  ~VisualTrait() = default;

  /**
   * @brief Copy constructor.
   */
  VisualTrait(const VisualTrait& handle) = default;

  /**
   * @brief Move constructor.
   */
  VisualTrait(VisualTrait&& rhs) noexcept = default;

  /**
   * @brief Copy assignment operator.
   */
  VisualTrait& operator=(const VisualTrait& handle) = default;

  /**
   * @brief Move assignment operator.
   */
  VisualTrait& operator=(VisualTrait&& rhs) noexcept = default;

  /**
   * @brief Downcasts a handle to VisualTrait handle.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a VisualTrait or an uninitialized handle
   */
  static VisualTrait DownCast(BaseHandle handle);

public: // Type-Safe Accessor API (Static Helpers)

  /**
   * @brief Registers a visual to the view with the given property index.
   *
   * If the View does not have a VisualTrait attached, this method will automatically
   * create one and attach it.
   *
   * @param[in] view The view to register the visual to
   * @param[in] index The property index to associate with the visual
   * @param[in] properties The property map defining the visual
   */
  static void RegisterVisual(View& view, Property::Index index, const Property::Map& properties);

  /**
   * @brief Registers a pre-created visual to the view.
   *
   * @param[in] view The view to register the visual to
   * @param[in] index The property index
   * @param[in] visual The visual to register
   */
  static void RegisterVisual(View& view, Property::Index index, Toolkit::Visual::Base& visual);

  /**
   * @brief Unregisters the visual associated with the index.
   *
   * @param[in] view The view
   * @param[in] index The property index
   */
  static void UnregisterVisual(View& view, Property::Index index);

  /**
   * @brief Retrieves the visual associated with the index.
   *
   * @param[in] view The view
   * @param[in] index The property index
   * @return The visual handle, or empty if not found
   */
  static Toolkit::Visual::Base GetVisual(View& view, Property::Index index);

  /**
   * @brief Enables or disables the visual.
   *
   * @param[in] view The view
   * @param[in] index The property index
   * @param[in] enable True to enable, false to disable
   */
  static void EnableVisual(View& view, Property::Index index, bool enable);

  /**
   * @brief Checks if the visual is enabled.
   *
   * @param[in] view The view
   * @param[in] index The property index
   * @return True if enabled
   */
  static bool IsVisualEnabled(View& view, Property::Index index);

public:
  /**
   * @brief Internal constructor.
   */
  explicit VisualTrait(Internal::VisualTraitImpl* impl);
};

} // namespace UI
} // namespace Dali

#endif // DALI_UI_VISUALS_VISUAL_TRAIT_H
