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

/*! \page example_Repository Repository example
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
 *  - QString: good support for unicode, simple to use. Qt also provides QTextCodec and related.
 *  - QVariant: runtime variant, simple to use (is complementary to std::variant or boost:variant)
 *
 * \subsection person_entity Person entity
 *
 * Because reflection is needed, the domain entity, %Person,
 *  will be implemented in several parts.
 *
 * At first, a data struct is created, which also receives some attributes:
 * \code
 * #include <Mdt/Reflection/ReflectStruc.h>
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
 * #include <Mdt/Expected.h>
 *
 * class PersonRepository
 * {
 *  public:
 *
 *   virtual Mdt::Expected<PersonId> add(const Person & person) = 0;
 *   virtual Mdt::Expected<Person> get(PersonId id) const  = 0;
 *   virtual bool update(const Person & person)  = 0;
 *   virtual bool remove(PersonId id)  = 0;
 *   virtual bool removeAll()  = 0;
 * };
 * \endcode
 *
 * \section sql_person_repository SQL implementation of Person repository
 *
 * In this repository implementation, the following libraries will be used:
 *  - \ref libs_Sql
 *
 * Here is a example of a repository declaration:
 * \code
 * #include "PersonRepository.h"
 * #include "PersonDataStruct.h"
 * #include <Mdt/Expected.h>
 * #include <Mdt/Sql/ReflectionStorageTableWithId.h>
 *
 * class SqlPersonRepository : public PersonRepository
 * {
 *  public:
 *
 *   SqlPersonRepository(const QSqlDatabase & db);
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
 * SqlPersonRepository::SqlPersonRepository(const QSqlDatabase & db)
 *  : mTable(db)
 * {
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
 * bool SqlPersonRepository::update(const Person & person)
 * {
 *   bool ok = mTable.update( personPrivateConstDataStruct(person) );
 *   if(!ok){
 *     // Maybe generate a more domain specific error message here
 *     setLastError(mTable.lastError());
 *     return false;
 *   }
 *   return true;
 * }
 *
 * bool SqlPersonRepository::remove(PersonId id)
 * {
 *   bool ok = mTable.remove( id.value() );
 *   if(!ok){
 *     // Maybe generate a more domain specific error message here
 *     setLastError(mTable.lastError());
 *     return false;
 *   }
 *   return true;
 * }
 *
 * bool SqlPersonRepository::removeAll()
 * {
 *   bool ok = mTable.removeAll();
 *   if(!ok){
 *     // Maybe generate a more domain specific error message here
 *     setLastError(mTable.lastError());
 *     return false;
 *   }
 *   return true;
 * }
 * \endcode
 *
 * \sa Mdt::Sql::ReflectionStorageTableWithId
 */
