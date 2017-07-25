/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_MAIN_H
#define MDT_DEPLOY_UTILS_MAIN_H

#include "Mdt/AbstractConsoleApplicationMainFunction.h"
#include <QObject>

/*! \brief Provides the ability to run a console application with Qt event loop running
 */
class MdtDeployUtilsMain : public Mdt::AbstractConsoleApplicationMainFunction
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  explicit MdtDeployUtilsMain(QObject* parent = nullptr);

  ~MdtDeployUtilsMain();
  
  /*! \brief This is the real main of the console application
   */
  int runMain() override;
//   int mainImpl();

//  public slots:
// 
//   void run();
//   void cleanup();
};

#endif // #ifndef MDT_DEPLOY_UTILS_MAIN_H
