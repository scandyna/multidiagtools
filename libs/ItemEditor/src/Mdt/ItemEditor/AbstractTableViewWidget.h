/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_ITEM_EDITOR_ABSTRACT_TABLE_VIEW_WIDGET_H
#define MDT_ITEM_EDITOR_ABSTRACT_TABLE_VIEW_WIDGET_H

///#include "AbstractEditorWidget.h"
#include <QWidget>
#include <QPointer>

class QTableView;
class QAbstractItemModel;

namespace Mdt{ namespace ItemEditor{

  class TableViewController;

  /*! \brief Common base for QTableView based editor
   */
  class AbstractTableViewWidget : public QWidget
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit AbstractTableViewWidget(QWidget* parent = nullptr);

    /*! \brief Get view
     */
    QTableView *view() const
    {
      return mView;
    }

   protected:

    /*! \brief Set controller
     *
     * Subclass that owns its controller
     *  must tell it by calling this method.
     */
//     template<typename Controller>
//     void setController(Controller *controller)
//     {
//       Q_ASSERT(controller != nullptr);
//       controller->setView(mView);
//     }

   private:

    QTableView *mView;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ABSTRACT_TABLE_VIEW_WIDGET_H
