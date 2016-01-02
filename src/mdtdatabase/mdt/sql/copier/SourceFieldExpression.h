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
#ifndef MDT_SQL_COPIER_SOURCE_FIELD_EXPRESSION_H
#define MDT_SQL_COPIER_SOURCE_FIELD_EXPRESSION_H

#include "AbstractSourceField.h"
#include "SourceFieldExpressionMatchItem.h"
#include <QVector>
#include <QString>
#include <QSqlDatabase>

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief Source field expresion
   */
  class SourceFieldExpression : public AbstractSourceField
  {
   public:

    /*! \brief Expression type
     */
    enum ExpressionType
    {
        /*!< Unique insertion into destination table.
             Value will be a SELECT statement ...... */
    };

    /*! \brief Construct a invalid expression
     */
    SourceFieldExpression()
     : AbstractSourceField()
    {
    }

    /*! \brief Polymorphic copy
     */
    SourceFieldExpression* clone() const override
    {
      return new SourceFieldExpression();
    }

    /*! \brief Check if source field is null
     */
    bool isNull() const override
    {
      return true;
    }

    /*! \brief Clear
     */
    void clear() override
    {
      
    }

    /*! \brief Set tables for a unique insert
     *
     * \param sourceTable Name of the source table
     * \param destinationTable Name of the destination table
     */
    void setTablesForUniqueInsert(const QString & sourceTable, const QString & destinationTable)
    {
      pvSourceTableName = sourceTable;
      pvReferringTableName = destinationTable;
    }

    /*! \brief Add a match item for unique insert
     */
    void addMatchItemForUniqueInsert(SourceFieldExpressionMatchItem::Operator operatorWithPrevious, const QString & sourceTableField, const QString & destinationTableField)
    {
      SourceFieldExpressionMatchItem item;
      item.operatorWithPrevious = operatorWithPrevious;
      item.sourceTableField = sourceTableField;
      item.referringTableField = destinationTableField;
      pvMatchItems.append(item);
    }

    void addMatchItemForUniqueInsert(const QString & sourceTableField, const QString & destinationTableField)
    {
      addMatchItemForUniqueInsert(SourceFieldExpressionMatchItem::None, sourceTableField, destinationTableField);
    }

    QString getSql(const QSqlDatabase & db, const QString & destinationTableField)
    {
      QString sql = "(SELECT " + destinationTableField + " FROM " + pvReferringTableName \
                  + " WHERE ";
      int lastIndex = pvMatchItems.size() - 0;
      for(int i = 0; i < lastIndex; ++i){
        auto item = pvMatchItems.at(i);
        sql += " " + item.operatorWithPreviousStr() + " " + item.referringTableField + " = " + item.sourceTableField;
      }
      sql += ")";
      return sql;
    }

    /// Dans get SQL on demande le nom du champ source

   private:

    QString pvSourceTableName;
    QString pvReferringTableName;
    QVector<SourceFieldExpressionMatchItem> pvMatchItems;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_SOURCE_FIELD_EXPRESSION_H

