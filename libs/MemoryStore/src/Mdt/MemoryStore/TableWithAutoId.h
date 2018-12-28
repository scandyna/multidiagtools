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
#ifndef MDT_MEMORY_STORE_TABLE_WITH_AUTO_ID_H
#define MDT_MEMORY_STORE_TABLE_WITH_AUTO_ID_H

#include "Table.h"
#include "Mdt/Error.h"
#include <QCoreApplication>
#include <QString>
#include <algorithm>

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
   *   Mdt::MemoryStore::TableWithAutoId<Person> mPersonTable;
   *   Mdt::MemoryStore::TableWithAutoId<Contact> mContactTable;
   * };
   * \endcode
   *
   * Implementing %add() could look like:
   * \code
   * Mdt::Expected<PersonId> MemoryPersonRepository::add(const Person & person)
   * {
   *   const int id = mPersonTable.add(person);
   *   for(const auto & contact : person.contacts()){
   *     const Contact newContact(PersonId(id), contact);
   *     mContactTable.add(newContact);
   *   }
   *
   *   return id;
   * }
   * \endcode
   */
  template<typename Record, typename UniqueId = int>
  class TableWithAutoId : public Table<Record, UniqueId>
  {
    Q_DECLARE_TR_FUNCTIONS(TableWithAutoId)

    using BaseClass = Table<Record, UniqueId>;
    using Map = typename BaseClass::Map;
    using BaseClass::containsPrimaryKey;

   public:

    /*! \brief Add a record to this table
     *
     * This method will create a new id
     *  and add the record to this table.
     *
     */
    UniqueId add(const Record & record)
    {
      const auto id = getNextId();
      Q_ASSERT(!containsPrimaryKey(id));

      BaseClass::add(record, id);

      return id;
    }

   private:

    using BaseClass::map;

    UniqueId getNextId() const noexcept
    {
      using KeyValuePair = typename Map::value_type;

      const auto cmp = [](const KeyValuePair & a, const KeyValuePair & b){
        return a.first < b.first;
      };
      const auto it = std::max_element(map().cbegin(), map().cend(), cmp);
      if(it == map().cend()){
        return 1;
      }

      return it->first + 1;
    }
  };

}} // namespace Mdt{ namespace MemoryStore{

#endif // MDT_MEMORY_STORE_TABLE_WITH_AUTO_ID_H
