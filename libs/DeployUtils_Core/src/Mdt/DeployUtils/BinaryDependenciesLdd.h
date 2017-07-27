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
#ifndef MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_LDD_H
#define MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_LDD_H

#include "BinaryDependenciesImplementationInterface.h"

namespace Mdt{ namespace DeployUtils{

  /*! \brief
   */
  class BinaryDependenciesLdd : public BinaryDependenciesImplementationInterface
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    BinaryDependenciesLdd(QObject* parent = nullptr);

    /*! \brief Find dependencies for a executable or a library
     */
    bool findDependencies(const QString & binaryFilePath) override;

   private:

    
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_LDD_H
