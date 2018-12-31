
#include "TestBase.h"

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
 *  In a context of  database, we also need to create some schema first.
 *
 * The goal is to avoid creating all the necessary stuff to create
 *  a SQL databse schema, SQL queries, etc..
 *
 * The domain entity, %Person, should ideally not be affected for reflection.
 *
 * Current C++ (C++17) offers no mechanism for reflection.
 *  There is no way, for example, to travel a class and query
 *  there members (like member name).
 *  To create reflection, we can use tuples and template magic.
 *
 * \section non_intrusive_reflection Non intrusive reflection
 *
 * First, we create a data struct that will be used ase base:
 * \code
 * struct PersonDataStruct
 * {
 *   qulonlong id = 0;
 *   QString firstName;
 *   QString lastName;
 * };
 * \endcode
 *
 * Whe should also provide functions to convert between %Person and %PersonDataStruct:
 * \code
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
 * bool SqlPesronRepository::add(const Person & person)
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
 * First, we create a data struct that will be used ase base:
 * \code
 * struct PersonDataStruct
 * {
 *   qulonlong id = 0;
 *   QString firstName;
 *   QString lastName;
 * };
 * \endcode
 *
 * The above data struct will now be used as member in the domain object:
 * \code
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
 * bool SqlPesronRepository::add(const Person & person)
 * {
 *   return mImpl.add<PersonDef>(person.constDataStruct());
 * }
 * \endcode
 *
 * Here, the domain entity has knowlage of reflection tricks.
 *  This could maybe bring some less overhead, because there are
 *  no data structs to instanciante and copies for each repository operations.
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
 * bool SqlPesronRepository::add(const Person & person)
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
 *  we probably will have to provide some some stuff to make those value objects serializable.
 *
 * Here, the domain entity is strongly coupled with the reflection details.
 *
 *
 * \section attributes
 *
 * Some attributes could be specific to some persistance engine.
 *  A example could be the primary key in a SQL database.
 *
 * Such attributes could be defined, for example,
 *  in some SQL schema:
 * \code
 * using PersonPrimaryKey = Mdt::Reflection::PrimaryKey<PersonDef, PersonDef::id>;
 * \endcode
 *
 * Some other attributes could also have meaning in the domain busines logic.
 *  For example, if some data is required, this could be defined in one place,
 *  the reused on the domain logic, in the UI and in the persistance
 *  (f.ex. generating corresponding constraints on the databse schema).
 *
 *
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

#include <QDebug>

/*!
 */

struct PersonDataStruct
{
  int id = 0;
  QString firstName;
  QString lastName;
};

template<typename T>
constexpr const char *reflectedClassName()
{
//   static_assert(false, "Not a reflected class");
  return "";
}

template<>
constexpr const char *reflectedClassName<PersonDataStruct>()
{
  return "Person";
}

struct PersonDef
{
  using ReflectedClass = PersonDataStruct;

  static constexpr const char *name()
  {
    return reflectedClassName<ReflectedClass>();
  }

  struct id
  {
    static constexpr const char *name()
    {
      return "id";
    }
  };

  struct firstName
  {
    static constexpr const char *name()
    {
      return "id";
    }
  };

  struct lastName
  {
    static constexpr const char *name()
    {
      return "id";
    }
  };
};

BOOST_FUSION_ADAPT_ASSOC_STRUCT(
  PersonDataStruct,
  (id, PersonDef::id)
  (firstName, PersonDef::firstName)
  (lastName, PersonDef::lastName)
)

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

template<>
constexpr const char *reflectedClassName<Person>()
{
  return "Person";
}



template<typename Def>
static constexpr const char* nameFromDef()
{
  return Def::name();
}

template<typename Class>
static constexpr const char* nameFromClass()
{
  return reflectedClassName<Class>();
}


template<typename Field>
static constexpr const char* fieldName()
{
  return Field::name();
//   return boost::fusion::extension::struct_member_name<DataStruct, Field>::call();
}

struct saver
{
  template<typename FieldValue>
  void operator()(const FieldValue & value) const
  {
    qDebug() << value;
//     qDebug() << "Field: " << Field::name(); /* << ", value: " << boost::fusion::at_key<Field>(field); */
  }
};

/*
 * Main
 */

int main(int argc, char **argv)
{
  Person pa("fA", "lA");

  qDebug() << "Name: " << nameFromDef<PersonDef>();
  qDebug() << "Name: " << nameFromClass<Person>();
  qDebug() << "id name: " << fieldName<PersonDef::id>();

  boost::fusion::for_each( personPrivateConstDataStruct(pa), saver());

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

