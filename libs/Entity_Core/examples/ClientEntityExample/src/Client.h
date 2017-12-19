/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#ifndef CLIENT_H
#define CLIENT_H

#include <Mdt/Entity/Field.h>
#include <QObject>
#include <QString>
#include <QFlags>
#include <QByteArray>
#include <QHash>

#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>

#include <boost/preprocessor/array/data.hpp>

#include <boost/preprocessor/punctuation/comma_if.hpp>

#include <array>

#include <QDebug>

enum class PropertyFlag
{
  IsPartOfUniqueIdentifier = 0x01,
  IsRequired = 0x02
};
Q_DECLARE_FLAGS(PropertyFlags, PropertyFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(PropertyFlags)

class PropertyAttributes
{
 public:

  PropertyAttributes() = default;

  PropertyAttributes(PropertyFlags flags, int maxLength = -1)
   : mFlags(flags),
     mMaxLength(maxLength)
  {
  }

  PropertyFlags flags() const
  {
    return mFlags;
  }

  int maxLength() const
  {
    return mMaxLength;
  }

//   bool isNull() const
//   {
//   }

 private:

  PropertyFlags mFlags;
  int mMaxLength = -1;
};

/*! \brief Client data struct
 *
 * This struct will contains fields,
 *  which provides value and a fieldName() static function.
 */
struct ClientDataStruct
{
  MDT_ENTITY_FIELD(qlonglong, id, 0)
  MDT_ENTITY_FIELD(QString, firstName)
  MDT_ENTITY_FIELD(QString, lastName)
};

#define PARAM_TYPE(param) \
  BOOST_PP_TUPLE_ELEM(2, 0, param)

#define PARAM_NAME(param) \
  BOOST_PP_TUPLE_ELEM(2, 1, param)

#define PARAM_NAME_STR(param) \
  BOOST_PP_STRINGIZE( BOOST_PP_TUPLE_ELEM(2, 1, param) )

// PARAM_TYPE( (int, id22) ) PARAM_NAME( (int, id22) ) ;
// PARAM_TYPE(BOOST_PP_SEQ_ELEM(idx, members)) PARAM_NAME(BOOST_PP_SEQ_ELEM(idx, members));

#define GENEARTE_MEMBER(z, idx, members) \
  MDT_ENTITY_FIELD( PARAM_TYPE(BOOST_PP_SEQ_ELEM(idx, members)), PARAM_NAME(BOOST_PP_SEQ_ELEM(idx, members)) )

#define GENEARTE_MEMBERS(members) \
  BOOST_PP_REPEAT(BOOST_PP_SEQ_SIZE(members), GENEARTE_MEMBER, members)

/*
 * Get the field name of a element as string.
 * Note: the trailing comma in a initializer list is accepted in C++
 * (we not have to do compilcated stuff to conditionnaly add the comma or not)
 */
#define GENERATE_FIELD_NAME(z, idx, members) \
  PARAM_NAME_STR(BOOST_PP_SEQ_ELEM(idx, members)) ,

// BOOST_PP_STRINGIZE(PARAM_NAME(BOOST_PP_SEQ_ELEM(idx, members)))
// BOOST_PP_CAT( BOOST_PP_STRINGIZE(PARAM_NAME(BOOST_PP_SEQ_ELEM(idx, members))), BOOST_PP_COMMA_IF(idx) )

#define GENERATE_FIELD_NAME_LIST(members) \
  static std::array<QString, BOOST_PP_SEQ_SIZE(members)> fieldNameList() \
  { \
  return {{BOOST_PP_REPEAT(BOOST_PP_SEQ_SIZE(members), GENERATE_FIELD_NAME, members)}}; \
  }

// return {{BOOST_PP_SEQ_FOR_EACH(GENERATE_FIELD_NAME, _, members)}};

#define MDT_ENTITY_DATA_STRUCT(name, fields) \
  struct name \
  { \
    GENEARTE_MEMBERS(fields) \
  };

// GENERATE_FIELD_NAME_LIST(fields)

MDT_ENTITY_DATA_STRUCT(
  SomeDataStruct,
  ((int, id))
  ((QString, name))
)

// int id;
// static QString str0 = GENERATE_FIELD_NAME(0, 0, ((int, id)) ) ;
// static QString str1 = GENERATE_FIELD_NAME(1, 0, ((int, id)) ((int, b)) ) ;
// static QString str2 = GENERATE_FIELD_NAME(1, 1, ((int, id)) ((int, b)) ) ;
// GENERATE_FIELD_NAME_LIST( ((int, id)) )

/*! \brief Client value class
 */
class ClientData
{
 public:

  /*! \brief Set client Id
   */
  void setId(long int id);

  /*! \brief Get client Id
   */
  long int id() const
  {
    return mData.id;
  }

  /*! \brief Set first name
   */
  void setFirstName(const QString & name);

  /*! \brief Get first name
   */
  QString firstName() const
  {
    return mData.firstName;
  }

  /*! \brief Set last name
   */
  void setLastName(const QString & name);

  /*! \brief Get last name
   */
  QString lastName() const
  {
    return mData.lastName;
  }

 private:

  ClientDataStruct mData;
};

/*! \brief Base class to create a entity
 *
 * In the context of Mdt::Entity,
 *  a entity is divided in several parts:
 *  - the entity data struct, that can be used for some compile time introspection,
 *    for example to help generating a Mdt::Sql::TableTemplate
 *  - the entity data, which is a lightweight value class
 *  - TODO ADAPT maybe the entity description, which is based on EntityBase (which inherits QObject),
 *    and uses the QObject introspection to store properties.
 *    PropertyAttributes can also be assigned to properties
 *
 * TODO ADAPT The entity description can be reused by the Mdt::Sql library,
 *  for example to create a table (see Mdt::Sql::Table).
 *
 * At first, create the entity struct:
 * \code
 * struct ClientDataStruct
 * {
 *  MDT_ENTITY_COLUMN(qlonglong, id, 0)
 *  MDT_ENTITY_COLUMN(QString, firstName)
 * };
 * \endcode
 *
 * Notice that the types of each member of the struct should be known by QMetaType .
 *
 * Then, create the entity data class:
 * \code
 * class ClientData
 * {
 *  public:
 *
 *    void setId(qlonglong id);
 *    qlonglong id() const;
 *
 *  private:
 *
 *    ClientDataStruct mData;
 * };
 * \endcode
 *
 *
 * Then, create the entity description:
 * \code
 * class Client : public EntityBase<ClientData>
 * {
 *  Q_OBJECT
 *
 *  public:
 *
 *    Q_PROPERTY(qlonglong id READ id WRITE setId)
 *    Q_PROPERTY(QString firstName READ firstName WRITE setFirstName)
 *
 *    explicit Client(QObject *parent = nullptr)
 *    {
 *      setPropertyAttributes("id", PropertyFlag::IsPartOfUniqueIdentifier | PropertyFlag::IsRequired);
 *      setPropertyAttributes("firstName", 250);
 *    }
 * };
 * \endcode
 */
template<typename T>
class EntityBase : public QObject, public T
{
 public:

  void setData(const T & data)
  {
    T::operator=(data);
  }

  const T & data() const
  {
    return static_cast<const T &>(*this);
  }

  void setPropertyAttributes(const QByteArray & propertyName, const PropertyAttributes & attributes)
  {
    Q_ASSERT_X( metaObject()->indexOfProperty(propertyName.constData()) >= 0, "Entity::setPropertyAttributes(): cannot set attributes to non existing property. Passed property name", propertyName.constData());
    mPropertyAttributesMap[propertyName] = attributes;
  }

  void setPropertyAttributes(const QByteArray & propertyName, PropertyFlags flags, int maxLength = -1)
  {
    setPropertyAttributes(propertyName, PropertyAttributes(flags, maxLength));
  }

  void setPropertyAttributes(const QByteArray & propertyName, int maxLength)
  {
    setPropertyAttributes(propertyName, PropertyFlags(), maxLength);
  }

  PropertyAttributes propertyAttributes(const QByteArray & propertyName) const
  {
    return mPropertyAttributesMap.value(propertyName);
  }

 private:

  QHash<QByteArray, PropertyAttributes> mPropertyAttributesMap;
};

/// Macros: see https://github.com/liu-xiao-guo/qqmlobjectlistmodel/blob/master/qqmlhelpers.h

/*
#define MDT_ENTITY_PROPERTY(type, name, attributes, ...) \
  setPropertyAttributes(name, attributes), \
  Q_PROPERTY(type name __VA_ARGS__)
*/

/**
 * moc does not support expanding somewhat complex macros, we cannot generate properties
 * see: https://stackoverflow.com/questions/32612309/using-a-macro-to-create-qobject-derived-classes
 */

/*
#define GENEARTE_PROPERTY(z, idx, fields) \
  Q_PROPERTY( PARAM_TYPE(BOOST_PP_SEQ_ELEM(idx, fields)) PARAM_NAME(BOOST_PP_SEQ_ELEM(idx, fields)) READ PARAM_NAME(BOOST_PP_SEQ_ELEM(idx, fields)) WRITE BOOST_PP_CAT(set, PARAM_NAME(BOOST_PP_SEQ_ELEM(idx, fields))) )

#define GENEARTE_PROPERTIES(fields) \
  BOOST_PP_REPEAT(BOOST_PP_SEQ_SIZE(fields), GENEARTE_PROPERTY, fields)
*/

#define MDT_ENTITY_BASE(name, DataType, fields) \
  class name : public EntityBase<DataType> \
  { \
   Q_OBJECT \
   public: \
    GENERATE_FIELD_NAME_LIST(fields) \
  };

// GENERATE_FIELD_NAME_LIST(fields)

// Q_PROPERTY( int hardCoded READ hardCoded WRITE sethardCoded )

MDT_ENTITY_DATA_STRUCT(
  MyEntityDataStruct,
  ((int, id))
)

class MyEntityData
{
 public:

  void setid(int id)
  {
    mData.id = id;
  }

  int id() const
  {
    return mData.id;
  }

 private:

  MyEntityDataStruct mData;
};

MDT_ENTITY_BASE(
  MyEntityBase, MyEntityData,
  ((int, id))
  ((QString, name))
)

#define MDT_ENTITY(name, fields) \
  MDT_ENTITY_DATA_STRUCT( BOOST_PP_CAT(name, DataStruct), fields )

// MDT_ENTITY(
//   MyEntity,
//   ((int, id))
// )

class Client : public EntityBase<ClientData>
{
 Q_OBJECT

 public:

  /// \todo Document that long int is not suported by the meta object
  Q_PROPERTY(qlonglong id READ id WRITE setId)
  Q_PROPERTY(QString firstName READ firstName WRITE setFirstName)

//   MDT_ENTITY_PROPERTY(qlonglong, id, PropertyAttributes(PropertyFlag::IsPartOfUniqueIdentifier), READ id, WRITE setId)
  /// MDT_ENTITY_PROPERTY()

  // Maybe notifier signals and additionnal stuff that requires QObject

  explicit Client(/**QObject *parent = nullptr*/)
   /// : EntityBase(parent)
  {
    setPropertyAttributes("id", PropertyFlag::IsPartOfUniqueIdentifier | PropertyFlag::IsRequired);
    setPropertyAttributes("firstName", 250);
  }
};

#endif // #ifndef CLIENT_H
