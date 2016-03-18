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
#ifndef MDT_SQL_COPIER_COPY_HELPER_H
#define MDT_SQL_COPIER_COPY_HELPER_H

#include "TableMapping.h"
#include "UniqueInsertCriteria.h"
#include "TableMappingItem.h"
#include "RelatedTableInsertExpression.h"
#include "mdtExpected.h"
#include "mdt/sql/error/Error.h"
#include "mdtError.h"
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QVector>
#include <QObject>
// #include <iterator>

#include <QDebug>

namespace mdt{ namespace sql{ namespace copier{

  /*! \internal Field name list container
   */
  struct CopyHelperFieldNameLists
  {
    QStringList rtiExpFieldNameList;
    QStringList fieldMapFieldNameList;
    QStringList fixedValueFieldNameList;
  };

  /*! \brief Helper class for data copy
   */
  class CopyHelper
  {
   public:

    /*! \brief Get SQL to count number of records existing in source table
     */
    static QString getSourceTableCountSql(const TableMapping * const tm, const QSqlDatabase & sourceDb);

    /*! \brief Get SQL to select data in source table
     */
    static QString getSourceTableSelectSql(const TableMapping * const tm, const QSqlDatabase & sourceDb);

    /*! \brief Get SQL to count number of records existing in destination table regarding criteria
     *
     * \pre matchItemList must not be empty
     * \pre destinationDb must be valid and open
     */
    static QString getUniqueInsertCriteriaSql(const TableMapping * const tm, const std::vector<ExpressionMatchItem> & matchItemList, const QSqlDatabase & destinationDb);

    /*! \brief Check if given record exists in destination table regarding criteria
     *
     * \pre tm's unique insert criteria must not be null
     * \pre destinationDb must be valid and open
     */
    template<typename T>
    static mdtExpected<bool> checkExistsInDestinationTable(const T & sourceRecord, const TableMapping * const tm, const QSqlDatabase & destinationDb)
    {
      Q_ASSERT(tm);
      Q_ASSERT(destinationDb.isOpen());
      auto uic = tm->uniqueInsertCriteria();
      Q_ASSERT(!uic.isNull());
      const auto matchItemList = uic.matchItems();
      QSqlQuery query(destinationDb);

      // Prepare query
      if(!query.prepare(getUniqueInsertCriteriaSql(tm, matchItemList, destinationDb))){
        auto error = mdtErrorNew(tr("Preparation of query to check if record allready exists failed"), mdtError::Error, "CopyHelper");
        error.stackError(ErrorFromQsqlQuery(query));
        error.commit();
        return error;
      }
      // Bind match items values
      for(const auto & matchItem : matchItemList){
        const int fi = matchItem.sourceValueFieldIndex;
        Q_ASSERT(fi >= 0);
        Q_ASSERT(fi < sourceRecord.count());
        query.addBindValue(sourceRecord.value(fi));
      }
      // Exec query
      if(!query.exec()){
        auto error = mdtErrorNew(tr("Execution of query to check if record allready exists failed"), mdtError::Error, "CopyHelper");
        error.stackError(ErrorFromQsqlQuery(query));
        error.commit();
        return error;
      }
      // Fetch result
      if(!query.next()){
        auto error = mdtErrorNew(tr("Query to check if record allready exists did not return any result"), mdtError::Error, "CopyHelper");
        error.commit();
        return error;
      }

      return (query.value(0).toInt() > 0);
    }

    /*! \brief Get list of destination field names
     */
    static CopyHelperFieldNameLists getDestinationFieldNameLists(const TableMapping *const tm);

    /*! \brief Get SQL for insertion into destination table
     */
    static QString getDestinationInsertSql(const TableMapping * const tm, const QSqlDatabase & destinationDb);

    /*! \brief Get destination data for a field map item
     *
     * \tparam T Must provide following functions:
     *  - int count() const
     *  - int value(int index) const
     */
    template<typename T>
    static QVariant getFieldMapDestinationData(const T & sourceRecord, const TableMappingItem & tmi)
    {
      Q_ASSERT(tmi.sourceFieldIndexList().count() == 1);
      Q_ASSERT(tmi.sourceFieldIndexList().at(0) >= 0);
      Q_ASSERT(tmi.sourceFieldIndexList().at(0) < sourceRecord.count());

      return sourceRecord.value(tmi.sourceFieldIndexList().at(0));
    }

    /*! \brief Get SQL to fetch related table key record
     *
     * \pre destinationDb must be valid and open
     */
    static mdtExpected<QString> getSqlForRelatedTableInsertExpressionKey(const RelatedTableInsertExpression & exp, const QSqlDatabase & destinationDb);

    /*! \brief Get related table key record
     */
    template<typename T>
    static mdtExpected< QVector<QVariant> > getRelatedTableInsertExpressionKey(const T & sourceRecord, const RelatedTableInsertExpression & exp, const QSqlDatabase & destinationDb)
    {
      Q_ASSERT(destinationDb.isOpen());

      QVector<QVariant> keyRecord;
      QSqlQuery query(destinationDb);
      const auto matchItemList = exp.matchItems();

      // Get SQL and prepare query
      const auto sql = getSqlForRelatedTableInsertExpressionKey(exp, destinationDb);
      if(!sql){
        /// \todo Generate a error with explanation. Add transalation. This part should be in .cpp
        return sql.error();
      }
      if(!query.prepare(sql.value())){
        /// \todo Generate a error with explanation. Add transalation. This part should be in .cpp
        return ErrorFromQsqlQuery(query);
      }
      // Bind values for each match item
      for(const auto & matchItem : matchItemList){
        const int fi = matchItem.sourceValueFieldIndex;
        Q_ASSERT(fi >= 0);
        Q_ASSERT(fi < sourceRecord.count());
        query.addBindValue(sourceRecord.value(fi));
      }
      // Exec query and fetch key
      if(!query.exec()){
        auto error = mdtErrorNew(tr("Execution of query to get related key failed"), mdtError::Error, "CopyHelper");
        error.stackError(ErrorFromQsqlQuery(query));
        error.commit();
        return error;
      }
      if(!query.next()){
        QString text = tr("No matching key found in related table.") \
                     + tr(" SQL for request ist:\n") \
                     + query.executedQuery();
        auto error = mdtErrorNew(text, mdtError::Error, "CopyHelper");
        error.commit();
        return error;
      }
      auto sqlRecord = query.record();
      keyRecord.reserve(sqlRecord.count());
      for(int i = 0; i < sqlRecord.count(); ++i){
        keyRecord.append(sqlRecord.value(i));
      }

      return keyRecord;
    }

    /*! \brief Get destination data record
     *
     * \tparam T 
     */
    template<typename T>
    static mdtExpected< QVector<QVariant> > getDestinationRecord(const T & sourceRecord, const TableMapping * const tm, const QSqlDatabase & destinationDb)
    {
      Q_ASSERT(tm != nullptr);
      Q_ASSERT(destinationDb.isOpen());

      using mdt::sql::FieldIndexList;

      QVector<QVariant> record;
      QVector<QVariant> rtExpRecord;
      QVector<QVariant> fieldMapRecord;
      QVector<QVariant> fixedValueRecord;

      // Build partial records
      for(int itemIndex = 0; itemIndex < tm->itemsCount(); ++itemIndex){
        auto tmi = tm->itemAt(itemIndex);
        /// \todo Check mapping state ?
        if(!tmi.isNull()){
          switch(tmi.type()){
            case TableMappingItem::RelatedTableInsertExpressionType:
            {
              auto rec = getRelatedTableInsertExpressionKey(sourceRecord, tmi.relatedTableInsertExpression(), destinationDb);
              if(!rec){
                return rec.error();
              }
              for(const auto & v : rec.value()){
                rtExpRecord.append(v);
              }
              break;
            }
            case TableMappingItem::FieldMappingType:
              fieldMapRecord.append(getFieldMapDestinationData(sourceRecord, tmi));
              break;
            case TableMappingItem::FixedValueType:
              fixedValueRecord.append(tmi.fixedValue());
              break;
          }
        }
      }
      // Build destination record
      record.reserve(rtExpRecord.size() + fieldMapRecord.size() + fixedValueRecord.size());
      record = rtExpRecord + fieldMapRecord + fixedValueRecord;

      return record;
    }

   private:

    static QString tr(const char *sourceText)
    {
      return QObject::tr(sourceText);
    }
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_COPY_HELPER_H
