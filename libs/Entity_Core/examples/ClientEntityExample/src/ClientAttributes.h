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
#ifndef CLIENT_ATTRIBUTES_H
#define CLIENT_ATTRIBUTES_H

#include "Client.h"

/**
 * What:
 *  A) Create a value class, which implement getters/setters (pros: checks, assertions, some business logic, ...)
 *  B) Generate easely a QObject based class, which contains properties:
 *    - Reuse (f.ex. in QML ?)
 *    - Q_PROPERTY defines:
 *     o The type of the member
 *     o Name of the member
 *     o Getter, setter methods
 *  C) Maybe, create easely a assotioation class with attributes, like:
 *    - Max length of a property
 *    - Is a property part of a unique identifier
 *    - ...
 *  D) In the storage system (like SQL):
 *    - Create a assossiation class with specific attribues
 *    - Reuse common known attributes from above (cover common cases)
 *    - Provide additionnal specific attributes
 *
 * Note: maybe a MACRO to generate A) and B) in a single shot ? No! not flexible for the user..
 *
 * Note: B) what about multiple inheritance ? See below !
 */
class Client
{
 public:

  void setId(long int id);
  long int id() const;

 private:

  long int mId = 0;
};

class ClientObject : public QObject, public Client
{
 Q_OBJECT

 public:

  Q_PROPERTY(long int id, READ id, WRITE setId)

  // Maybe notifier signals and additionnal stuff that requires QObject
};
/// \todo Check if this works ! (should..)

/**
 * Store:
 *  Opt1)
 *   ClientObject cli1:
 *   cli1.setId(26);
 *   Storage::store(cli1);
 *  Opt2)
 *   Client cli2;
 *   cli2.setId(36);
 *   ClientObject cli2o;
 *   Cli2o.setData(cli2);    <- Hmm.. how ?
 *   Storage::store(cli2o);
 *
 * Query:
 *  
 */

/// Check how to use in queries (SQL..) with type safety

/// Inspiration: https://github.com/KDAB/sqlate

/// Maybe: convert to Mdt::Sql::Schema::Table ?

/// \todo boff!!
MDT_ENTITY_CREATE_ATTRIBUTES(
  Client,
  MDT_ENTITY_MEMEBER_PROPERTY(long int id, READ id, WRITE setId, isPartOfUniqueIdentifier),
  MDT_ENTITY_MEMEBER_PROPERTY(QString firstName, READ firstName, WRITE setFirstName, isUnique, uniqueGroup?, MAX_LENGTH 250)
)

/*
 */

struct ClientData
{
  long int id = 0;
  QString firstName;
};

class Client
{
 public:

  void setId(long int id);

 private:

  ClientData mData;
};


// Some magic..
// Result to:
class ClientObjectBase : public QObject
{
 Q_OBJECT

 public:

  Q_PROPETY(long int id READ id WRITE setId)

  void setId(long int id);

};

// Then attributes map

class ClientAttributes
{
};

#endif // #ifndef CLIENT_ATTRIBUTES_H
