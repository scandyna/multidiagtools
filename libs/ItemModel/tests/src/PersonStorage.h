/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef PERSON_STORAGE_H
#define PERSON_STORAGE_H

#include <QString>
#include <QMap>

struct Person
{
  int id = 0;
  QString name;
};

class PersonStorage
{
 public:

  void add(Person person);

  void update(const Person & person);

  void remove(int id);

  int count() const
  {
    return mMap.count();
  }

  bool hasId(int id) const
  {
    Q_ASSERT(id > 0);
    return mMap.contains(id);
  }

  Person getById(int id) const
  {
    Q_ASSERT(id > 0);
    Q_ASSERT(hasId(id));

    return mMap.value(id);
  }

  QString nameById(int id) const
  {
    Q_ASSERT(id > 0);
    Q_ASSERT(hasId(id));

    return getById(id).name;
  }

 private:

  int nextId() const;

  QMap<int, Person> mMap;
};

#endif // #ifndef PERSON_STORAGE_H
