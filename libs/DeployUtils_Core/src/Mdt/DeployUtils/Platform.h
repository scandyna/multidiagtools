/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_PLATFORM_H
#define MDT_DEPLOY_UTILS_PLATFORM_H

#include "OperatingSystem.h"
#include "Compiler.h"
#include "Processor.h"
#include "MdtDeployUtils_CoreExport.h"

namespace Mdt{ namespace DeployUtils{

  /*! \brief Definition of a platform
   *
   * \todo Currently, we have a very poor support.
   */
  class MDT_DEPLOYUTILS_CORE_EXPORT Platform
  {
   public:

    /*! \brief Construct a platform
     */
    Platform(OperatingSystem os = nativeOperatingSystem(), Compiler compiler = nativeCompiler(), Processor processor = nativeProcessor());

    /*! \brief Get operating system
     */
    OperatingSystem operatingSystem() const
    {
      return mOperatingSystem;
    }

    /*! \brief Get native operating system
     */
    static OperatingSystem nativeOperatingSystem();

    /*! \brief Get compiler
     */
    Compiler compiler() const
    {
      return mCompiler;
    }

    /*! \brief Get native compiler
     */
    static Compiler nativeCompiler();

    /*! \brief Get processor
     */
    Processor processor() const
    {
      return mProcessor;
    }

    /*! \brief Get native processor
     */
    static Processor nativeProcessor();

   private:

    OperatingSystem mOperatingSystem;
    Compiler mCompiler;
    Processor mProcessor;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_PLATFORM_H
