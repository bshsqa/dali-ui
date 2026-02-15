/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
 */

#ifndef DALI_UI_TEXT_SELECTION_POPUP_H
#define DALI_UI_TEXT_SELECTION_POPUP_H

/**
 * @brief Stub header for TextSelectionPopup types.
 *
 * The actual popup UI has been removed from dali-ui-text.
 * This header preserves only the Buttons enum and related types
 * used internally by the text controller and decorator.
 */

namespace Dali
{
namespace UI
{

/**
 * @brief Minimal stub replacing the former TextSelectionPopup control.
 *
 * Only the Buttons enum flags are retained for use by the text controller.
 * No actual popup UI is created.
 */
class TextSelectionPopup
{
public:
  /**
   * @brief Popup button flag bits used by text-controller to track enabled operations.
   */
  enum Buttons
  {
    NONE      = 0,
    CUT       = 1u << 0,
    COPY      = 1u << 1,
    PASTE     = 1u << 2,
    SELECT    = 1u << 3,
    SELECT_ALL = 1u << 4,
    CLIPBOARD = 1u << 5,
  };
};

/**
 * @brief Stub callback interface — formerly used to receive popup button presses.
 */
class TextSelectionPopupCallbackInterface
{
public:
  virtual ~TextSelectionPopupCallbackInterface() = default;

  /**
   * @brief Called when a popup button is touched.
   * @param[in] button The button that was touched.
   */
  virtual void TextPopupButtonTouched(TextSelectionPopup::Buttons button) = 0;
};

} // namespace UI
} // namespace Dali

#endif // DALI_UI_TEXT_SELECTION_POPUP_H
