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

};

class ClientObject : public EntityObject<Client>
{
 Q_OBJECT

 public:

  /// \todo Document that long int is not suported by the meta object
  Q_PROPERTY(qlonglong id READ id WRITE setId)
  Q_PROPERTY(QString firstName READ firstName WRITE setFirstName)

  // Maybe notifier signals and additionnal stuff that requires QObject
};

#endif // #ifndef CLIENT_H
