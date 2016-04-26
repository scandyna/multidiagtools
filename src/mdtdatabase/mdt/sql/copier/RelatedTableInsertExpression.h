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
#ifndef MDT_SQL_COPIER_RELATED_TABLE_INSERT_EXPRESSION_H
#define MDT_SQL_COPIER_RELATED_TABLE_INSERT_EXPRESSION_H

#include "AbstractTableMappingItem.h"
#include "ExpressionMatchItem.h"
#include "mdt/sql/FieldIndexList.h"
#include <QString>
#include <vector>

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief Fetch value in related table in SQL copier
   *
   * For example, say we have to import a CSV file
   *  containing clients with their addresses:
   * <table>
   *  <tr>
   *   <th>ClientName</th>
   *   <th>Address</th>
   *  </tr>
   *  <tr>
   *   <td>Client 1</td>
   *   <td>Street 11</td>
   *  </tr>
   *  <tr>
   *   <td>Client 2</td>
   *   <td>Street 21</td>
   *  </tr>
   * </table>
   *
   * In a database we have a table named Client_tbl:
   * <table>
   *  <tr>
   *   <th>Id_PK</th>
   *   <th>Name</th>
   *  </tr>
   *  <tr>
   *   <td>1</td>
   *   <td>Some client A</td>
   *  </tr>
   *  <tr>
   *   <td>2</td>
   *   <td>Some client B</td>
   *  </tr>
   * </table>
   *
   * We also have Address_tbl:
   * <table>
   *  <tr>
   *   <th>Id_PK</th>
   *   <th>Client_Id_FK</th>
   *   <th>Street</th>
   *  </tr>
   *  <tr>
   *   <td>9</td>
   *   <td>1</td>
   *   <td>A street of some client A</td>
   *  </tr>
   *  <tr>
   *   <td>10</td>
   *   <td>2</td>
   *   <td>A street of some client B</td>
   *  </tr>
   * </table>
   *
   * First, we import client part into Client_tbl:
   * <table>
   *  <tr>
   *   <th>Id_PK</th>
   *   <th>Name</th>
   *  </tr>
   *  <tr>
   *   <td>1</td>
   *   <td>Some client A</td>
   *  </tr>
   *  <tr>
   *   <td>2</td>
   *   <td>Some client B</td>
   *  </tr>
   *  <tr>
   *   <td>3</td>
   *   <td>Client 1</td>
   *  </tr>
   *  <tr>
   *   <td>4</td>
   *   <td>Client 2</td>
   *  </tr>
   * </table>
   *
   * Then, we import address part into Address_tbl.
   *  During this process, each address must be related
   *  to the correct client.
   *  After import, Address_tbl looks like:
   * <table>
   *  <tr>
   *   <th>Id_PK</th>
   *   <th>Client_Id_FK</th>
   *   <th>Street</th>
   *  </tr>
   *  <tr>
   *   <td>9</td>
   *   <td>1</td>
   *   <td>A street of some client A</td>
   *  </tr>
   *  <tr>
   *   <td>10</td>
   *   <td>2</td>
   *   <td>A street of some client B</td>
   *  </tr>
   *  <tr>
   *   <td>11</td>
   *   <td>3</td>
   *   <td>Street 11</td>
   *  </tr>
   *  <tr>
   *   <td>12</td>
   *   <td>4</td>
   *   <td>Street 21</td>
   *  </tr>
   * </table>
   *
   * To match the correct client while importing addresses into Address_tbl,
   *  RelatedTableInsertExpression becomes useful.
   *
   * \code
   * RelatedTableInsertExpression rtExp;
   * std::vector<ExpressionMatchItem> matchItems;
   *
   * // Set field index of field to populate (here 1, Client_Id_FK)
   * rtExp.addDestinationFieldIndex(1);
   * // Set the related table in which the key must be fetched
   * rtExp.setDestinationRelatedTableName("Client_tbl");
   * // Set the field index of the key to fetch (here 0, Id_PK in Client_tbl)
   * rtExp.addDestinationRelatedFieldIndex(0);
   * // We must match: Client_tbl.Name = CSV.ClientName
   * // First argument is source value field index (here 0, which is ClientName in CSV)
   * // Second argument is destination field index (here 1, which is Name in Client_tbl)
   * matchItems.emplace_back(0, 1);
   * rtExp.setMatchItems(matchItems);
   * \endcode
   */
  class RelatedTableInsertExpression : public AbstractTableMappingItem
  {
   public:

    /*! \brief Create a invalid expression
     */
    RelatedTableInsertExpression()
     : AbstractTableMappingItem()
    {
//       qDebug() << "C  RelatedTableInsertExpression::RelatedTableInsertExpression() - ref: " << ref.load();
    }

    /*! \brief Destructor
     */
    ~RelatedTableInsertExpression()
    {
//       qDebug() << "D  RelatedTableInsertExpression::~RelatedTableInsertExpression() - ref: " << ref.load();
    }

    /*! \brief Copy constructor
     */
    RelatedTableInsertExpression(const RelatedTableInsertExpression & other)
     : AbstractTableMappingItem(other),
       pvDestinationRelatedTableName(other.pvDestinationRelatedTableName),
       pvDestinationRelatedTableKey(other.pvDestinationRelatedTableKey),
       pvMatchItems(other.pvMatchItems)
    {
//       qDebug() << "CPY  RelatedTableInsertExpression::RelatedTableInsertExpression(other) - ref: " << ref.load();
    }

    /*! \brief Copy assignment
     */
    RelatedTableInsertExpression & operator=(const RelatedTableInsertExpression & other)
    {
      if(&other != this){
//         qDebug() << "CPY  RelatedTableInsertExpression::operator=(other) - ref: " << ref.load();
        copyMembersOfAbstract(other);
        pvDestinationRelatedTableName = other.pvDestinationRelatedTableName;
        pvDestinationRelatedTableKey = other.pvDestinationRelatedTableKey;
        pvMatchItems = other.pvMatchItems;
      }
      return *this;
    }

    /*! \brief Polymorphic copy
     */
    RelatedTableInsertExpression* clone() const override
    {
//       qDebug() << " RelatedTableInsertExpression::clone() - ref: " << ref.load();
      return new RelatedTableInsertExpression(*this);
    }

    /*! \brief Add a destination field index
     */
    void addDestinationFieldIndex(int index)
    {
      Q_ASSERT(index >= 0);
      AbstractTableMappingItem::addDestinationFieldIndex(index);
    }

    /*! \brief Set destination field index list
     */
    void setDestinationFieldIndexList(const FieldIndexList & indexList)
    {
      AbstractTableMappingItem::setDestinationFieldIndexList(indexList);
    }

    /*! \brief Set destination related table name
     */
    void setDestinationRelatedTableName(const QString & tableName)
    {
      pvDestinationRelatedTableName = tableName;
    }

    /*! \brief Set destination related table name
     */
    QString destinationRelatedTableName() const
    {
      return pvDestinationRelatedTableName;
    }

    /*! \brief Add a field index to destination related table key
     */
    void addDestinationRelatedFieldIndex(int fieldIndex)
    {
      pvDestinationRelatedTableKey.append(fieldIndex);
    }

    /*! \brief Get destination related table key
     */
    FieldIndexList destinationRelatedTableKey() const
    {
      return pvDestinationRelatedTableKey;
    }

    /*! \brief Clear destination related table key
     */
    void clearDestinationRelatedTableKey()
    {
      pvDestinationRelatedTableKey.clear();
    }

    /*! \brief Get count of match items
     */
    int matchItemsCount() const
    {
      return pvMatchItems.size();
    }

    /*! \brief Set match items
     */
    void setMatchItems(const std::vector<ExpressionMatchItem> & items)
    {
      pvMatchItems = items;
    }

    /*! \brief Get match items
     */
    std::vector<ExpressionMatchItem> matchItems() const
    {
      return pvMatchItems;
    }

    /*! \brief Check if expression is null
     *
     * Expression is null if:
     *  - no destination field index was set
     *  - destination related table name was not set
     *  - destination related table key was not set
     *  - not match item was set or one of them is null
     */
    bool isNull() const override
    {
      return ( (destinationFieldIndexCount() < 1)
               || pvDestinationRelatedTableName.isEmpty()
               || pvDestinationRelatedTableKey.isEmpty()
               || pvMatchItems.empty()
               || oneMatchItemIsNull()
             );
    }

   private:

    /*! \brief Check if one match item is null
     */
    bool oneMatchItemIsNull() const
    {
      for(const auto & item : pvMatchItems){
        if( (item.sourceValueFieldIndex < 0) || (item.destinationFieldIndex < 0) ){
          return true;
        }
      }
      return false;
    }

    /*! \brief Clear expression
     */
    void clearItem() override
    {
      pvDestinationRelatedTableName.clear();
      pvDestinationRelatedTableKey.clear();
      pvMatchItems.clear();
    }

    QString pvDestinationRelatedTableName;
    FieldIndexList pvDestinationRelatedTableKey;
    std::vector<ExpressionMatchItem> pvMatchItems;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_RELATED_TABLE_INSERT_EXPRESSION_H
