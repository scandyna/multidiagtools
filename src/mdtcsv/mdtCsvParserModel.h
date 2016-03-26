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
#ifndef MDT_CSV_PARSER_MODEL_H
#define MDT_CSV_PARSER_MODEL_H

#include "mdtCsvData.h"
#include <QAbstractTableModel>
#include <QModelIndex>
#include <QVariant>

/*! \brief Item model to display data while parsing CSV
 */
class mdtCsvParserModel : public QAbstractTableModel
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtCsvParserModel(QObject *parent = nullptr);

  // Copy disabled
  mdtCsvParserModel(const mdtCsvParserModel &) = delete;
  mdtCsvParserModel & operator=(const mdtCsvParserModel &) = delete;

  // Move disabled
  mdtCsvParserModel(mdtCsvParserModel &&) = delete;
  mdtCsvParserModel & operator=(mdtCsvParserModel &&) = delete;

  /*! \brief Get row count
   */
  int rowCount(const QModelIndex & parent = QModelIndex()) const;

  /*! \brief Get column count
   */
  int columnCount(const QModelIndex & parent = QModelIndex()) const;

  /*! \brief Get header data
   */
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  /*! \brief Get data
   */
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

 protected:

  /*! \brief Set header
   */
  void setHeader(const mdtCsvRecord & header);

  /*! \brief Add a record
   */
  void addRecord(const mdtCsvRecord & record);

  /*! \brief Clear cache
   *
   * Will clear header and data cache
   */
  void clearCache();

 private:

  /*! \brief Add a column
   */
  void addColumn();

  mdtCsvRecord pvHeader;
  mdtCsvData pvBuffer;
};

#endif // #ifndef MDT_CSV_PARSER_MODEL_H
