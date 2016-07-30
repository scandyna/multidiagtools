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
#ifndef MDT_ITEM_EDITOR_TEST_TABLE_MODEL_H
#define MDT_ITEM_EDITOR_TEST_TABLE_MODEL_H

#include <QAbstractTableModel>
#include <vector>

/*
 * TestTableModel storage
 */
enum class TestTableModelRoleStorage
{
  GroupDisplayAndEditRoleData,
  SeparateDisplayAndEditRoleData
};

/*
 * Item data for TestTableModel
 */
class TestTableModelItemData
{
 public:

  TestTableModelItemData(TestTableModelRoleStorage s)
  {
    switch(s){
      case TestTableModelRoleStorage::GroupDisplayAndEditRoleData:
        pvEditRoleDataSeparate = false;
        break;
      case TestTableModelRoleStorage::SeparateDisplayAndEditRoleData:
        pvEditRoleDataSeparate = true;
        break;
    }
  }

  bool isEditRoleDataSeparate() const
  {
    return pvEditRoleDataSeparate;
  }

  void setDisplayRoleData(const QVariant & data)
  {
    pvDisplayRoleData = data;
  }

  QVariant displayRoleData() const
  {
    return pvDisplayRoleData;
  }

  void setEditRoleData(const QVariant & data)
  {
    if(pvEditRoleDataSeparate){
      pvEditRoleData = data;
    }else{
      pvDisplayRoleData = data;
    }
  }

  /*
   * If EditRoleDataSeparate is true,
   * returns data set by setEditRoleData(),
   * else it returns the same as displayRoleData()
   */
  QVariant editRoleData() const
  {
    if(pvEditRoleDataSeparate){
      return pvEditRoleData;
    }
    return pvDisplayRoleData;
  }

  /*
   * Set item enabled/disabled
   * Default is enabled
   */
  void setEnabled(bool enable)
  {
    pvIsEnabled = enable;
  }

  bool isEnabled() const
  {
    return pvIsEnabled;
  }

  /*
   * Set item editable/read only
   * Default id editable
   */
  void setEditable(bool editable)
  {
    pvIsEditable = editable;
  }

  bool isEditable() const
  {
    return pvIsEditable;
  }

 private:

  bool pvEditRoleDataSeparate;
  bool pvIsEditable = true;
  bool pvIsEnabled = true;
  QVariant pvDisplayRoleData;
  QVariant pvEditRoleData;
};

/*
 * Table model used for some tests
 */
class TestTableModel : public QAbstractTableModel
{
 public:

  TestTableModel(TestTableModelRoleStorage s = TestTableModelRoleStorage::GroupDisplayAndEditRoleData, QObject *parent = nullptr);

  int columnCount(const QModelIndex & parent = QModelIndex()) const override;

  int rowCount(const QModelIndex & parent = QModelIndex()) const override;

  /*
   * If storage is SeparateDisplayAndEditRoleData,
   * data for Qt::EditRole will be distinct from data for Qt::DisplayRole.
   */
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

  void setItemEnabled(const QModelIndex & index, bool enable);

  void setItemEditable(const QModelIndex & index, bool editable);

  Qt::ItemFlags flags(const QModelIndex & index) const override;

  /*
   * If storage is SeparateDisplayAndEditRoleData,
   * data for Qt::EditRole is stored separatly from data for Qt::DisplayRole
   */
  bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;

  /*
   * Populate model with data
   * Form is:
   * For row 0: 0A, 0B, ..., 0Z
   * For row 1: 1A, 1B, ..., 1Z
   */
  void populate(int rows, int columns);

  /*
   * Will clear all data.
   * ( rowCount() and columnCount() will return 0 after this call )
   */
  void clear();

 private:

  std::vector<TestTableModelItemData> getRowData(int currentRow);

  TestTableModelRoleStorage pvStorage;
  int pvColumnCount;
  std::vector< std::vector<TestTableModelItemData> > pvData;
};

#endif // #ifndef MDT_ITEM_EDITOR_TEST_TABLE_MODEL_H
