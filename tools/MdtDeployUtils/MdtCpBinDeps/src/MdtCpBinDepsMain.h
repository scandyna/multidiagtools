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
#ifndef MDT_CP_BIN_DEPS_MAIN_H
#define MDT_CP_BIN_DEPS_MAIN_H

#include "Mdt/AbstractConsoleApplicationMainFunction.h"

/*! \brief Provides the ability to run a console application with Qt event loop running
 */
class MdtCpBinDepsMain : public Mdt::AbstractConsoleApplicationMainFunction
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  explicit MdtCpBinDepsMain(QObject* parent = nullptr);

  /*! \brief This is the real main of the console application
   */
  int runMain() override;
};

#endif // #ifndef MDT_CP_BIN_DEPS_MAIN_H
