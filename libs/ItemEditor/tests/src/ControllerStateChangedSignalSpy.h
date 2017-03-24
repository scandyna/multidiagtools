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
#ifndef MDT_ITEM_EDITOR_CONTROLLER_STATE_CHANGED_SIGNAL_SPY_H
#define MDT_ITEM_EDITOR_CONTROLLER_STATE_CHANGED_SIGNAL_SPY_H

#include "Mdt/ItemEditor/ControllerState.h"
#include <QVector>
#include <QtTest>
#include <QObject>

class ControllerStateChangedSignalSpy : public QObject
{
 Q_OBJECT

 public:

  template<typename T, typename PointerToMemberFunction>
  ControllerStateChangedSignalSpy(T *obj, PointerToMemberFunction signalFunction)
  {
    Q_ASSERT(obj != nullptr);
    connect(obj, signalFunction, this, &ControllerStateChangedSignalSpy::onStateChangedChanged);
  }

  int count() const
  {
    return mStateList.count();
  }
  
  Mdt::ItemEditor::ControllerState takeFirst()
  {
    Q_ASSERT(!mStateList.isEmpty());
    return mStateList.takeFirst();
  }

  void clear();

 private slots:

  void onStateChangedChanged(Mdt::ItemEditor::ControllerState state);

 private:

  QVector<Mdt::ItemEditor::ControllerState> mStateList;
};

#endif // #ifndef MDT_ITEM_EDITOR_CONTROLLER_STATE_CHANGED_SIGNAL_SPY_H
