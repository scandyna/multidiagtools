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
#ifndef MDT_ITEM_EDITOR_MAPPED_WIDGET_H
#define MDT_ITEM_EDITOR_MAPPED_WIDGET_H

#include <QPointer>
#include <QWidget>

namespace Mdt{ namespace ItemEditor{

  class MappedWidget
  {
   public:

    /*
     * Contructor
     */
    MappedWidget(QWidget *widget, int column)
     : mWidget(widget), mColumn(column)
    {
      Q_ASSERT(mWidget != nullptr);
      Q_ASSERT(column >= 0);
    }

    QWidget *widget() const
    {
      return mWidget;
    }

    int column() const
    {
      return mColumn;
    }

   private:

    QPointer<QWidget> mWidget;
    int mColumn;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_MAPPED_WIDGET_H
