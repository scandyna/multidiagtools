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
#ifndef MDT_CSV_PARSER_FORMAT_SETUP_PROXY_MODEL_H
#define MDT_CSV_PARSER_FORMAT_SETUP_PROXY_MODEL_H

#include "RecordFormatter.h"
#include <QAbstractProxyModel>

namespace mdt{ namespace csv{

  /*! \brief Shifts rows down to let place for format selection
   */
  class ParserFormatSetupProxyModel : public QAbstractProxyModel
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     */
    ParserFormatSetupProxyModel(QObject *parent = nullptr);

    /*! \brief Set source model
     */
    void setSourceModel(QAbstractItemModel *sourceModel) override;

    /*! \brief Returns the index of the item in the model specified by the given row, column and parent index.
     */
    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const override;

    /*! \brief Returns the parent of the model item with the given index. If the item has no parent, an invalid QModelIndex is returned.
     */
    QModelIndex parent(const QModelIndex & index) const override;

    /*! \brief Get row count
     */
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;

    /*! \brief Get column count
     */
    int columnCount(const QModelIndex & parent = QModelIndex()) const override;

    /*! \brief Get header data
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /*! \brief Get data
     */
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

    /*! \brief Set data
     */
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;

    /*! \brief Get item flags
     */
    Qt::ItemFlags flags(const QModelIndex & index) const override;

    /*! \brief Map source index to proxy index
     */
    QModelIndex mapFromSource(const QModelIndex & sourceIndex) const override;

    /*! \brief Map proxy index to source model index
     */
    QModelIndex mapToSource(const QModelIndex & proxyIndex) const override;

    /*! \brief Map source selection to proxy selection
     */
//     QItemSelection mapSelectionFromSource(const QItemSelection & sourceSelection) const override;

    /*! \brief Map proxy selection to source selection
     */
//     QItemSelection mapSelectionToSource(const QItemSelection & proxySelection) const override;

   private slots:

    void onSourceModelAboutToBeReset();

    /*! \brief Called after source model reset
     */
    void onSourceModelReset();

    void onSourceColumnsAboutToBeInserted(const QModelIndex & parent, int first, int last);
    void onSourceColumnsAboutToBeRemoved(const QModelIndex & parent, int first, int last);
    void onSourceColumnsInserted(const QModelIndex & parent, int first, int last);
    void onSourceColumnsRemoved(const QModelIndex & parent, int first, int last);
    void onSourceDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight, const QVector<int> & roles = QVector<int> ());
    void onSourceHeaderDataChanged(Qt::Orientation orientation, int first, int last);
//     void onSourceLayoutAboutToBeChanged(const QList<QPersistentModelIndex> & parents = QList<QPersistentModelIndex> (),
//                                         QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::NoLayoutChangeHint);
//     void onSourceLayoutChanged(const QList<QPersistentModelIndex> & parents = QList<QPersistentModelIndex> (),
//                                QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::NoLayoutChangeHint);
    void onSourceRowsAboutToBeInserted(const QModelIndex & parent, int start, int end);
    void onSourceRowsAboutToBeRemoved(const QModelIndex & parent, int start, int end);
    void onSourceRowsInserted(const QModelIndex & parent, int start, int end);
    void onSourceRowsRemoved(const QModelIndex & parent, int start, int end);

   private:

    /*! \brief Get format data
     */
    QVariant fieldFormatData(int column, int role) const;

    /*! \brief Set field format
     */
    void setFieldType(int column, QMetaType::Type type);

    RecordFormatter pvRecordFormatter;
  };

}} // namespace mdt{ namespace csv{

#endif // #ifndef MDT_CSV_PARSER_FORMAT_SETUP_PROXY_MODEL_H
