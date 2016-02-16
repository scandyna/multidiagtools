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
#ifndef MDT_SQL_COPIER_RELATED_TABLE_INSERT_MATCH_ITEM_MODEL_H
#define MDT_SQL_COPIER_RELATED_TABLE_INSERT_MATCH_ITEM_MODEL_H

#include "ExpressionMatchItemModel.h"
#include "RelatedTableInsertExpression.h"
#include "mdtSqlTableSchema.h"
#include "mdtExpected.h"
#include <QString>
#include <QStringList>

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief Model to access match items of related table insert expression
   */
  class RelatedTableInsertMatchItemModel : public ExpressionMatchItemModel
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    RelatedTableInsertMatchItemModel(const std::shared_ptr<const TableMapping> & tm, QObject *parent = nullptr);

    // Copy is disabled
    RelatedTableInsertMatchItemModel(const RelatedTableInsertMatchItemModel &) = delete;
    RelatedTableInsertMatchItemModel & operator=(const RelatedTableInsertMatchItemModel &) = delete;

    /*! \brief Set destination related table
     */
    mdtExpected<bool> setDestinationRelatedTable(const QString & tableName);

    /*! \brief Set expression
     *
     * Will set related source and destination tables
     *  and match items from given expression.
     *
     * \sa setSourceRelatedTable()
     * \sa setDestinationRelatedTable()
     */
    mdtExpected<bool> setExpression(const RelatedTableInsertExpression & exp);

    /*! \brief Clear
     */
    void clear();

    /*! \brief Get list of source related table fields
     */
    QStringList getSourceRelatedTableFieldNameList() const;

    /*! \brief Get list of destination related table fields
     */
    QStringList getDestinationRelatedTableFieldNameList() const;

    /*! \brief Get destination table name
     */
    QString destinationTableName() const override;

    /*! \brief Get destination field count
     */
    int destinationFieldCount() const override;

   private:

    /*! \brief Get destination field name
     */
    QString fetchDestinationFieldName(int fieldIndex) const override;

    /*! \brief Get field index of given field name in destination
     */
    int fetchDestinationFieldIndexOf(const QString & fieldName) const override;

    mdtSqlTableSchema pvDestinationRelatedTable;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_RELATED_TABLE_INSERT_MATCH_ITEM_MODEL_H
