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
#include "PersonStorage.h"
#include <algorithm>
#include <iterator>

int PersonStorage::add(Person person)
{
  if(person.id == 0){
    person.id = nextId();
  }
  Q_ASSERT(!hasId(person.id));

  mMap.insert(person.id, person);

  return person.id;
}

void PersonStorage::populate(std::initializer_list<Person> list)
{
  clear();
  for(const Person & person : list){
    add(person);
  }
}

void PersonStorage::populateByNames(const QStringList& names)
{
  clear();
  for(const QString & name : names){
    Person person;
    person.name = name;
    add(person);
  }
}

void PersonStorage::update(const Person& person)
{
  Q_ASSERT(person.id > 0);
  Q_ASSERT(hasId(person.id));

  mMap[person.id] = person;
}

void PersonStorage::remove(int id)
{
  Q_ASSERT(id > 0);
  Q_ASSERT(hasId(id));

  mMap.remove(id);
}

void PersonStorage::clear()
{
  mMap.clear();
}

std::vector<Person> PersonStorage::getCountPersons(int num) const
{
  std::vector<Person> persons;

  const int n = std::min(num, count());
  std::copy_n(mMap.cbegin(), n, std::back_inserter(persons));

  return persons;
}

int PersonStorage::nextId() const
{
  if(mMap.isEmpty()){
    return 1;
  }
  return mMap.lastKey() + 1;
}

/*
 * PersonPendingTasks
 */

// void PersonPendingTasks::submitTask(int taskId, const Person& person)
// {
//   Q_ASSERT(taskId > 0);
//   Q_ASSERT(!hasTask(taskId));
// 
//   mPendingTasks.insert(taskId, person);
// }
