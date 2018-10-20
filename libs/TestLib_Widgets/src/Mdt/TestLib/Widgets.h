/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_TEST_LIB_WIDGETS_H
#define MDT_TEST_LIB_WIDGETS_H

#include "MdtTestLib_WidgetsExport.h"

class QComboBox;
class QAbstractItemView;
class QAbstractItemModel;
class QModelIndex;

namespace Mdt{ namespace TestLib{

  /*! \brief Check if a item represented by \a index is selected in \a view
   */
  bool MDT_TESTLIB_WIDGETS_EXPORT isItemSelected(QAbstractItemView & view, const QModelIndex & index);

  /*! \brief Simulate a mouse click on the item in \a view represented by \a index
   *
   * \pre \a view must have a valid item model
   * \pre \a index must be valid (which includes that it is in the range of the model in \a view)
   */
  void MDT_TESTLIB_WIDGETS_EXPORT mouseClickOnItem(QAbstractItemView & view, const QModelIndex & index);

  /*! \brief Simulate selecting a row of \a comboBox with the mouse
   *
   * \pre \a comboBox must have a model and a view
   * \pre \a row must be in valid range ( 0 <= \a row < comboBox.model()->rowCount() )
   */
  void MDT_TESTLIB_WIDGETS_EXPORT mouseSelectRow(QComboBox & comboBox, int row);

}} // namespace Mdt{ namespace TestLib{

#endif // #ifndef MDT_TEST_LIB_WIDGETS_H
