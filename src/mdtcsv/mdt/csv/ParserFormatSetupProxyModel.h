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
    QItemSelection mapSelectionFromSource(const QItemSelection & sourceSelection) const override;

    /*! \brief Map proxy selection to source selection
     */
    QItemSelection mapSelectionToSource(const QItemSelection & proxySelection) const override;
  };

}} // namespace mdt{ namespace csv{

#endif // #ifndef MDT_CSV_PARSER_FORMAT_SETUP_PROXY_MODEL_H
