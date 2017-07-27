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
#include "BinaryDependencies.h"

namespace Mdt{ namespace DeployUtils{

BinaryDependencies::BinaryDependencies(Mdt::DeployUtils::Platform targetPlatform, QObject* parent)
 : QObject(parent)
{
  Platform nativePlatform;

  /*
   * On Linux we have different choices:
   *  - ldd if target platform is also Linux
   *  - objdump if target platform is Windows (cross-compiled sw)
   */
  if(nativePlatform.operatingSystem() == OperatingSystem::Linux){
    if(targetPlatform.operatingSystem() == OperatingSystem::Linux){
      /// LDD
    }else if(targetPlatform.operatingSystem() == OperatingSystem::Windows){
      /// Objsump
    }
  }
  /// if null, Error (add a isValid() + assert in ather members, so the caller has a chance to log a error)
}


}} // namespace Mdt{ namespace DeployUtils{
