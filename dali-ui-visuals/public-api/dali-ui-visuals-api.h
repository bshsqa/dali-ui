#ifndef DALI_UI_VISUALS_API_H
#define DALI_UI_VISUALS_API_H

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

#ifdef BUILDING_DALI_UI_VISUALS
#define DALI_UI_VISUALS_API __attribute__ ((visibility ("default")))
#else
#define DALI_UI_VISUALS_API
#endif

#endif // DALI_UI_VISUALS_API_H
