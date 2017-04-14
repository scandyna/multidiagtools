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
#ifndef MDT_ITEM_EDITOR_CURRENT_ROW_TO_BE_SET_SPY_H
#define MDT_ITEM_EDITOR_CURRENT_ROW_TO_BE_SET_SPY_H

#include <QObject>

namespace Mdt{ namespace ItemEditor{

  class RowChangeEventDispatcher;
  class AbstractController;

}} // namespace Mdt{ namespace ItemEditor{

class CurrentRowToBeSetSpy : public QObject
{
 Q_OBJECT

 public:

  explicit CurrentRowToBeSetSpy(const Mdt::ItemEditor::RowChangeEventDispatcher & dispatcher, QObject* parent = 0);
  explicit CurrentRowToBeSetSpy(const Mdt::ItemEditor::AbstractController & controller, QObject* parent = 0);

  int count() const
  {
    return mCount;
  }

  int takeCurrentRow();

  void clear();

 private slots:
 
  void setCurrentRow(int row);

 private:

  int mCount = 0;
  int mCurrentRow = -2;
};

#endif // #ifndef MDT_ITEM_EDITOR_CURRENT_ROW_TO_BE_SET_SPY_H
