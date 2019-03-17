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

/*! \page libs_Reflection Reflection library
 *
 * \section introduction Introduction
 *
 * In a domain model some entity is declared:
 * \code
 * class Person
 * {
 *  public:
 *
 *   Person(const QString & firstName, const QString & lastName);
 *   PersonId id() const;
 *   QString firstName() const;
 *   QString lastName() const;
 *   // Some other usefull methods ..
 *
 *  private:
 *
 *   QString mFirstName;
 *   QString mLastName;
 * };
 * \endcode
 *
 * At some place, we should be able to persist a object of %Person,
 *  in a database, or send it over the network in JSON format.
 *  In a context of database, we also need to create some schema first.
 *
 * The goal is to avoid creating all the necessary stuff to create
 *  a SQL database schema, SQL queries, etc..
 *
 * The domain entity, %Person, should ideally not be affected for reflection.
 *
 * Current C++ (C++17) offers no mechanism for reflection.
 *  There is no way, for example, to travel a class and query
 *  there members (like member name).
 *  To create reflection, we can use tuples and template magic.
 *
 * Reflection is done by creating a data struct,
 *  then using MDT_REFLECT_STRUCT() :
 * \code
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
 *   (firstName, FieldFlag::IsRequired),
 *   (lastName, FieldFlag::IsRequired)
 * )
 * \endcode
 *
 * \sa MDT_REFLECT_STRUCT()
 *
 * \section non_intrusive_reflection Non intrusive reflection
 *
 * Whe could provide functions to convert between %Person and %PersonDataStruct:
 * \code
 * #include "Person.h"
 * #include "PersonDataStruct.h"
 *
 * Person personFromDataStruct(const PersonDataStruct & data)
 * {
 *   return Person(data.id, data.firstName, data.lastName);
 * }
 *
 * PersonDataStruct dataStructFromPerson(const Person & person)
 * {
 *   return PersonDataStruct{person.id().value(), person.firstName(), person.lastName()};
 * }
 * \endcode
 *
 * In the repository implementation:
 * \code
 * bool SqlPersonRepository::add(const Person & person)
 * {
 *   return mImpl.add<PersonDef>( dataStructFromPerson(person) );
 * }
 * \endcode
 *
 * Here, there is no reflection details in the domain object.
 *  By providing the two helper functions for conversion between
 *  %Person and %PersonDataStruct, we have only a few changes
 *  to make when changing the attributes of %Person;
 *
 * \section somewhat_instrusive_reflection Somewhat intrusive reflection
 *
 * The above data struct will now be used as member in the domain object:
 * \code
 * #include "PersonDataStruct.h"
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
 *   PersonDataStruct & dataStruct();
 *   const PersonDataStruct & constDataStruct() const;
 *
 *  private:
 *
 *   PersonDataStruct mDataStruct;
 * };
 * \endcode
 *
 * In the repository implementation:
 * \code
 * bool SqlPersonRepository::add(const Person & person)
 * {
 *   return mImpl.add<PersonDef>(person.constDataStruct());
 * }
 * \endcode
 *
 * Here, the domain entity has knowlage of reflection tricks.
 *  This could maybe bring some less overhead, because there are
 *  no data structs to instanciante and copy for each repository operations.
 *
 * But, here the domain model has direct access to the attributes of %Person:
 * \code
 * person.dataStruct().firstName = " ";
 * \endcode
 * In above example, we change the first name to some invalid string.
 *
 * \section reflect_with_friend Reflection with friend access function
 *
 * One solution could be to define friend functions to access the internal data struct:
 * \code
 * #include "PersonDataStruct.h"
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
 * In the repository implementation:
 * \code
 * #include "Person_p.h"
 *
 * bool SqlPersonRepository::add(const Person & person)
 * {
 *   return mImpl.add<PersonDef>( personPrivateConstDataStruct(person) );
 * }
 * \endcode
 *
 * \section reflect_domain_object Reflect the domain entity directly
 *
 * \code
 * class Person
 * {
 *  public:
 *
 *   Person(const QString & firstName, const QString & lastName);
 *   Person(PersonId id, const QString & firstName, const QString & lastName);
 *   void setPersonId(PersonId id);
 *   PersonId id() const;
 *   void setFirstName(const QString & name);
 *   QString firstName() const;
 *   void setLastName(const QString & name);
 *   QString lastName() const;
 *   // Some other usefull methods ..
 *
 *  private:
 *
 *   PersonId mId;
 *   QString mFirstName;
 *   QString mLastName;
 * };
 * \endcode
 *
 * To reflect the domain entity directly, we force %Person
 *  to provide a ID. Also, all setters and getters are mandatory for the reflection.
 *  Also, if some value objects are used, like %PersonId, %PersonName, etc..,
 *  we probably will have to provide some stuff to make those value objects serializable.
 *
 * Here, the domain entity is strongly coupled with the reflection details.
 *
 * This is not supported by %Mdt %Reflection.
 *
 * \section field_types Field types
 *
 * While defining the data struct to reflect, take care to privilege types that are handled by QMetaType:
 * \code
 * struct PersonDataStruct
 * {
 *   qlonglong id = 0;
 *   QString firstName;
 *   QString lastName;
 * };
 * \endcode
 *
 * Doing so will enable to reuse the existing tools.
 *  For example, Qt modules, like QSqlQuery, uses QVariant as data interface,
 *  which also relies on QMetaType.
 *  In a later chapiter, it will be proposed a simple way to
 *  create a SQL schema using functions provided in Mdt/Sql/Schema/Reflection.h .
 *  Those functions also relies on types known by QMetaType.
 *
 * For primitive types, like integral types, they should be initialized in the struct,
 *  avoiding strange values when a struct is instanciated.
 *
 * Please also note to avoid using unsigned integral types.
 *  Comparing them < 0, for example, can be surprising.
 *  They also will give problems to be stored in some databases,
 *  because some DBMS do not support unsigned integral types (like PostgreSQL).
 *
 * Choosing a unique ID type.
 *  Most DBMS provide a auto increment integral unique id primary key.
 *  SQLite uses 64 bit (signed) int, and no other type can be defined.
 *  MySQL is more flexible. In some examples, a (signed, 32bit) integer is used.
 *  PostgreSQL uses smallserial (signed 16bit integral), serial (signed 32bit integral) and bigserial (signed 64bit  integral).
 *  To be compatible across those databases, qlonglong (signed 64 bit integer) should be choosed.
 *
 * It is possible to define custom types:
 * \code
 * #include <QMetaType>
 *
 * struct MyStruct
 * {
 *   int i = 0;
 * };
 * Q_DECLARE_METATYPE(MyStruct)
 * \endcode
 * See Qt's QMetaType documentation for more details.
 *
 * Please also considere that using custom types will require
 *  more work in some modules.
 *
 * \section Ids_and_primary_keys IDs and primary keys
 *
 * The concept of a primary key seems to be specific to the world of relational database.
 *
 * Here is a example of a person id domain object:
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
 * Because above class uses a integral type that is supported by QVariant,
 *  it can be persisted in a simple way.
 *  A example of a repository implementation is done in the SQL library.
 *
 * \sa \ref libs_Sql
 *
 * Lets take a example of a id that contains more than one field:
* \code
 * struct CableLinkDataStruct
 * {
 *   qlonglong startConnectionId = 0;
 *   qlonglong endConnectionId = 0;
 *   QString identification;
 * };
 *
 * MDT_REFLECT_STRUCT(
 *   (CableLinkDataStruct),
 *   CableLink,
 *   (startConnectionId),
 *   (endConnectionId),
 *   (identification, FieldFlag::IsRequired)
 * )
 *
 * using CableLinkPrimaryKey = Mdt::Reflection::PrimaryKey<CableLinkDef::startConnectionId, CableLinkDef::endConnectionId>;
 * \endcode
 *
 * Here, some concept of a record is required as value of the primary key:
 * \code
 * using CableLinkIdRecord = Mdt::Reflection::PrimaryKeyRecord<CableLinkPrimaryKey>;
 * \endcode
 *
 * Getting the values from a struct:
 * \code
 * CableLinkDataStruct link;
 * link.startConnectionId = 14;
 * link.endConnectionId = 31;
 *
 * CableLinkIdRecord pkr = Mdt::Reflection::primaryKeyRecordFromStruct<CableLinkPrimaryKey>(link);
 * \endcode
 *
 * 
 *
 * \code
 * struct CableLinkIdRecord
 * {
 *   qlonglong startConnectionId = 0;
 *   qlonglong endConnectionId = 0;
 * };
 * \endcode
 *
 *
 * Above code will not compile, because LinkIdRecord was not reflected,
 *  implying that the LinkIdRecord struct cannot be traversed.
 *
 * \code
 * using CableLinkIdRecord = std::array<qlonglong, 2>;
 * \endcode
 *
 * \todo should also support tuple
 *
 * \code
 * using CableLinkIdRecord = Mdt::Reflection::PrimaryKeyRecord<CableLinkPrimaryKey>;
 * \endcode
 *
 * \code
 * MDT_REFLECTION_MAKE_PRIMARY_KEY_RECORD(
 *   (CableLinkDataStruct),
 *   CableLinkIdRecord,
 *   (startConnectionId),
 *   (endConnectionId)
 * )
 * \endcode
 *
 * .... generates:
 * \code
 * struct CableLinkIdRecord
 * {
 *   qlonglong startConnectionId = qlonglong{};
 *   qlonglong endConnectionId = qlonglong{};
 * };
 * \endcode
 *
 * ... or:
 * \code
 * struct CableLinkIdRecord
 * {
 *   qlonglong & startConnectionId()
 *   {
 *     return boost::get<0>(_internalVector);
 *   }
 *
 *   qlonglong & endConnectionId()
 *   {
 *     return boost::get<1>(_internalVector);
 *   }
 *
 *   boost::fusion::vector<qlonglong, qlonglong> _internalVector;
 * };
 * \endcode
 *
 * ... or:
 * \code
 * using CableLinkIdRecord = boost::fusion::vector<qlonglong, qlonglong>;
 * \endcode
 *  .. bof! (tags..)
 *
 * \todo See Mdt::Entity::valueAt()
 *   If the final result depends on QVariant,
 *   better use std::array<QVariant, N> for eterogenious PKR..
 *
 * \todo problems:
 *  - Be able to iterate over the pk record
 *  - map between a reflected struct (PersonDataStruct) and its pk members
 *  - get the type infos about a member of pk record with its associated struct definition
 *  - See what BOOST_FUSION_ASSOC_STRUCT() can provide  <- Nothing ! Just a pointer to a reflected data struct..
 *
 * \code
 * struct LinkIdRecordDataStruct
 * {
 *   qlonglong endConnectionId = 0;
 *   qlonglong startConnectionId = 0;
 * };
 *
 * MDT_REFLECT_STRUCT(
 *   (LinkIdRecordDataStruct),
 *   LinkIdRecord,
 *   (endConnectionId),
 *   (startConnectionId)
 * )
 * \endcode
 *
 *  For example, get the link from a repository by id:
 * \code
 * class LinkRepository
 * {
 *  public:
 *
 *   // Here, LinkId is a domain object
 *   Mdt::Expected<Link> getLink(LinkId id) const;
 * };
 * \endcode
 *
 * \code
 * // Methods are omitted here
 * class LinkId
 * {
 *   qlonglong mStartConnectionId = 0;
 *   qlonglong mEndConnectionId = 0;
 * };
 * \endcode
 *
 * \code
 * using LinkIdData = std::array<qlonglong, 2>;
 * \endcode
 *
 * \code
 * // Some methods are omitted here
 * class LinkId
 * {
 *  public:
 *
 *   qlonglong startConnectionId() const
 *   {
 *     return mData[0];
 *   }
 *
 *   qlonglong endConnectionId() const
 *   {
 *     return mData[1];
 *   }
 *
 *  private:
 *
 *   LinkIdData mData;
 * };
 * \endcode
 *
 *
 * \section Attributes
 *
 * Some attributes could be specific to some persistance engine.
 *  A example could be the primary key in a SQL database.
 *
 * Such attributes could be defined, for example,
 *  in some SQL schema:
 * \code
 * using PersonPrimaryKey = Mdt::Reflection::PrimaryKey<PersonDef::id>;
 * \endcode
 *
 * Some other attributes could also have meaning in the domain busines logic.
 *  For example, if some data is required, this could be defined in one place,
 *  then reused on the domain logic, in the UI and in the persistance
 *  (f.ex. generating corresponding constraints on the database schema).
 *
 * \sa Mdt::Reflection::PrimaryKey
 * \sa Mdt::Reflection::IdPrimaryKey
 * \sa Mdt::Reflection::AutoIncrementIdPrimaryKey
 *
 * Defining a unique constraint can be done this way:
 * \code
 * using PersonUniqueConstraint = Mdt::Reflection::UniqueConstraint<PersonDef::firstName, PersonDef::lastName>;
 * \endcode
 *
 * This can be later enforced, for example, in a SQL schema.
 *
 * \sa Mdt::Reflection::UniqueConstraint
 *
 * Define a relation between 2 structs.
 *  The concept of relation can be used to generate,
 *  for example, a SQL schema, and can also be re-used
 *  later to generate some queries.
 *
 * In this example, a relation between a person and its addresses will be defined.
 *
 * Here is the person struct:
 * \code
 * struct PersonDataStruct
 * {
 *   qlonglong id = 0;
 *   QString firstName;
 *   QString lastName;
 * };
 *
 * MDT_REFLECT_STRUCT(
 *   (PersonDataStruct),
 *   Person,
 *   (id),
 *   (firstName),
 *   (lastName)
 * )
 * \endcode
 *
 * Here the address struct is defined:
 * \code
 * struct AddressDataStruct
 * {
 *   qlonglong id = 0;
 *   QString street;
 *   qlonglong personId = 0;
 * };
 *
 * MDT_REFLECT_STRUCT(
 *   (AddressDataStruct),
 *   Address,
 *   (id),
 *   (street),
 *   (personId)
 * )
 * \endcode
 *
 * \code
 * using PersonPrimaryKey = Mdt::Reflection::PrimaryKey<PersonDef::id>;
 * \endcode
 *
 * \code
 * using PersonAddressRelation = Mdt::Reflection::Relation<PersonPrimaryKey, AddressDef::personId>;
 * \endcode
 *
 * \sa Mdt::Reflection::Relation
 *
 * \section sql_schema Create a SQL schema
 *
 * In this example, a simple function will create
 *  a SQL schema based on reflected entities:
 * \code
 * #include "PersonDataStruct.h"
 * #include "AddressDataStruct.h"
 * #include <Mdt/Sql/Schema/Reflection.h>
 * #include <Mdt/Sql/Schema/ForeignKeySettings.h"
 * #include <Mdt/Sql/Schema/Driver.h>
 * #include <QSqlDatabase>
 *
 * using PersonPrimaryKey = Mdt::Reflection::PrimaryKey<PersonDef::id>;
 * using AddressPrimaryKey = Mdt::Reflection::PrimaryKey<AddressDef::id>;
 * using PersonAddressRelation = Mdt::Reflection::Relation<PersonPrimaryKey, AddressDef::personId>;
 *
 * bool createSqlSchema(const QSqlDatabase & dbConnection)
 * {
 *   using Mdt::Sql::Schema::ForeignKeyAction;
 *
 *   Mdt::Sql::Schema::ForeignKeySettings commonForeignKeySettings;
 *   commonForeignKeySettings.setIndexed(true);
 *   commonForeignKeySettings.setOnDeleteAction(ForeignKeyAction::Restrict);
 *   commonForeignKeySettings.setOnUpdateAction(ForeignKeyAction::Cascade);
 *
 *   Mdt::Sql::Schema::Driver driver(dbConnection);
 *   if(!driver.isValid()){
 *     return false;
 *   }
 *
 *   auto personTable = Mdt::Sql::Schema::tableFromReflected<PersonDef, PersonPrimaryKey>();
 *   Mdt::Sql::Schema::addUniqueConstraintToTable<PersonUniqueConstraint>(personTable);
 *   if( !driver.createTable(personTable) ){
 *     return false;
 *   }
 *
 *   auto addressTable = Mdt::Sql::Schema::tableFromReflected<AddressDef, AddressPrimaryKey>();
 *   Mdt::Sql::Schema::addForeignKeyFromRelationToTable<PersonAddressRelation>(addressTable, commonForeignKeySettings);
 *   if( !driver.createTable(addressTable) ){
 *     return false;
 *   }
 *
 *   return true;
 * }
 * \endcode
 *
 * \sa Mdt::Sql::Schema::Reflection::tableFromReflected()
 * \sa Mdt::Sql::Schema::Reflection::addUniqueConstraintToTable()
 * \sa Mdt::Sql::Schema::Reflection::addForeignKeyFromRelationToTable()
 *
 * \note If the reflected struct uses custom types,
 *  a mapping between the custom type or the SQL field type
 *  must be provided.
 *  If such mapping is possible, see Mdt::Sql::Schema::FieldTypeMap .
 *  If not possible, the SQL schema should be done manually .
 *
 * \section sql_queries Use SQL queries
 *
 * In this section it will be discussed how some queries could be created with the reflected structs.
 *
 * Here is a example of a function (which could be a method of a repository)
 *  to add a person to a table:
 * \code
 * #include "PersonDataStruct.h"
 * #include "PersonId.h"
 * #include <Mdt/Expected.h>
 * #include <Mdt/Sql/InsertQuery.h>
 * #include <Mdt/Sql/Reflection/InsertStatement.h>
 *
 * Mdt::Expected<PersonId> addPerson(const PersonDataStruct & person, const QSqlDatabase & dbConnection)
 * {
 *   const auto statement = Mdt::Sql::Reflection::insertStatementFromReflected<PersonPrimaryKey>(person);
 *   Mdt::Sql::InsertQuery query(dbConnection);
 *
 *   if(!query.execStatement(statement)){
 *     return query.lastError(); // Maybe a better error message should be generated here
 *   }
 *
 *   return PersonId::fromQVariant(query.lastInsertId());
 * }
 * \endcode
 *
 * \sa Mdt::Sql::Reflection::insertStatementFromReflected()
 * \sa Mdt::Sql::InsertQuery
 *
 * This next example shows how a person could be updated in a table:
 * \code
 * #include "PersonDataStruct.h"
 * #include <Mdt/Sql/UpdateQuery.h>
 * #include <Mdt/Sql/Reflection/UpdateStatement.h>
 *
 * bool updatePerson(const PersonDataStruct & person, const QSqlDatabase & dbConnection)
 * {
 *   const auto statement = Mdt::Sql::Reflection::updateStatementFromReflected<PersonPrimaryKey>(person);
 *   Mdt::Sql::UpdateQuery query(dbConnection);
 *
 *   if(!query.execStatement(statement)){
 *     setLastError( query.lastError() ); // Maybe a better error message should be generated here
 *     return false;
 *   }
 *
 *   return true;
 * }
 * \endcode
 *
 * \sa Mdt::Sql::Reflection::updateStatementFromReflected()
 * \sa Mdt::Sql::UpdateQuery
 *
 *
 * This example shows how to create arbitrary filter conditions:
 * \code
 * #include "PersonDataStruct.h"
 * #include <Mdt/Sql/DeleteQuery.h>
 * #include <Mdt/QueryExpression/Reflection/FilterExpression.h>
 *
 * using namespace Mdt::QueryExpression::Reflection;
 *
 * QueryField<PersonDef::id> personId;
 *
 * Mdt::Sql::DeleteStatement statement;
 * statement.setTableName("Person");
 * statement.setConditions( personId == 21 );
 * \endcode
 *
 * This example shows how to create a delete statement which could accept arbitrary conditions:
 * \code
 * #include "PersonDataStruct.h"
 * #include <Mdt/Sql/DeleteQuery.h>
 * #include <Mdt/QueryExpression/Reflection/DeleteStatement.h>
 *
 * using namespace Mdt::QueryExpression::Reflection;
 *
 * QueryField<PersonDef::id> personId;
 *
 * DeleteStatement<PersonDef> statement; // Inherits Mdt::QueryExpression::DeleteStatement
 * statement.setConditions( personId == 21 );
 * \endcode
 *
 *
 * This example shows how a person could be removed from a table:
 * \code
 * #include "PersonDataStruct.h"
 * #include "PersonId.h"
 * #include <Mdt/Sql/DeleteQuery.h>
 * #include <Mdt/Sql/Reflection/DeleteStatement.h>
 *
 * bool removePersonById(PersonId id, const QSqlDatabase & dbConnection)
 * {
 *   const auto statement = Mdt::Sql::Reflection::deleteStatementFromReflectedById<PersonPrimaryKey>(id);
 *   Mdt::Sql::DeleteQuery query(dbConnection);
 *
 *   if(!query.execStatement(statement)){
 *     setLastError( query.lastError() ); // Maybe a better error message should be generated here
 *     return false;
 *   }
 *
 *   return true;
 * }
 * \endcode
 *
 * \sa Mdt::Sql::Reflection::deleteStatementFromReflectedById()
 * \sa Mdt::Sql::DeleteQuery
 *
 * \section Alternatives
 *
 * Qt offers some runtime introspection with the QMetaObject.
 *  A idea could be to create a entity as a subclass of QObject,
 *  and define properties:
 * \code
 * class Person : public QObject
 * {
 *  Q_OBJECT
 *  Q_PROPERTY(qlonglong id READ id WRITE setId);
 *  public:
 *   qlonglong id() const;
 *   void setId(qlonglong id);
 * };
 * \endcode
 * The first problem with a QObject subclass is
 *  that it is not a value class: it cannot be copied.
 *  The second problem is that properties are runtime only.
 *  For example, having a expression like:
 * \code
 *  Person::id == 25
 * \endcode
 *  is not possible.
 *
 * There are several C++ libraries available for compile time reflection:
 * - [Boost Mpl](http://www.boost.org/doc/libs/1_69_0/libs/mpl/doc/index.html) which is used to implement Mdt Reflection
 * - [Boost Fusion](https://www.boost.org/doc/libs/1_69_0/libs/fusion/doc/html/index.html) which is used to implement Mdt Reflection
 * - [Boost Hana](http://www.boost.org/doc/libs/1_66_0/libs/hana/doc/html/index.html)
 * - [iguana](https://github.com/qicosmos/iguana)
 * - Probably many more
 */
