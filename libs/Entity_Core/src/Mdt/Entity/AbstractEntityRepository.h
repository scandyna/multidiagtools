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
#ifndef MDT_ENTITY_ABSTRACT_ENTITY_REPOSITORY_H
#define MDT_ENTITY_ABSTRACT_ENTITY_REPOSITORY_H

#include "AbstractEntityRepositoryBase.h"
#include "PrimaryKey.h"
#include "PrimaryKeyRecord.h"
#include "Mdt/Expected.h"

namespace Mdt{ namespace Entity{

  /*! \brief Base class to create a generic repository
   *
   * Using a generic repository can avoid creating a set of classes
   *  for each entity.
   *  To avoid repetition of template arguments, a alias can be created:
   * \code
   * using PersonRepository = Mdt::Entity::AbstractEntityRepository<PersonData, PersonId>;
   * \endcode
   *
   * Using the generic repository in a business object could be done like this:
   * \code
   * class CreatePerson
   * {
   *  public:
   *
   *   CreatePerson(const std::shared_ptr<PersonRepository> & repository)
   *    : mRepository(repository)
   *   {
   *   }
   *
   *   bool execute();
   *
   *  private:
   *
   *   std::shared_ptr<PersonRepository> mRepository;
   * };
   * \endcode
   *
   * Example to instanciate a generic repository:
   * \code
   * using namespace Mdt::Entity;
   *
   * QSqlDatabase db;
   * auto repository = std::make_shared< SqlEntityRepository<PersonData, PersonId> >();
   * repository->setDatabase(db);
   *
   * CreatePerson createPerson(repository);
   * createPerson.execute();
   * \endcode
   *
   * To add functionalities that are not available from the generic repository,
   *  and avoid to re-implement the provided ones,
   *  2 classes are needed:
   *  - The repository interface (backend independent)
   *  - The repository implementation
   *
   * The repository interface could look like this:
   * \code
   * class PersonRepository : public virtual Mdt::Entity::AbstractEntityRepository<PersonData, PersonId>
   * {
   *  public:
   *
   *   virtual void someCustomFunction() = 0;
   * };
   * \endcode
   *
   * The implementation would be:
   * \code
   * class SqlPersonRepository : public PersonRepository, public SqlEntityRepository<PersonData, PersonId>
   * {
   *  public:
   *
   *   void someCustomFunction() override;
   * };
   * \endcode
   *
   * Using the %PersonRepository:
   * \code
   * QSqlDatabase db;
   * auto repository = std::make_shared<SqlPersonRepository>();
   * repository->setDatabase(db);
   *
   * CreatePerson createPerson(repository);
   * createPerson.execute();
   * \endcode
   *
   * \tparam EntityData Data type that inherits DataTemplate
   * \tparam UniqueId Unique id type that inherits IntegralUniqueId
   */
  template<typename EntityData, typename UniqueId>
  class AbstractEntityRepository : public virtual AbstractEntityRepositoryBase
  {
   public:

    /*! \brief Constructor
     */
    explicit AbstractEntityRepository(QObject *parent = nullptr)
     : AbstractEntityRepositoryBase(parent)
    {
    }

    /*! \brief Add \a record to the storage
     *
     * \note Currently only works for entities that have a single Id,
     *   not a composed PK
     */
    virtual Mdt::Expected<UniqueId> add(const EntityData & record) = 0;

    /*! \brief Get a entity by its id
     *
     * If the record matching \a id was found, the record is returned.
     *  If no record was found, a empty one is returned.
     *  If a error occured, this error is returned.
     *
     * \code
     * const auto personData = repository->getById(personId);
     * if(!personData){
     *   // Error handling
     * }
     * doSomething(*personData);
     * \endcode
     *
     * \pre \a id must not be null
     */
    virtual Mdt::Expected<EntityData> getById(UniqueId id) const = 0;

    /*! \brief Get a entity by its primary key
     *
     * If the record matching \a pk was found, the record is returned.
     *  If no record was found, a empty one is returned.
     *  If a error occured, this error is returned.
     *
     * \code
     * const auto personData = repository->getByPrimaryKey(personPk);
     * if(!personData){
     *   // Error handling
     * }
     * doSomething(*personData);
     * \endcode
     *
     * \pre \a pk must not be null
     */
    virtual Mdt::Expected<EntityData> getByPrimaryKey(const PrimaryKeyRecord & pk) const = 0;

    /*! \brief Update \a record in the storage
     *
     * \note Currently only works for entities that have a single Id,
     *   not a composed PK
     * \pre \a record must have a non null unique id
     */
    virtual bool update(const EntityData & record) = 0;

    /*! \brief Remove the entity identified by \a id
     *
     * Returns true if the entity have been successfully removed,
     *  or did not exists, false on error.
     *
     * \pre \a id must not be null
     */
    virtual bool remove(UniqueId id) = 0;

    /*! \brief Remove all records from the storage
     */
    virtual bool removeAll() = 0;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_ABSTRACT_ENTITY_REPOSITORY_H
