
#include "TestBase.h"
#include "Mdt/Reflection/ReflectStruct.h"
#include "Mdt/Reflection/PrimaryKey.h"
#include "Mdt/Reflection/PrimaryKeyAlgorithm.h"
#include "Mdt/Reflection/FieldAttributes.h"
#include "Mdt/Reflection/FieldAlgorithm.h"
#include "Mdt/Reflection/StructAlgorithm.h"

/*!
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
 *  the using MDT_REFLECT_STRUCT() :
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
 * const PersonDataStruct & personPrivateConstDataStruct(const Person & person)
 * {
 *   return person.mDataStruct;
 * }
 *
 * inline
 * PersonDataStruct & personPrivateDataStruct(Person & person)
 * {
 * return person.mDataStruct;
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
 *  avoiding strange values when a struct is intanciated.
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
 * \section attributes
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
 *  (f.ex. generating corresponding constraints on the databse schema).
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
 *   Mdt::Sql::Schema::addForeignKeyFromRelationToTable<PersonAddressRelation>(personTable, commonForeignKeySettings);
 *   if( !driver.createTable(personTable) ){
 *     return false;
 *   }
 *
 *   return true;
 * }
 * \endcode
 *
 * \sa Mdt::Sql::Schema::Reflection::tableFromReflected()
 *
 * \note If the reflected struct uses custom types,
 *  a mapping between the custom type or the SQL field type
 *  must be provided.
 *  If such mapping is possible, see Mdt::Sql::Schema::FieldTypeMap .
 *  If not possible, the SQL schema should be done manually .
 *
 * \section alternatives
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
 * - [Boost Mpl](http://www.boost.org/doc/libs/1_66_0/libs/mpl/doc/index.html)
 * - [Boost Hana](http://www.boost.org/doc/libs/1_66_0/libs/hana/doc/html/index.html)
 * - [iguana](https://github.com/qicosmos/iguana)
 * - Probably many more
 *
 *
 */

#include <boost/fusion/adapted/struct/adapt_assoc_struct.hpp>
#include <boost/fusion/include/adapt_assoc_struct.hpp>

#include <boost/fusion/adapted/adt/adapt_assoc_adt.hpp>
#include <boost/fusion/include/adapt_assoc_adt.hpp>

#include <boost/fusion/include/for_each.hpp>

// #include <boost/fusion/include/at.hpp>
#include <boost/fusion/include/distance.hpp>
#include <boost/fusion/include/begin.hpp>
#include <boost/fusion/include/find.hpp>

#include <boost/fusion/iterator/key_of.hpp>
#include <boost/fusion/include/key_of.hpp>

#include <boost/fusion/container/map/convert.hpp>
#include <boost/fusion/include/as_map.hpp>

#include <boost/fusion/support/pair.hpp>
#include <boost/fusion/include/pair.hpp>

#include <boost/fusion/algorithm/iteration/iter_fold.hpp>
#include <boost/fusion/include/iter_fold.hpp>

#include <boost/mpl/vector.hpp>
#include <boost/mpl/for_each.hpp>

#include <QDebug>

/*!
 */

struct PersonDataStruct
{
  PersonDataStruct(const PersonDataStruct &) = delete;
  PersonDataStruct & operator=(const PersonDataStruct &) = delete;
  PersonDataStruct(PersonDataStruct &&) = delete;
  PersonDataStruct & operator=(PersonDataStruct &&) = delete;

  int id = 0;
  QString firstName;
  QString lastName;
};

MDT_REFLECT_STRUCT(
  (PersonDataStruct),
  Person,
  (id),
  (firstName, FieldFlag::IsRequired),
  (lastName, FieldFlag::HasDefaultValue, FieldMaxLength(250))
)

/*
struct PersonDef
{
  using reflected_struct = PersonDataStruct;

  static constexpr const char *name_()
  {
    return "Person";
  }

  struct id
  {
    using struct_def = PersonDef;

    static constexpr Mdt::Reflection::FieldAttributes fieldAttributes()
    {
      using namespace Mdt::Reflection;
      return Mdt::Reflection::FieldAttributes();
    }
  };

  struct firstName
  {
    using struct_def = PersonDef;

    static constexpr Mdt::Reflection::FieldAttributes fieldAttributes()
    {
      using namespace Mdt::Reflection;
      return Mdt::Reflection::FieldAttributes(FieldFlag::IsRequired);
    }
  };

  struct lastName
  {
    using struct_def = PersonDef;

    static constexpr Mdt::Reflection::FieldAttributes fieldAttributes()
    {
      using namespace Mdt::Reflection;
      return Mdt::Reflection::FieldAttributes(FieldMaxLength(215), FieldFlag::HasDefaultValue);
    }
  };

  // To iterate all fields using boost::mpl::for_each() - No need to instantiate a Struct for it
  using field_list = boost::mpl::vector<id, firstName, lastName>;
};

BOOST_FUSION_ADAPT_ASSOC_STRUCT(
  PersonDataStruct,
  (id, PersonDef::id)
  (firstName, PersonDef::firstName)
  (lastName, PersonDef::lastName)
)

*/

// struct FieldIsRequired
// {
// };
//
// template<int Length>
// struct FieldMaxLength
// {
// };

// using PersonFirstNameAttributes = boost::mpl::vector<FieldIsRequired, FieldMaxLength<20> >;

// using PersonFieldAttributes =
//   boost::mpl::vector<
//     boost::mpl::vector<>,
//     boost::mpl::vector<FieldIsRequired>,
//     boost::mpl::vector< FieldIsRequired, FieldMaxLength<20> >
//   >;

class Person
{
 public:

  Person(const QString & firstName, const QString & lastName)
   : mDataStruct{0, firstName, lastName}
  {
  }

  QString firstName() const
  {
    return mDataStruct.firstName;
  }

  QString lastName() const
  {
    return mDataStruct.lastName;
  }

 private:

  friend
  const PersonDataStruct & personPrivateConstDataStruct(const Person & person);

  friend
  PersonDataStruct & personPrivateDataStruct(Person & person)
  {
    return person.mDataStruct;
  }

  PersonDataStruct mDataStruct;
};



inline
const PersonDataStruct & personPrivateConstDataStruct(const Person & person)
{
  return person.mDataStruct;
}

struct saver
{
  template<typename FieldValue>
  void operator()(const FieldValue & value) const
  {
    qDebug() << value;
  }
};

struct Inspector
{
  using result_type = int;

  template<typename It>
  int operator()(const int &, const It & it) const
  {
    using Field = typename boost::fusion::result_of::key_of<It>::type;

    qDebug() << "It: " << typeid(It).name();
    qDebug() << "Field: " << typeid(Field).name() << ", value: " << *it;

    return 0;
  }
};

template<typename StructDef>
struct Inspector2
{
  template<typename FieldValuePair>
  void operator()(const FieldValuePair & p) const
  {
    using namespace Mdt::Reflection;

    using Field = typename FieldValuePair::first_type;

    qDebug() << "Field: " << nameFromStructDef<StructDef>() << "." << fieldName<Field>();
    qDebug() << " Required: " << isFieldRequired<Field>();
    qDebug() << " Has default value: " << hasFieldDefaultValue<Field>();
    qDebug() << " Max length: " << fieldMaxLength<Field>();
    qDebug() << " Value: " << p.second;
  }
};

template<typename StructDef>
struct Inspector3
{
  template<typename Field>
  void operator()(Field) const
  {
    using namespace Mdt::Reflection;

    qDebug() << "Inspector3 field: " << fieldName<Field>();
    qDebug() << " Required: " << isFieldRequired<Field>();
    qDebug() << " Has default value: " << hasFieldDefaultValue<Field>();
    qDebug() << " Max length: " << fieldMaxLength<Field>();
  }
};

using PersonPrimaryKey = Mdt::Reflection::PrimaryKey<PersonDef::id, PersonDef::lastName>;

template<typename StructDef>
struct PrintPkName
{
  template<typename Field>
  void operator()(Field)
  {
    using namespace Mdt::Reflection;

    qDebug() << "PK field: " << nameFromStructDef<StructDef>() << '.' << fieldName<Field>();
  }
};

/*
 * Main
 */

int main(int argc, char **argv)
{
  using namespace Mdt::Reflection;

  Person pa(QLatin1String("fA"), QLatin1String("lA"));

//   qDebug() << "Name: " << nameFromDef<PersonDef>();
//   qDebug() << "Name: " << nameFromClass<Person>();
//   qDebug() << "id name: " << fieldName<PersonDef::id>();

  qDebug() << "Field: " << fieldNameAtInStruct<PersonDataStruct, 1>();
  qDebug() << "Field: " << fieldName<PersonDef::lastName>();

  boost::fusion::for_each( personPrivateConstDataStruct(pa), saver());

  forEachFieldValuePairInStruct( personPrivateConstDataStruct(pa), Inspector2<PersonDef>() );

  forEachFieldInStructDef<PersonDef>( Inspector3<PersonDef>() );

  boost::fusion::iter_fold( personPrivateConstDataStruct(pa), 0, Inspector() );

  Mdt::Reflection::forEachPrimaryKeyField<PersonPrimaryKey>( PrintPkName<PersonDef>() );

//   qDebug() << "Id name: " << boost::fusion::extension::struct_member_name<Person, 1>::call();

//   PersonDataStruct person{1, "fA", "lA"};

//   qDebug() << "Name: " << name<PersonDef>();

//   qDebug() << "Id name: " << boost::fusion::extension::struct_member_name<PersonDataStruct, 0>::call();
//
//   qDebug() << "Field: " << fieldName<PersonDataStruct, PersonDef::id>();

//   Mdt::CoreApplication app(argc, argv);
//   MemoryVehicleTypeRepositoryTest test;
//
// //   app.debugEnvironnement();
//
//   return QTest::qExec(&test, argc, argv);
}

