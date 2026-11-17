#pragma once

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
// INTERNAL INCLUDES
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/object/query-interface-types.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
/**
 * @brief Provides text access independently of the concrete component type.
 *
 * Obtain this interface with Dali::QueryInterface<ITextContent>(). Calls must
 * follow the owning component's event-thread rules. Published virtual slots
 * are frozen; introduce a new interface contract to extend this API.
 *
 * @SINCE_3_0.2
 */
class DALI_QUERY_INTERFACE ITextContent
{
public:
  /**
   * @brief Sets the component's UTF-8 text content.
   *
   * @SINCE_3_0.2
   * @param[in] text The text to set; an empty string clears the content
   */
  virtual void SetText(const Dali::String& text) = 0;

  /**
   * @brief Gets a copy of the component's current text content.
   *
   * @SINCE_3_0.2
   * @return The UTF-8 text, which may be empty
   */
  virtual Dali::String GetText() const = 0;

protected:
  /**
   * @brief Destroys the interface as part of its owning implementation.
   *
   * @SINCE_3_0.2
   * @note Release the InterfacePtr instead of deleting an interface pointer.
   */
  ~ITextContent() = default;
};

} //namespace Ui
} //namespace DALI_NAMESPACE
#ifndef DALI_QUERY_INTERFACE_GENERATOR
#include <dali-ui-foundation/public-api/interfaces/text-content.generated.h>
#endif
