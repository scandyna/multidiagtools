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
#ifndef MDT_PLAIN_TEXT_RECORD_LIST_TABLE_MODEL_H
#define MDT_PLAIN_TEXT_RECORD_LIST_TABLE_MODEL_H

#include "RecordList.h"
#include <QAbstractTableModel>
#include <QModelIndex>
#include <QVariant>

namespace Mdt{ namespace PlainText{

  /*! \brief RecordListTableModel is a table model to access data in a RecordList
   *
   * Typical usage:
   * \code
   * #include <Mdt/PlainText/RecordListTableModel>
   * #include <QTableView>
   *
   * using namespace Mdt::PlainText;
   *
   * // Build some list of records
   * RecordList recordList = {{1,"A"},{2,"B"}};
   * // Setup model and view
   * RecordListTableModel model;
   * model.setRecordList(recordList);
   * QTableView view;
   * view.setModel(&model);
   * \endcode
   */
  class RecordListTableModel : public QAbstractTableModel
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit RecordListTableModel(QObject* parent = nullptr);

    /*! \brief Get row count
     */
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;

    /*! \brief Get column count
     */
    int columnCount(const QModelIndex & parent = QModelIndex()) const override;

    /*! \brief Get data
     */
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

    /*! \brief Set record list
     *
     * Note that setting a new record list will reset the model.
     */
    void setRecordList(const RecordList & recordList);

   private:

    void updateColumnCount();

    int mColumnCount = 0;
    RecordList mRecordList;
  };

}} // namespace Mdt{ namespace PlainText{

#endif // #ifndef MDT_PLAIN_TEXT_RECORD_LIST_TABLE_MODEL_H
