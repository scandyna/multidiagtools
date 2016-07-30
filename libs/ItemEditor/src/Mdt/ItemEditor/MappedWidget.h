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
    MappedWidget(QWidget *widget, int column, bool hasReadOnlyProperty)
     : pvWidget(widget), pvColumn(column), pvHasReadOnlyProperty(hasReadOnlyProperty)
    {
      Q_ASSERT(pvWidget != nullptr);
      Q_ASSERT(column >= 0);
    }

    QWidget *widget() const
    {
      return pvWidget;
    }

    int column() const
    {
      return pvColumn;
    }

    bool hasReadOnlyProperty() const
    {
      return pvHasReadOnlyProperty;
    }

   private:

    QPointer<QWidget> pvWidget;
    int pvColumn;
    bool pvHasReadOnlyProperty;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_MAPPED_WIDGET_H
