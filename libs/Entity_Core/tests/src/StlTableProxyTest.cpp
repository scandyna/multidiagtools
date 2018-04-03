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
#include "StlTableProxyTest.h"
#include "Mdt/Entity/StlTableProxy.h"
#include "Mdt/Entity/StlTableProxyImplementationInterface.h"
#include "Mdt/Entity/Def.h"
#include "Mdt/Entity/DataTemplate.h"
#include "Mdt/Entity/FieldDataValidator.h"
#include "Mdt/Entity/RowResizableTableModel.h"
#include "Mdt/TestLib/ItemModel.h"
#include <QString>
#include <QStringList>
#include <vector>
#include <iterator>
#include <utility>
#include <algorithm>

#include "Mdt/Entity/ReadOnlyTableModel.h"
#include "Mdt/Entity/EditableTableModel.h"

#include <QDebug>

using namespace Mdt::Entity;
using namespace Mdt::TestLib;

/*
 * Entities and data
 */

struct PersonDataStruct
{
  qulonglong id;
  QString firstName;
  QString lastName;
};

MDT_ENTITY_DEF(
  (PersonDataStruct),
  Person,
  (id, FieldFlag::IsPrimaryKey),
  (firstName, FieldMaxLength(5)),
  (lastName, FieldMaxLength(10))
)

class PersonData : public Mdt::Entity::DataTemplate<PersonEntity>
{
 public:

  void setFirstName(const QString & name)
  {
    dataStruct().firstName = name.trimmed();
    Q_ASSERT(!firstName().isEmpty());
  }

  QString firstName() const
  {
    return constDataStruct().firstName;
  }

  void setLastName(const QString & name)
  {
    dataStruct().lastName = name.trimmed();
    Q_ASSERT(!lastName().isEmpty());
  }

  QString lastName() const
  {
    return constDataStruct().lastName;
  }
};

/*
 * Business logic
 */

class EditPerson
{
 public:

  bool setFirstName(const QString & name)
  {
    if( !mFieldDataValidator.validateData(name, mData.def().firstName()) ){
      return false;
    }
    mData.setFirstName(name);
    return true;
  }

  QString firstName() const
  {
    return mData.firstName();
  }

  bool setLastName(const QString & name)
  {
    if( !mFieldDataValidator.validateData(name, mData.def().lastName()) ){
      return false;
    }
    mData.setLastName(name);
    return true;
  }

  QString lastName() const
  {
    return mData.lastName();
  }

  bool setData(const PersonData & data)
  {
    if(!setFirstName(data.firstName())){
      return false;
    }
    if(!setLastName(data.lastName())){
      return false;
    }
    return true;
  }

  PersonData data() const
  {
    return mData;
  }

 private:

  PersonData mData;
  FieldDataValidator<> mFieldDataValidator;
};

/*
 * Repository
 */

// class AbstractPersonRepository
// {
//  public:
// 
//   virtual ~AbstractPersonRepository() = default;
// };

using MemoryPersonRepository = std::vector<PersonData>;

// class MemoryPersonRepository : public AbstractPersonRepository
// {
//  public:
// 
//   int rowCount() const
//   {
//     return mList.size();
//   }
// 
//  private:
// 
//   std::vector<PersonData> mList;
// };

/*
 * STL Proxy
 */

class EditPersonTableProxyImpl /* : public Mdt::Entity::StlTableProxyImplementationInterface */
{
 public:

  int rowCount() const /* override */
  {
    return repository.size();
  }

  const PersonData & recordAt(int row) const
  {
    Q_ASSERT(row >= 0);
    Q_ASSERT( row < rowCount() );
    return repository[row];
  }

//   PersonData && recordAt(int row)
//   {
//     Q_ASSERT(row >= 0);
//     Q_ASSERT( row < rowCount() );
// //     return *std::next(repository.begin(), row);
//     return std::move( repository[row] );
//   }

  void setRecordAt(int row, const PersonData & data)
  {
    Q_ASSERT(row >= 0);
    Q_ASSERT( row < rowCount() );

    if(mEditPerson.setData(data)){
      repository[row] = mEditPerson.data();
    }
  }

  MemoryPersonRepository repository;
  EditPerson mEditPerson;
};

using EditPersonTableProxy = Mdt::Entity::StlTableProxy<PersonData, EditPersonTableProxyImpl>;

///class EditPersonTableModel : Mdt::Entity::RowResizableTableModel<EditPersonTableProxy>
///class EditPersonTableModel : public Mdt::Entity::EditableTableModel<EditPersonTableProxy>
class EditPersonTableModel : public Mdt::Entity::ReadOnlyTableModel<EditPersonTableProxy>
{
 public:

  ///using ParentClass = Mdt::Entity::RowResizableTableModel<EditPersonTableProxy>;
  using ParentClass = Mdt::Entity::ReadOnlyTableModel<EditPersonTableProxy>;
//   using ParentClass::ParentClass;

  EditPersonTableModel()
  {
    // table().impl().setRepository(...);
   /// auto *impl = new EditPersonTableProxyImpl;
   // table().setImplementation(impl);
  }
};

/*
 * Helpers
 */

PersonData createPersonData(const QString & nameBase)
{
  PersonData data;

  data.setFirstName("f" + nameBase);
  data.setLastName("l" + nameBase);

  return data;
}

void fillMemoryPersonRepository(MemoryPersonRepository & repository, const QStringList & nameBaseList)
{
  repository.clear();
  for(const auto & nameBase : nameBaseList){
    repository.push_back( createPersonData(nameBase) );
  }
}

/*
 * Tests
 */

void StlTableProxyTest::iteratorDereferenceProxyTest()
{
  using dereference_proxy_type = StlTableProxyIteratorDereferenceProxy<PersonData, EditPersonTableProxyImpl>;

  PersonData data;
  EditPersonTableProxyImpl impl;
  fillMemoryPersonRepository(impl.repository, {"A","B"});

  dereference_proxy_type dProxy0(0, &impl);
  data = dProxy0;
  QCOMPARE(data.firstName(), QString("fA"));
  QCOMPARE(data.lastName(), QString("lA"));
  data.setFirstName("  EfA  ");
  dProxy0 = data;
  QCOMPARE(data.firstName(), QString("EfA"));
  QCOMPARE(data.lastName(), QString("lA"));
}

template<typename Iterator>
void iteratorCommonRequirementTestImpl()
{
  using value_type = typename Iterator::value_type;

  value_type data;
  EditPersonTableProxyImpl impl;
  fillMemoryPersonRepository(impl.repository, {"A","B"});
  Iterator it(0, &impl);
  /*
   * Dereference
   */
  data = *it;
  QCOMPARE(data.firstName(), QString("fA"));
  QCOMPARE(data.lastName(), QString("lA"));
  /*
   * Increment
   */
  ++it;
  data = *it;
  QCOMPARE(data.firstName(), QString("fB"));
  QCOMPARE(data.lastName(), QString("lB"));
  /*
   * Copy construct
   */
  Iterator it1(it);
  data = *it1;
  QCOMPARE(data.firstName(), QString("fB"));
  QCOMPARE(data.lastName(), QString("lB"));
  /*
   * Copy assign
   */
  Iterator it2(0, &impl);
  data = *it2;
  QCOMPARE(data.firstName(), QString("fA"));
  QCOMPARE(data.lastName(), QString("lA"));
  it2 = it1;
  data = *it2;
  QCOMPARE(data.firstName(), QString("fB"));
  QCOMPARE(data.lastName(), QString("lB"));
  /*
   * Swap
   */
  Iterator itA(0, &impl);
  Iterator itB(1, &impl);
  value_type data1 = *itA;
  value_type data2 = *itB;
  QCOMPARE(data1.firstName(), QString("fA"));
  QCOMPARE(data2.firstName(), QString("fB"));
  std::swap(data1, data2);
  QCOMPARE(data1.firstName(), QString("fB"));
  QCOMPARE(data2.firstName(), QString("fA"));
}

void StlTableProxyTest::iteratorRequirementTest()
{
  using const_iterator = StlTableProxyConstIterator<PersonData, EditPersonTableProxyImpl>;
  using iterator = StlTableProxyIterator<PersonData, EditPersonTableProxyImpl>;

  iteratorCommonRequirementTestImpl<const_iterator>();
  iteratorCommonRequirementTestImpl<iterator>();

  /*
   * Dereference iterator for write
   */
  using value_type = typename iterator::value_type;

  value_type data;
  EditPersonTableProxyImpl impl;
  fillMemoryPersonRepository(impl.repository, {"A"});
  iterator it(0, &impl);
  data = *it;
  QCOMPARE(data.firstName(), QString("fA"));
  QCOMPARE(data.lastName(), QString("lA"));
  // Set invalid data (to long first name)
  data.setFirstName("123456789");
  *it = data;
  data = *it;
  QCOMPARE(data.firstName(), QString("fA"));
  // Set valid data
  data.setFirstName("EfA");
  *it = data;
  data = *it;
  QCOMPARE(data.firstName(), QString("EfA"));

  /*
   * Swap
   */
  fillMemoryPersonRepository(impl.repository, {"A","B"});
  iterator it1(0, &impl);
  iterator it2(1, &impl);
  value_type data1 = *it1;
  value_type data2 = *it2;
  QCOMPARE(data1.firstName(), QString("fA"));
  QCOMPARE(data2.firstName(), QString("fB"));
  using std::swap;
  swap(*it1, *it2);
  data1 = *it1;
  data2 = *it2;
  QCOMPARE(data1.firstName(), QString("fB"));
  QCOMPARE(data2.firstName(), QString("fA"));
  std::iter_swap(it1, it2);;
  data1 = *it1;
  data2 = *it2;
  QCOMPARE(data1.firstName(), QString("fA"));
  QCOMPARE(data2.firstName(), QString("fB"));
  /*
   * Move
   */
  iterator it3(0, &impl);
  value_type data3 = std::move(*it3);
  QCOMPARE(data3.firstName(), QString("fA"));
  *it2 = std::move(data3);
  data2 = *it2;
  QCOMPARE(data2.firstName(), QString("fA"));
  qDebug() << "Mov...";
  *it1 = std::move( *iterator(1, &impl) );
  qDebug() << "Mov END";
  data1 = *it1;
  QCOMPARE(data1.firstName(), QString("fB"));
}

template<typename InputIterator>
void inputIteratorCommonRequirementTestImpl()
{
  using value_type = typename InputIterator::value_type;

  value_type data;
  EditPersonTableProxyImpl impl;
  fillMemoryPersonRepository(impl.repository, {"A","B","C","D"});
  /*
   * Increment
   */
  InputIterator it(0, &impl);
  data = *it;
  QCOMPARE(data.firstName(), QString("fA"));
  ++it;
  data = *it;
  QCOMPARE(data.firstName(), QString("fB"));
  it++;
  data = *it;
  QCOMPARE(data.firstName(), QString("fC"));
  data = *it++;
  QCOMPARE(data.firstName(), QString("fC"));
  data = *it;
  QCOMPARE(data.firstName(), QString("fD"));
  /*
   * Equality comparison
   */
  InputIterator it1(0, &impl);
  InputIterator it2(0, &impl);
  QVERIFY(it1 == it2);
  QVERIFY(!(it1 != it2));
  ++it2;
  QVERIFY(!(it1 == it2));
  QVERIFY(it1 != it2);
}

void StlTableProxyTest::inputIteratorRequirementTest()
{
  using const_iterator = StlTableProxyConstIterator<PersonData, EditPersonTableProxyImpl>;
  using iterator = StlTableProxyIterator<PersonData, EditPersonTableProxyImpl>;

  inputIteratorCommonRequirementTestImpl<const_iterator>();
  inputIteratorCommonRequirementTestImpl<iterator>();

  QFAIL("Missing it->m");
}

void StlTableProxyTest::outputIteratorRequirementTest()
{
  using iterator = StlTableProxyIterator<PersonData, EditPersonTableProxyImpl>;
  using value_type = typename iterator::value_type;

  value_type data;
  EditPersonTableProxyImpl impl;
  fillMemoryPersonRepository(impl.repository, {"A"});
  iterator it(0, &impl);

  data.setFirstName("EfA");
  data.setLastName("ElA");
  *it = data;
  QCOMPARE(impl.repository[0].firstName(), QString("EfA"));

  data.setFirstName("EfA2");
  data.setLastName("ElA");
  *it++ = data;
  QCOMPARE(impl.repository[0].firstName(), QString("EfA2"));
}

template<typename ForwardIterator>
void forwardIteratorCommonRequirementTestImpl()
{
  using value_type = typename ForwardIterator::value_type;

  value_type data;
  EditPersonTableProxyImpl impl;
  fillMemoryPersonRepository(impl.repository, {"A","B"});
  /*
   * Default construct
   */
  ForwardIterator itNull;
  ForwardIterator it(0, &impl);
  QVERIFY(it != itNull);
  /*
   * Multi-pass
   */
  ForwardIterator it1(0, &impl);
  ForwardIterator it2 = it1;
  QVERIFY(it1 == it2);
  data = *it1;
  QCOMPARE(data.firstName(), QString("fA"));
  ++it2;
  data = *it1;
  QCOMPARE(data.firstName(), QString("fA"));
}

void StlTableProxyTest::forwardIteratorRequirementTest()
{
  using const_iterator = StlTableProxyConstIterator<PersonData, EditPersonTableProxyImpl>;
  using iterator = StlTableProxyIterator<PersonData, EditPersonTableProxyImpl>;

  forwardIteratorCommonRequirementTestImpl<const_iterator>();
  forwardIteratorCommonRequirementTestImpl<iterator>();
}

template<typename BidirectionalIterator>
void bidirectionalIteratorCommonRequirementTestImpl()
{
  using value_type = typename BidirectionalIterator::value_type;

  value_type data;
  EditPersonTableProxyImpl impl;
  fillMemoryPersonRepository(impl.repository, {"A","B","C","D"});
  /*
   * Decrement
   */
  BidirectionalIterator it(3, &impl);
  data = *it;
  QCOMPARE(data.firstName(), QString("fD"));
  --it;
  data = *it;
  QCOMPARE(data.firstName(), QString("fC"));
  it--;
  data = *it;
  QCOMPARE(data.firstName(), QString("fB"));
  data = *it--;
  QCOMPARE(data.firstName(), QString("fB"));
}

void StlTableProxyTest::bidirectionalIteratorRequirementTest()
{
  using const_iterator = StlTableProxyConstIterator<PersonData, EditPersonTableProxyImpl>;
  using iterator = StlTableProxyIterator<PersonData, EditPersonTableProxyImpl>;

  bidirectionalIteratorCommonRequirementTestImpl<const_iterator>();
  bidirectionalIteratorCommonRequirementTestImpl<iterator>();
}

template<typename RandomAccessIterator>
void randomAccessIteratorCommonRequirementTestImpl()
{
  using value_type = typename RandomAccessIterator::value_type;

  value_type data;
  EditPersonTableProxyImpl impl;
  fillMemoryPersonRepository(impl.repository, {"A","B","C","D"});

  RandomAccessIterator it(1, &impl);
  data = *it;
  QCOMPARE(data.firstName(), QString("fB"));
  it += 2;
  data = *it;
  QCOMPARE(data.firstName(), QString("fD"));
  it += -1;
  data = *it;
  QCOMPARE(data.firstName(), QString("fC"));
  auto it1 = it + 1;
  data = *it1;
  QCOMPARE(data.firstName(), QString("fD"));
  data = *it;
  QCOMPARE(data.firstName(), QString("fC"));
  it1 = -1 + it;
  data = *it1;
  QCOMPARE(data.firstName(), QString("fB"));
  data = *it;
  QCOMPARE(data.firstName(), QString("fC"));
  it -= 1;
  data = *it;
  QCOMPARE(data.firstName(), QString("fB"));
  it1 = it - 1;
  data = *it1;
  QCOMPARE(data.firstName(), QString("fA"));
  data = *it;
  QCOMPARE(data.firstName(), QString("fB"));
  QCOMPARE(it - it1, 1);
  /*
   * Comparison
   */
  RandomAccessIterator it2(0, &impl);
  it1 = it2;
  QVERIFY(!(it1 < it2));
  QVERIFY(it1 <= it2);
  QVERIFY(!(it1 > it2));
  QVERIFY(it1 >= it2);
  ++it2;
  QVERIFY(it1 < it2);
  QVERIFY(it1 <= it2);
  QVERIFY(it2 > it1);
  QVERIFY(it2 >= it1);
}

void StlTableProxyTest::randomAccessIteratorRequirementTest()
{
  using const_iterator = StlTableProxyConstIterator<PersonData, EditPersonTableProxyImpl>;
  using iterator = StlTableProxyIterator<PersonData, EditPersonTableProxyImpl>;

  randomAccessIteratorCommonRequirementTestImpl<const_iterator>();
  randomAccessIteratorCommonRequirementTestImpl<iterator>();

  QFAIL("Missing it[n]");
}

void StlTableProxyTest::simpleSetGetTest()
{
  using value_type = EditPersonTableProxy::value_type;

  value_type data;
  EditPersonTableProxy proxy;
  QCOMPARE(proxy.rowCount(), 0);
  QCOMPARE(std::distance(proxy.cbegin(), proxy.cend()), 0);

  fillMemoryPersonRepository(proxy.impl().repository, {"A"});
  QCOMPARE(proxy.rowCount(), 1);
  QCOMPARE(std::distance(proxy.cbegin(), proxy.cend()), 1);
  data = *proxy.cbegin();
  QCOMPARE(data.firstName(), QString("fA"));
  data = *std::prev(proxy.cend(), 1);
  QCOMPARE(data.firstName(), QString("fA"));

  fillMemoryPersonRepository(proxy.impl().repository, {"A","B","C","D"});
  QCOMPARE(proxy.rowCount(), 4);
  QCOMPARE(std::distance(proxy.cbegin(), proxy.cend()), 4);
  data = *proxy.cbegin();
  QCOMPARE(data.firstName(), QString("fA"));
  data = *std::prev(proxy.cend(), 1);
  QCOMPARE(data.firstName(), QString("fD"));

  auto it1 = std::next(proxy.begin(), 1);
  data = *it1;
  QCOMPARE(data.firstName(), QString("fB"));
  data.setFirstName("EfB");
  *it1 = data;
  QCOMPARE(proxy.impl().repository[1].firstName(), QString("EfB"));
}

void StlTableProxyTest::stlAlgorithmTest()
{
  using value_type = EditPersonTableProxy::value_type;

  value_type data;
  EditPersonTableProxy proxy;
  fillMemoryPersonRepository(proxy.impl().repository, {"A","C","B"});
  QCOMPARE(proxy.rowCount(), 3);

//   auto appendString = [](value_type & data){
//     
//   };
//   std::for_each(proxy.begin(), proxy.end(), appendString);
  
  auto cmp = [](const value_type & a, const value_type & b){
    ///return a.firstName() < b.firstName();
    return (QString::compare(a.firstName(), b.firstName()) < 0);
  };
  qDebug() << "sort..";
  std::sort(proxy.begin(), proxy.end(), cmp);
  qDebug() << "sort END";
  QCOMPARE(proxy.impl().repository[0].firstName(), QString("fA"));
  QCOMPARE(proxy.impl().repository[1].firstName(), QString("fB"));
  QCOMPARE(proxy.impl().repository[2].firstName(), QString("fC"));

  QFAIL("Not complete");
}

void StlTableProxyTest::editTableModelTest()
{
  EditPersonTableModel model;
  fillMemoryPersonRepository(model.table().impl().repository, {"A","B"});
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 1), QVariant("fA"));
  QCOMPARE(getModelData(model, 0, 2), QVariant("lA"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("fB"));
  QCOMPARE(getModelData(model, 1, 2), QVariant("lB"));
  QVERIFY(setModelData(model, 0, 1, "  EfA "));
  QVERIFY(setModelData(model, 0, 2, "  ElA "));
  QCOMPARE(getModelData(model, 0, 1), QVariant("EfA"));
  QCOMPARE(getModelData(model, 0, 2), QVariant("ElA"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("fB"));
  QCOMPARE(getModelData(model, 1, 2), QVariant("lB"));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  StlTableProxyTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
