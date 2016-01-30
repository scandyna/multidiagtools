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
#ifndef MDT_SQL_FIELD_INDEX_LIST_H
#define MDT_SQL_FIELD_INDEX_LIST_H

#include <QVector>
#include <QMetaType>
#include <algorithm>

//#include <QDebug>

namespace mdt{ namespace sql{

  /*! \brief List of field indexes used in database module
   */
  class FieldIndexList
  {
   public:

    typedef QVector<int>::value_type value_type;

//     FieldIndexList() = default;
//     FieldIndexList(const FieldIndexList &) = default;
//     FieldIndexList(FieldIndexList &&) = default;
// 
//     FieldIndexList(std::initializer_list<int> indexes)
//      : pvIndexes(indexes)
//     {
//     }

    FieldIndexList & operator=(const FieldIndexList &) = default;

    // Used by unit tests
    FieldIndexList & operator=(const QVector<int> & v)
    {
      pvIndexes = v;
      return *this;
    }

    /*! \brief Reserve memory for size elements
     */
    void reserve(int size)
    {
      pvIndexes.reserve(size);
    }

    /*! \brief Append a field index
     */
    void append(int fieldIndex)
    {
      pvIndexes.append(fieldIndex);
    }

    /*! \brief Clear field index list
     */
    void clear()
    {
      pvIndexes.clear();
    }

    /*! \brief Get count of field indexes
     */
    int count() const
    {
      return pvIndexes.count();
    }

    /*! \brief Check if index list is empty
     */
    bool isEmpty() const
    {
      return pvIndexes.isEmpty();
    }

    /*! \brief Get field index at i
     *
     * \pre i must be in valid range
     */
    int at(int i) const
    {
      Q_ASSERT(i >= 0);
      Q_ASSERT(i < pvIndexes.size());
      return pvIndexes.at(i);
    }

    /*! \brief Get STL-style begin iterator
     */
    QVector<int>::iterator begin()
    {
      return pvIndexes.begin();
    }

    /*! \brief Get STL-style begin iterator
     */
    QVector<int>::const_iterator begin() const
    {
      return pvIndexes.begin();
    }

    /*! \brief Get STL-style begin iterator
     */
    QVector<int>::const_iterator cbegin() const
    {
      return pvIndexes.cbegin();
    }

    /*! \brief Get STL-style end iterator
     */
    QVector<int>::iterator end()
    {
      return pvIndexes.end();
    }

    /*! \brief Get STL-style end iterator
     */
    QVector<int>::const_iterator end() const
    {
      return pvIndexes.end();
    }

    /*! \brief Get STL-style end iterator
     */
    QVector<int>::const_iterator cend() const
    {
      return pvIndexes.cend();
    }

    /*! \brief STL compatible append
     */
    void push_back(int index)
    {
      pvIndexes.push_back(index);
    }

    /*! \brief Check if a and b are equal
     *
     * \todo update once it is clear what is ==
     */
    friend
    bool operator==(const FieldIndexList & a, const FieldIndexList & b)
    {
      return std::equal(a.cbegin(), a.cend(), b.cbegin(), b.cend());
    }

    /*! \brief Check if a is < b
     */
    friend
    bool operator<(const FieldIndexList & a, const FieldIndexList & b)
    {
      auto itA = std::min_element(a.cbegin(), a.cend());
      auto itB = std::min_element(b.cbegin(), b.cend());

      // Case of a empty vector
      if( (itA == a.cend()) || (itB == b.cend()) ){
        return false;
      }
//       qDebug() << "minA: " << *itA << " , minB: " << *itB;

      return (*itA < *itB);
    }

    /*! \brief Check if a is <= b
     */
    friend
    bool operator<=(const FieldIndexList & a, const FieldIndexList & b)
    {
      if(a == b){
        return true;
      }
      return (a < b);
    }

   private:

    QVector<int> pvIndexes;
  };

}} // namespace mdt{ namespace sql{

Q_DECLARE_METATYPE(mdt::sql::FieldIndexList)

#endif // #ifndef MDT_SQL_FIELD_INDEX_LIST_H
