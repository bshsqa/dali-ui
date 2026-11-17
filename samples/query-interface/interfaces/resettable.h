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
#include <dali/public-api/object/query-interface-types.h>

namespace QueryInterfaceSample
{
/**
 * @brief App-owned contract for restoring a component's default content.
 *
 * QueryInterface retains the component through its owning DALi handle. This
 * sample contract has no DALi platform version marker; the app owns its ABI.
 */
class DALI_QUERY_INTERFACE IResettable
{
public:
  /**
   * @brief Restores the component's default content.
   */
  virtual void ResetContent() = 0;

protected:
  /**
   * @brief Destroys the interface only as part of its owning implementation.
   */
  ~IResettable() = default;
};

} //namespace QueryInterfaceSample
#ifndef DALI_QUERY_INTERFACE_GENERATOR
#include "interfaces/resettable.generated.h"
#endif
