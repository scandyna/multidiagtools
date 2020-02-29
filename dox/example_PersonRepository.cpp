/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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

/*! \page example_PersonRepository Person repository example (blocking and async)
 *
 * \todo Primary key
 * \todo Unique constraint
 * \todo SQL schema
 *
 * This example shows how a repository could be implemented.
 *
 * \section domain_model Domain model
 *
 * In the domain model we want as less dependencies as possible.
 *
 * The following libraries will be used:
 *  - Mdt::Expected
 *  - Mdt::Numeric
 *  - \ref libs_Reflection
 *
 * Mdt::Expected is just a helper, std::expected could (or should) be used instead.
 *
 * Because C++ currently (C++17) does not have any native support for reflection,
 *  the %Mdt provided reflection library is also used.
 *  The main goal of using reflection is to reduce the amount of boilerplate code
 *  as much as possible when objects have to be persisted.
 *
 * \note Mdt libraries heavliy depends on Qt,
 *   because Qt provides very useful tools currently missing in C++ (C++17):
 *  - QString: good support for unicode, simple to use. Qt also provides QTextCodec and related stuff.
 *  - QVariant: runtime variant, simple to use (is complementary to std::variant or boost::variant)
 *
 * \subsection person_entity Person entity
 *
 * Because reflection is needed, the domain entity, %Person,
 *  will be implemented in several parts.
 *
 * At first, a data struct is created, which also receives some attributes:
 * \code
 * #include <Mdt/Reflection/ReflectStruct.h>
 * #include <Mdt/Reflection/AutoIncrementIdPrimaryKey.h>
 * #include <Mdt/Reflection/UniqueConstraint.h>
 * #include <QString>
 *
 * struct PersonDataStruct
 * {
 *   qlonlong id = 0;
 *   QString firstName;
 *   QString lastName;
 * };
 *
 * MDT_REFLECT_STRUCT(
 *   (PersonDataStruct),
 *   Person,
 *   (id),
 *   (firstName, FieldFlag::IsRequired, FieldMaxLength(100)),
 *   (lastName, FieldFlag::IsRequired, FieldMaxLength(100))
 * )
 * \endcode
 *
 * Notice that some flags are affected to %firstName and %lastName.
 *  Those attributes seems to be a persisting concern,
 *  but declaring such attributes directly here could also be reused
 *  in a GUI (f.ex. setting the max length in a QLineEdit).
 *
 * A strong id can be created for the entity:
 * \code
 * #include <Mdt/Numeric/IntegralUniqueIdTemplate.h>
 *
 * class PersonId : public Mdt::Numeric::IntegralUniqueIdTemplate<PersonId>
 * {
 *  public:
 *
 *   using IntegralUniqueIdTemplate::IntegralUniqueIdTemplate;
 * };
 * \endcode
 *
 * Here is the declaration of the %Person domain class:
 * \code
 * #include "PersonDataStruct.h"
 * #include <QString>
 *
 * class Person
 * {
 *  public:
 *
 *   Person(const QString & firstName, const QString & lastName);
 *   QString firstName() const;
 *   QString lastName() const;
 *   // Some other usefull methods ..
 *
 *  private:
 *
 *   Person(const PersonDataStruct & data);
 *
 *   friend
 *   Person personFromDataStruct(const PersonDataStruct & data);
 *
 *   friend
 *   const PersonDataStruct & personPrivateConstDataStruct(const Person & person);
 *
 *   friend
 *   PersonDataStruct & personPrivateDataStruct(Person & person);
 *
 *   PersonDataStruct mDataStruct;
 * };
 * \endcode
 *
 * The internal data struct is not exposed in the domain model,
 *  but it will be required later to implement a repository.
 *  This is done using friend access functions.
 *
 * In some private header, we could define those access functions:
 * \code
 * // Person_p.h
 * #include "Person.h"
 *
 * inline
 * Person personFromDataStruct(const PersonDataStruct & data)
 * {
 *   return Person(data);
 * }
 *
 * inline
 * const PersonDataStruct & personPrivateConstDataStruct(const Person & person)
 * {
 *   return person.mDataStruct;
 * }
 *
 * inline
 * PersonDataStruct & personPrivateDataStruct(Person & person)
 * {
 *   return person.mDataStruct;
 * }
 * \endcode
 *
 * \sa \ref libs_Reflection
 *
 * \subsection repository_interface Repository interface
 *
 * In the domain model, a repository interface is created,
 *  which has as less dependencies as possible.
 *
 * Here is a example of a repository interface declaration:
 * \code
 * #include "Person.h"
 * #include <Mdt/Error.h>
 * #include <Mdt/Expected.h>
 *
 * class PersonRepository
 * {
 *  public:
 *
 *   virtual Mdt::Expected<PersonId> add(const Person & person) = 0;
 *   virtual Mdt::Expected<Person> get(PersonId id) const = 0;
 *   virtual bool update(const Person & person) = 0;
 *   virtual bool remove(PersonId id) = 0;
 *   virtual bool removeAll() = 0;
 *
 *   Mdt::Error lastError() const;
 *
 *  protected:
 *
 *   void setLastError(const Mdt::Error & error) const;
 *
 *  private:
 *
 *   mutable Mdt::Error mLastError;
 * };
 * \endcode
 *
 * \section sql_person_repository SQL implementation of Person repository
 *
 * In this repository implementation, the following libraries will be used:
 *  - \ref libs_Sql
 *
 * Here is a example of a repository declaration:
 *  \todo bool + lastError()
 * \code
 * #include "PersonRepository.h"
 * #include "PersonDataStruct.h"
 * #include <Mdt/Sql/ReflectionStorageTableWithId.h>
 *
 * class SqlPersonRepository : public PersonRepository
 * {
 *  public:
 *
 *   SqlPersonRepository(const Mdt::Sql::Connection & connection);
 *   Mdt::Expected<PersonId> add(const Person & person) override;
 *   Mdt::Expected<Person> get(PersonId id) const override;
 *   bool update(const Person & person) override;
 *   bool remove(PersonId id) override;
 *   bool removeAll() override;
 *
 *  private:
 *
 *   Mdt::Sql::ReflectionStorageTableWithId<PersonDef, PersonPrimaryKey, PersonId::value_type> mTable;
 * };
 * \endcode
 *
 * The implementation could look like:
 * \code
 * #include "SqlPersonRepository.h"
 * #include "Person_p.h"
 *
 * SqlPersonRepository::SqlPersonRepository(const Mdt::Sql::Connection & connection)
 *  : mTable(connection)
 * {
 * }
 *
 * Mdt::Expected<PersonId> SqlPersonRepository::add(const Person & person)
 * {
 *   const auto id = mTable.add( personPrivateConstDataStruct(person) );
 *   if(!id){
 *     // Maybe generate a more domain specific error message here
 *     setLastError(id.error());
 *     return id.error();
 *   }
 *   return PersonId(*id);
 * }
 *
 * Mdt::Expected<Person> SqlPersonRepository::get(PersonId id) const
 * {
 *   const auto personData = mTable.get( id.value() );
 *   if(!personData){
 *     // Maybe generate a more domain specific error message here
 *     setLastError(personData.error());
 *     return personData.error();
 *   }
 *   return personFromDataStruct(*personData);
 * }
 *
 * bool SqlPersonRepository::update(const Person & person)
 * {
 *   if( !mTable.update( personPrivateConstDataStruct(person) ) ){
 *     // Maybe generate a more domain specific error message here
 *     setLastError(mTable.lastError());
 *     return false;
 *   }
 *
 *   return true;
 * }
 *
 * bool SqlPersonRepository::remove(PersonId id)
 * {
 *   if( !mTable.remove( id.value() ) ){
 *     // Maybe generate a more domain specific error message here
 *     setLastError(mTable.lastError());
 *     return false;
 *   }
 *
 *   return true;
 * }
 *
 * bool SqlPersonRepository::removeAll()
 * {
 *   if( !mTable.removeAll() ){
 *     // Maybe generate a more domain specific error message here
 *     setLastError(mTable.lastError());
 *     return false;
 *   }
 *
 *   return true;
 * }
 * \endcode
 *
 * \sa Mdt::Sql::ReflectionStorageTableWithId
 *
 * \section person_repository_with_async Person repository with some asyc method
 *
 * \subsection person_repository_with_async_iterface Repository interface
 *
 * The repository interface changes a bit:
 * \code
 * #include "Person.h"
 * #include <Mdt/Expected.h>
 * #include <Mdt/Error.h>
 * #include <QObject>
 *
 * class PersonRepository : public QObject
 * {
 *   Q_OBJECT
 *
 *  public:
 *
 *   virtual void addAsync(const Person & person) = 0;
 *   virtual Mdt::Expected<PersonId> add(const Person & person) = 0;
 * 
 *   virtual void submitGet(PersonId id) const = 0;
 *   virtual Mdt::Expected<Person> get(PersonId id) const = 0;
 * 
 *   virtual void getAllAsync() = 0;
 *   virtual Mdt::Expected<PersonList> getAll() const = 0;
 * 
 *   virtual void submitUpdate(const Person & person) = 0;
 *   virtual bool update(const Person & person) = 0;
 *   virtual void submitRemove(PersonId id) = 0;
 *   virtual bool remove(PersonId id) = 0;
 *   virtual void submitRemoveAll() = 0;
 *   virtual bool removeAll() = 0;
 *
 *   Mdt::Error lastError() const;
 *
 *  signals:
 *
 *   void personAdded(PersonId id);
 *   void newPersonFetched(const Person & person);
 * 
 *   void updatePersonDone(PersonId id);
 *   void errorOccured(const Mdt::Error & error);
 *
 *  protected:
 *
 *   void setLastError(const Mdt::Error & error) const;
 *
 *  private:
 *
 *   mutable Mdt::Error mLastError;
 * };
 * \endcode
 *
 * The async methods will return early, once the query was submitted to the backend.
 *  In this example, the Qt signal/slot mechanism will be used to notify
 *  when a person was added or fetched from the backend, or a error occured.
 *
 * \subsection sql_person_repository_with_async SQL implementation of Person repository with async method
 *
 * Here is a example of the new SQL repository declaration:
 * \code
 * #include "PersonRepository.h"
 * #include "PersonDataStruct.h"
 * #include <Mdt/Expected.h>
 * #include <Mdt/Container/VariantRecord.h>
 * #include <Mdt/Sql/ReflectionStorageTableWithId.h>
 * #include <Mdt/Sql/ReflectionAsyncStorageTableWithId.h>
 * #include <Mdt/Sql/Connection.h>
 * #include <Mdt/Sql/AsyncQueryConnection.h>
 * #include <memory>
 *
 * class SqlPersonRepository : public PersonRepository
 * {
 *   using PersonTable = Mdt::Sql::ReflectionAsyncStorageTableWithId<PersonDef, PersonPrimaryKey, PersonId::value_type>;
 *
 *  public:
 *
 *   SqlPersonRepository(const std::shared_ptr<Mdt::Sql::AsyncQueryConnection> & connection, QObject *parent = nullptr);
 *
 *   void addAsync(const Person & person) override;
 *   Mdt::Expected<PersonId> add(const Person & person) override;
 * 
 *   void submitGet(PersonId id) const override;
 *   Mdt::Expected<Person> get(PersonId id) const override;
 * 
 *   void getAllAsync() override;
 *   Mdt::Expected<PersonList> getAll() const override;
 * 
 *   void submitUpdate(const Person & person) override;
 *   bool update(const Person & person) override;
 *   void submitRemove(PersonId id) override;
 *   bool remove(PersonId id) override;
 *   void submitRemoveAll() override;
 *   bool removeAll() override;
 *
 *  private slots:
 *
 *   void onNewPersonRecordFetched(const Mdt::Container::VariantRecord & record);
 *   void onPersonAdded(const QVariant & id);
 * 
 *   void onTableAddErrorOccured(const Mdt::Error & error);
 *   void onTableGetErrorOccured(const Mdt::Error & error);
 *   void onTableUpdateErrorOccured(const Mdt::Error & error);
 *   void onTableRemoveErrorOccured(const Mdt::Error & error);
 *
 *  private:
 *
 *   PersonTable mPersonTable;
 * };
 * \endcode
 *
 * In the implementation, the following headers will be used:
 * \code
 * #include "SqlPersonRepository.h"
 * #include "Person_p.h"
 * \endcode
 *
 * For the implementation, start from the constructor:
 * \code
 * SqlPersonRepository::SqlPersonRepository(const std::shared_ptr<Mdt::Sql::AsyncQueryConnection> & connection, QObject *parent = nullptr)
 *  : PersonRepository(parent),
 *    mPersonTable(connection)
 * {
 *   connect(&mPersonTable, &PersonTable::newRecordAdded, this, &SqlPersonRepository::onNewPersonRecordFetched);
 *   connect(&mPersonTable, &PersonTable::newRecordFetched, this, &SqlPersonRepository::onPersonAdded);
 *   connect(&mPersonTable, &PersonTable::errorOccured, this, &SqlPersonRepository::errorOccured);
 * }
 * \endcode
 *
 * The implementation of addAsync() will emit the personAdded() signal.
 *  The table will notify when a person was added and pass the new id.
 *  From that, we create a %PersonId domain object and finally emit the personAdded() signal:
 * \code
 * void SqlPersonRepository::onPersonAdded(const QVariant & id)
 * {
 *   emit personAdded( PersonId::fromQVariant(id) );
 * }
 * \endcode
 *
 * Here is a implementation of addAsync():
 * \code
 * void SqlPersonRepository::addAsync(const Person & person)
 * {
 *   mPersonTable.addAsync( personPrivateConstDataStruct(person) );
 * }
 * \endcode
 *
 * The implementation of getAllAsync() will emit the newPersonFetched() signal.
 *  The table will notify each time a record was fetched from the database table.
 *  From that, we create a %Person domain object and finally emit the newPersonFetched() signal:
 * \code
 * void SqlPersonRepository::onNewPersonRecordFetched(const Mdt::Container::VariantRecord & record)
 * {
 *   emit newPersonFetched( personFromDataStruct( PersonTable::structFromRecord(record) ) );
 * }
 * \endcode
 *
 * The getAllAsync() implementation looks like:
 * \code
 * void SqlPersonRepository::getAllAsync() const
 * {
 *   mPersonTable.getAllAsync();
 * }
 * \endcode
 *
 * \todo Fix:
 *    - Should emit result and error for sync operations ?
 *    - How to catch errors reliabily ?
 *
 * The implementation could look like:
 * \code
 * #include "SqlPersonRepository.h"
 * #include "Person_p.h"
 *
 *
 * void SqlPersonRepository::submitAdd(const Person & person)
 * {
 *   mTable.submitAdd( personPrivateConstDataStruct(person) );
 * }
 *
 * Mdt::Expected<PersonId> SqlPersonRepository::add(const Person & person)
 * {
 *   const auto id = mTable.add( personPrivateConstDataStruct(person) );
 *   if(!id){
 *     // Maybe generate a more domain specific error message here
 *     return id.error();
 *   }
 *   return PersonId(*id);
 * }
 *
 * Mdt::Expected<Person> SqlPersonRepository::get(PersonId id) const
 * {
 *   const auto personData = mTable.get( id.value() );
 *   if(!personData){
 *     // Maybe generate a more domain specific error message here
 *     return personData.error();
 *   }
 *   return personFromDataStruct(*personData);
 * }
 *
 * Mdt::ExpectedResult SqlPersonRepository::update(const Person & person)
 * {
 *   const auto result = mTable.update( personPrivateConstDataStruct(person) );
 *   if(!result){
 *     // Maybe generate a more domain specific error message here
 *   }
 *   return result;
 * }
 *
 * Mdt::ExpectedResult SqlPersonRepository::remove(PersonId id)
 * {
 *   const auto result = mTable.remove( id.value() );
 *   if(!result){
 *     // Maybe generate a more domain specific error message here
 *   }
 *   return result;
 * }
 *
 * Mdt::ExpectedResult SqlPersonRepository::removeAll()
 * {
 *   const auto result = mTable.removeAll();
 *   if(!result){
 *     // Maybe generate a more domain specific error message here
 *   }
 *   return result;
 * }
 *
 * void SqlPersonRepository::getAllAsync()
 * {
 *   mTable.getAllAsync();
 * }
 *
 * void onNewRecordAvailable(const Mdt::Container::VariantRecord & record)
 * {
 *   emit newPersonFetched( mTable.structFromRecord(record) );
 * }
 * \endcode
 *
 * \sa Mdt::Sql::ReflectionAsyncStorageTableWithId
 *
 * Take a look of the method that changes in their implementation.
 *
 * \todo For coherence, the whole implementation changes, because errors must now be notified
 *
 * \todo Think we .....
 *
 * Constructor:
 * \code
 * SqlPersonRepository::SqlPersonRepository(const QSqlDatabase & db, const std::shared_ptr<Mdt::Sql::AsyncQueryConnection> & asyncConnection, QObject *parent)
 *  : QObject(parent),
 *    mTable(db),
 *    mAsyncTable(asyncConnection)
 * {
 * }
 * \endcode
 *
 * For coherence, get will also emit the ...... 
 */
