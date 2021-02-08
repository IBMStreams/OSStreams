/*
 * Copyright 2021 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//----------------------------------------------------------------------------
//
// Title-
//       DistilleryServant.h
//
// Purpose-
//       Distillery Servant Class
//       This class provides all the services provided by
//       DistilleryServer, plus:
//         - support service start/stop
//
//----------------------------------------------------------------------------

#ifndef _DISTILLERYSERVANT_H_
#define _DISTILLERYSERVANT_H_

#include <UTILS/UTILSTypes.h>

UTILS_NAMESPACE_BEGIN

/**
   \internal

   This calls extends DistilleryServer to provide support for the
   creation of Distillery daemons
*/
class DistilleryServant
{
  protected:
    virtual ~DistilleryServant() {} // To silence compiler warning
    virtual bool isServerRunning(void) = 0;
    virtual void startServer(void) = 0;
    virtual void startDispatcher(void) = 0;
    virtual void stopDispatcher(void) = 0;
    virtual void stopServer(void) = 0;
};

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
