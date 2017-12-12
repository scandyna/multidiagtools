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

#include <QObject>
#include <QString>
#include <QFlags>
#include <QByteArray>
#include <QHash>


#include <QDebug>

/*! \brief Client value class
 */
class Client
{
 public:

  /*! \brief Set client Id
   */
  void setId(long int id);

  /*! \brief Get client Id
   */
  long int id() const
  {
    return mId;
  }

  void setFirstName(const QString & name);

  QString firstName() const
  {
    return mFirstName;
  }

 private:

  long int mId = 0;
  QString mFirstName;
};

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

template<typename T>
class EntityObject : public QObject, public T
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

class ClientObject : public EntityObject<Client>
{
 Q_OBJECT

 public:

  /// \todo Document that long int is not suported by the meta object
  Q_PROPERTY(qlonglong id READ id WRITE setId)
  Q_PROPERTY(QString firstName READ firstName WRITE setFirstName)

//   MDT_ENTITY_PROPERTY(qlonglong, id, PropertyAttributes(PropertyFlag::IsPartOfUniqueIdentifier), READ id, WRITE setId)
  /// MDT_ENTITY_PROPERTY()

  // Maybe notifier signals and additionnal stuff that requires QObject

  explicit ClientObject(QObject *parent = nullptr)
  {
    setPropertyAttributes("id", PropertyFlag::IsPartOfUniqueIdentifier | PropertyFlag::IsRequired);
    setPropertyAttributes("firstName", 250);
  }
};

#endif // #ifndef CLIENT_H
