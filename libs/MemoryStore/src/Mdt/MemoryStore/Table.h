/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_MEMORY_STORE_TABLE_H
#define MDT_MEMORY_STORE_TABLE_H

#include "Mdt/Error.h"
#include "Mdt/Expected.h"
#include <QCoreApplication>
#include <QString>
#include <map>
#include <vector>

namespace Mdt{ namespace MemoryStore{

  /*!\brief Helper class to implement a memory repository
   *
   * This class can be usefull to implement a memory repository
   *  for some unit tests.
   *
   * For example, let's use this person repository interface:
   * \code
   * class PersonRepository
   * {
   *  public:
   *
   *   virtual Mdt::Expected<PersonId> add(const Person & person) = 0;
   * };
   * \endcode
   *
   * While Person could be a aggregate root for %Person and %Contact,
   *  we could model the memory repository by using 2 tables:
   * \code
   * class MemoryPersonRepository
   * {
   *  public:
   *
   *   Mdt::Expected<PersonId> add(const Person & person) override;
   *
   *  private:
   *
   *   Mdt::MemoryStore::Table<Person, PersonId> mPersonTable;
   *   Mdt::MemoryStore::Table<Contact, ContactId> mContactTable;
   * };
   * \endcode
   *
   * \tparam PrimaryKey A key that can be comparable
   *   with the < operator:
   *   \code
   *   bool operator<(const MyPrimaryKey & a, const MyPrimaryKey & b);
   *   \endcode
   *
   * \sa TableWithAutoId
   */
  template<typename Record, typename PrimaryKey>
  class Table
  {
    Q_DECLARE_TR_FUNCTIONS(Table)

   public:

    /*! \brief Add a record to this table
     *
     * If \a pk allready exists in this table,
     *  this method returns false,
     *  otherwise the record is added and true is returned.
     */
    bool add(const Record & record, const PrimaryKey & pk)
    {
      if(containsPrimaryKey(pk)){
        const QString msg = tr("A record allready exists for given primary key");
        const auto error = mdtErrorNew(msg, Mdt::Error::Critical, "Mdt::MemoryStore::Table");
        setLastError(error);
        return false;
      }
      mMap.insert(std::make_pair(pk, record));

      return true;
    }

    /*! \brief Check if this table contains a primary key matching \a pk
     */
    bool containsPrimaryKey(const PrimaryKey & pk) const noexcept
    {
      return mMap.find(pk) != mMap.cend();
    }

    /*! \brief Get a record matching \a pk
     *
     * If \pk refers to a existing record in this table,
     *  a valid value containing the record is returned,
     *  otherwise a error.
     */
    Mdt::Expected<Record> getByPrimaryKey(const PrimaryKey & pk) const
    {
      const auto it = mMap.find(pk);
      if(it == mMap.cend()){
        const QString msg = tr("Could not find a record for given primary key");
        const auto error = mdtErrorNew(msg, Mdt::Error::Critical, "Mdt::MemoryStore::Table");
//         setLastError(error);
        return error;
      }

      return it->second;
    }

    /*! \brief Get all records
     */
    std::vector<Record> getAll() const
    {
      std::vector<Record> list;

      for(const auto & keyValuePair : mMap){
        list.emplace_back(keyValuePair.second);
      }

      return list;
    }

    /*! \brief Update a record in this table
     *
     * If \a pk exists in this table,
     *  the corresponding record will be updated and true is returned,
     *  otherwise false is returned.
     */
    bool update(const Record & record, const PrimaryKey & pk)
    {
      auto it = mMap.find(pk);
      if(it == mMap.end()){
        const QString msg = tr("Could not find a record for given primary key");
        const auto error = mdtErrorNew(msg, Mdt::Error::Critical, "Mdt::MemoryStore::Table");
        setLastError(error);
        return false;
      }
      it->second = record;

      return true;
    }

    /*! \brief Remove a record from this table
     */
    void remove(const PrimaryKey & pk)
    {
      mMap.erase(pk);
    }

    /*! \brief Remove all records from this table
     */
    void removeAll()
    {
      mMap.clear();
    }

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

   protected:

    using Map = std::map<PrimaryKey, Record>;

    const Map & map() const
    {
      return mMap;
    }

   private:

    void setLastError(const Mdt::Error & error)
    {
      Q_ASSERT(!error.isNull());

      mLastError = error;
    }

    Map mMap;
    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace MemoryStore{

#endif // MDT_MEMORY_STORE_TABLE_H
