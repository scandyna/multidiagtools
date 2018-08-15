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
#ifndef MDT_ENTITY_MEMORY_ENTITY_REPOSITORY_H
#define MDT_ENTITY_MEMORY_ENTITY_REPOSITORY_H

#include "AbstractEntityRepository.h"
#include "Value.h"
#include "Mdt/Error.h"
#include <QMap>
#include <QString>

namespace Mdt{ namespace Entity{

  /*! \brief Memory implementation for AbstractEntityRepository
   *
   * This repository implementation can be used for unit tests.
   *
   * The simplest way to avoid repeating template arguments
   *  is to create a test class:
   * \code
   * class TestPersonRepository : public Mdt::Entity::MemoryEntityRepository<PersonData, PersonId>
   * {
   * };
   * \endcode
   *
   * If a interafce repository is used in the business code,
   *  the test class will look like tis:
   * \code
   * class TestPersonRepository : public PersonRepository, public Mdt::Entity::MemoryEntityRepository<PersonData, PersonId>
   * {
   * };
   * \endcode
   *
   * The repository could be instanciated with a shared pointer:
   * \code
   * auto personRepository = std::make_shared<TestPersonRepository>();
   * \endcode
   */
  template<typename EntityData, typename UniqueId>
  class MemoryEntityRepository : public virtual AbstractEntityRepository<EntityData, UniqueId>
  {
    using ParentClass = AbstractEntityRepository<EntityData, UniqueId>;
    using ParentClass::tr;

   public:

    /*! \brief Constructor
     */
    explicit MemoryEntityRepository(QObject *parent = nullptr)
     : AbstractEntityRepository<EntityData, UniqueId>(parent)
    {
    }

    /*! \brief Add \a record to the storage
     *
     * \note Currently only works for entities that have a single Id,
     *   not a composed PK
     */
    Mdt::Expected<UniqueId> add(const EntityData & record) override
    {
      auto id = uniqueIdValue<EntityData, UniqueId>(record);

      if(id.isNull()){
        id = UniqueId(nextId());
        auto recordCpy = record;
        setUniqueIdValue<EntityData, UniqueId>(recordCpy, id);
        mTable.insert( id.value(), recordCpy );
      }else{
        if(mTable.contains(id.value())){
          const auto msg = tr("A record with id '%1' allready exists.")
                           .arg(id.value());
          auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
          return error;
        }
        mTable.insert( id.value(), record );
      }

      return id;
    }

    /*! \brief Get a entity by its id
     *
     * If the record matching \a id was found, the record is returned.
     *  If no record was found, a empty one is returned.
     *  If a error occured, this error is returned.
     *
     * \pre \a id must not be null
     */
    Mdt::Expected<EntityData> getById(UniqueId id) const override
    {
      Q_ASSERT(!id.isNull());

      return mTable.value(id.value());
    }

    /*! \brief Get a entity by its primary key
     *
     * If the record matching \a pk was found, the record is returned.
     *  If no record was found, a empty one is returned.
     *  If a error occured, this error is returned.
     *
     * \pre \a pk must not be null
     */
    Mdt::Expected<EntityData> getByPrimaryKey(const PrimaryKeyRecord & pk) const override
    {
      Q_ASSERT(!pk.isNull());
    }

    /*! \brief Get count of items in the memory storage
     */
    int storageCount() const
    {
      return mTable.count();
    }

   private:

    int nextId() const
    {
      if(mTable.isEmpty()){
        return 1;
      }
      return mTable.lastKey() + 1;
    }

    QMap<int, EntityData> mTable;

  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_MEMORY_ENTITY_REPOSITORY_H
