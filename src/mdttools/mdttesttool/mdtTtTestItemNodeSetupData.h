/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#ifndef MDT_TT_TEST_ITEM_NODE_SETUP_DATA_H
#define MDT_TT_TEST_ITEM_NODE_SETUP_DATA_H

#include "mdtTtTestNodeSetupData.h"
#include <QList>
#include <QVariant>

/*! \brief Store setup data related to a test item
 */
class mdtTtTestItemNodeSetupData
{
 public:

  /*! \brief Constructor
   */
  mdtTtTestItemNodeSetupData();

  /*! \brief Clear data
   */
  void clear();

  /*! \brief Check if more step is available
   */
  inline bool hasMoreStep() const
  {
    return (pvCurrentStep < pvNodeSetupDataList.size());
  }

  /*! \brief Get next step
   */
  mdtTtTestNodeSetupData getNextStep();

  /*! \brief Get device identification for current step
   */
  QVariant deviceIdentification() const;

 private:

  int pvCurrentStep;
  QList<mdtTtTestNodeSetupData> pvNodeSetupDataList;
};

#endif // #ifndef MDT_TT_TEST_ITEM_NODE_SETUP_DATA_H
