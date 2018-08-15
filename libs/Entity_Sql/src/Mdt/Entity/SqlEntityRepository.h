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
#ifndef MDT_ENTITY_SQL_ENTITY_REPOSITORY_H
#define MDT_ENTITY_SQL_ENTITY_REPOSITORY_H

#include "SqlEntityRepositoryImpl.h"
#include "Mdt/Entity/AbstractEntityRepository.h"
#include "Mdt/Entity/PrimaryKeyRecord.h"
#include <QSqlDatabase>

namespace Mdt{ namespace Entity{

  /*! \brief SQL implementation for AbstractEntityRepository
   */
  template<typename EntityData, typename UniqueId>
  class SqlEntityRepository : public virtual AbstractEntityRepository<EntityData, UniqueId>
  {
   public:

    /*! \brief Constructor
     */
    explicit SqlEntityRepository(QObject *parent = nullptr)
     : AbstractEntityRepository<EntityData, UniqueId>(parent)
    {
    }

    /*! \brief Set the database connection \a db to this repository
     *
     * \pre \a db must be valid (must have a driver loaded)
     */
    void setDatabase(const QSqlDatabase & db)
    {
      Q_ASSERT(db.isValid());

      mImpl.setDatabase(db);
    }

    /*! \brief Get the database connection
     */
    QSqlDatabase database() const
    {
      return mImpl.database();
    }

    /*! \brief Add \a record to the storage
     *
     * \note Currently only works for entities that have a single Id,
     *   not a composed PK
     */
    Mdt::Expected<UniqueId> add(const EntityData & record) override
    {
      return mImpl.add<EntityData, UniqueId>(record);
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

      return mImpl.getById<EntityData, UniqueId>(id);
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

      return mImpl.getByPrimaryKey<EntityData>(pk);
    }

   private:

    SqlEntityRepositoryImpl mImpl;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_SQL_ENTITY_REPOSITORY_H
