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
#ifndef MDT_ITEM_EDITOR_PRIMARY_KEY_CHANGED_SIGNAL_SPY_H
#define MDT_ITEM_EDITOR_PRIMARY_KEY_CHANGED_SIGNAL_SPY_H

#include "Mdt/ItemModel/PrimaryKey.h"
#include <QVector>
#include <QtTest>
#include <QObject>

/*! \brief Created du to a limitation using QSignalSpy
 *
 * When argument of a signal is in a namespace,
 *  QSignalSpy cannot find it.
 */
class PrimaryKeyChangedSignalSpy : public QObject
{
 Q_OBJECT

 public:

  template<typename T>
  PrimaryKeyChangedSignalSpy(T *obj)
  {
    Q_ASSERT(obj != nullptr);
    connect(obj, &T::primaryKeyChanged, this, &PrimaryKeyChangedSignalSpy::onPrimaryKeyChanged);
  }

  int count() const
  {
    return mPrimaryKeyList.count();
  }
  
  Mdt::ItemModel::PrimaryKey takeFirst()
  {
    Q_ASSERT(!mPrimaryKeyList.isEmpty());
    return mPrimaryKeyList.takeFirst();
  }

  void clear();

 private slots:

  void onPrimaryKeyChanged(const Mdt::ItemModel::PrimaryKey & pk);

 private:

  QVector<Mdt::ItemModel::PrimaryKey> mPrimaryKeyList;
};

#endif // #ifndef MDT_ITEM_EDITOR_PRIMARY_KEY_CHANGED_SIGNAL_SPY_H
