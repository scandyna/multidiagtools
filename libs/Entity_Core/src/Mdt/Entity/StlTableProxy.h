/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_ENTITY_STL_TABLE_PROXY_H
#define MDT_ENTITY_STL_TABLE_PROXY_H

#include "StlTableProxyIterator.h"
#include "StlTableProxyImplementationInterface.h"
#include <memory>

namespace Mdt{ namespace Entity{

  /*! \brief STL iteratator based table proxy
   *
   * To view and edit a collection of data,
   *  Qt provides the item/model framework.
   *
   * To create a custom table model,
   *  that applies business rules and get/put data to a custom repository,
   *  subclassing QAbstractTableModel could be a good start.
   *  The problem when doing so is to allways implement the same methods
   *  for each custom model ( %rowCount(), %columnCount(), %data(), %insertRows(), ... ).
   *  To reduce the amount of code to rewrite, some item model templates can be used:
   *  - ReadOnlyTableModel
   *  - EditableTableModel
   *  - RowResizableTableModel
   *
   *  Those models woks on STL compatible containser (like std::vector or DataList based ones).
   *
   * If a class, having business logic to edit a person, exists:
   * \code
   * class EditPerson
   * {
   *  public:
   *
   *   bool setFirstName(const QString & name);
   *   QString firstName() const;
   *   bool setLastName(const QString & name);
   *   QString lastName() const;
   * };
   * \endcode
   *
   * The item model can probably also read the data directly from a repository (or a data structure in memory):
   * \code
   * class AbstractPersonRepository
   * {
   *  public:
   *
   *   
   * };
   * \endcode
   *
   * To glue all things together, a adapter implementation should be created:
   * \code
   * class EditPersonTableProxyImpl : public Mdt::Entity::StlTableProxyImplementationInterface
   * {
   * };
   * \endcode
   *
   * Finally, we can create the item model:
   * \code
   * using EditPersonTableProxy = Mdt::Entity::StlTableProxy<PersonData, EditPersonTableProxyImpl>;
   *
   * class EditPersonTableModel : public Mdt::Entity::RowResizableTableModel<EditPersonTableProxy>
   * {
   *  Q_OBJECT
   *
   *  public:
   *
   *   using ParentClass = Mdt::Entity::RowResizableTableModel<EditPersonTableProxy>;
   *   using ParentClass::ParentClass;
   * };
   * \endcode
   */
  template<typename EntityData, typename Impl>
  class StlTableProxy
  {
   public:

    /*! \brief STL style value type
     */
    using value_type = EntityData;

    /*! \brief STL style const iterator
     */
    using const_iterator = StlTableProxyConstIterator<EntityData, Impl>;

    /*! \brief STL style iterator
     */
    using iterator = StlTableProxyIterator<EntityData, Impl>;

//     StlTableProxy()
//      : mImpl()
//     {
//     }

//     void setImplementation(StlTableProxyImplementationInterface * impl)
//     {
//       Q_ASSERT(impl != nullptr);
//       mImpl.reset(impl);
//     }

//     StlTableProxy()
//      : mImpl( std::make_shared<Impl>() )
//     {
//     }

//     StlTableProxy( const std::shared_ptr<StlTableProxyImplementationInterface> & impl )
//      : mImpl( impl )
//     {
//     }

//     /*! \brief Access implementation instance
//      */
//     std::shared_ptr<Impl> impl()
//     {
//       return mImpl;
//     }

    Impl & impl()
    {
      return mImpl;
    }

    /*! \brief Get row count
     */
    int rowCount() const
    {
      return mImpl.rowCount();
    }

    /*! \brief Get a const iterator to the first element
     */
    const_iterator cbegin() const
    {
      return const_iterator(0, &mImpl);
    }

    /*! \brief Get a const iterator past the last element
     */
    const_iterator cend() const
    {
      return const_iterator( rowCount(), &mImpl );
    }

    /*! \brief Get a iterator to the first element
     */
    iterator begin()
    {
      return iterator(0, &mImpl);
    }

    /*! \brief Get a iterator past the last element
     */
    iterator end()
    {
      return iterator( rowCount(), &mImpl );
    }

   private:

//     std::unique_ptr<StlTableProxyImplementationInterface> mImpl;
    Impl mImpl;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_STL_TABLE_PROXY_H
