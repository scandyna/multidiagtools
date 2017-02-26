/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#ifndef MDT_ITEM_EDITOR_CONTROLLER_RELATION_IMPL_BASE_H
#define MDT_ITEM_EDITOR_CONTROLLER_RELATION_IMPL_BASE_H

#include "AbstractControllerRelation.h"
#include "Mdt/ItemModel/RelationKey.h"
#include <QPointer>

namespace Mdt{ namespace ItemEditor{

  /*! \brief Relation between a parent controller and a child controller
   *
   * ControllerRelationImplBase is a class template used by ControllerRelationList
   *  to handle relations between controllers.
   *
   * To be usefull, a class that implements some of concrete controller's
   *  method in a specific way must be created as subclass of this.
   *
   * The implementation subclass must implement pure virtual methods
   *  and also provide following ones:
   * \code
   * // Set implementation specific filter
   * void setSpecificRelationFilter(const Filter & conditions);
   *
   * // This method is called when parent controller's model changed.
   * void setParentControllerModelToChildController();
   * \endcode
   * \todo Check if setParentControllerModelToChildController() is not obelete
   *
   * ControllerRelationImplBase does not own any controller,
   *  deleting them is the responsability of the user of this class.
   *
   * \tparam Controller Must be AbstractController or subclass
   * \tparam Derived Class that inherits from this and that implements specific methods
   */
  template<typename Controller, typename Derived>
  class ControllerRelationImplBase : public AbstractControllerRelation
  {
   public:

    /*! \brief Construct a relation
     *
     * \pre \a parentController must be a valid pointer
     */
    explicit ControllerRelationImplBase(Controller *parentController, QObject *parent = nullptr)
       : AbstractControllerRelation(parentController, parent),
         mParentController(parentController)
    {
    }

    /*! \brief Get parent controller
     */
    Controller *parentController() const
    {
      return mParentController;
    }

    /*! \brief Get child controller
     */
    Controller *childController() const
    {
      return mChildController;
    }

    /*! \brief Set child controller
     *
     * \pre \a controller must be a valid pointer
     */
    void setChildController(Controller *controller)
    {
      Q_ASSERT(controller != nullptr);

      registerAbstractChildController(controller);
      if(!mChildController.isNull()){
        unregisterChildController();
      }
      mChildController = controller;
      registerChildController();
    }

    /*! \brief Set implementation specific relation filter
     *
     * \pre \a conditions must be a valid relation filter expression
     */
    template<typename T>
    void setRelationFilter(const T & conditions)
    {
      impl()->setSpecificRelationFilter(conditions);
    }

    /*! \brief Set relation filter from parent controller's primary key and child controller's foreign key
     *
     * \pre child controller must allready been set
     * \pre parent controller must have a non null primary key set
     * \pre child controller must have a non null foreign key set
     * \pre Both primary of parent controller and foreign key of child controller must have the same count of columns, and max 4
     */
    void setRelationFilterFromPkFk()
    {
      Q_ASSERT(childController() != nullptr);
      Q_ASSERT(!parentController()->getPrimaryKey().isNull());
      Q_ASSERT(!childController()->getForeignKey().isNull());
      Q_ASSERT(parentController()->getPrimaryKey().columnCount() == childController()->getForeignKey().columnCount());

      Mdt::ItemModel::RelationKey key;
      key.setKey( parentController()->getPrimaryKey(), childController()->getForeignKey() );
      setFilterFromRelationKey(key);
    }

   protected:

    /*! \brief Register child controller
     *
     * Subclass must implement this function to
     *  make signal/slot connections and whatever is needed.
     */
    virtual void registerChildController() = 0;

    /*! \brief Unregister child controller
     *
     * Subclass must implement this function to
     *  disconnect signal/slot and whatever is needed.
     */
    virtual void unregisterChildController() = 0;

    /*! \brief Set relation filter from relation key
     */
    virtual void setFilterFromRelationKey(const Mdt::ItemModel::RelationKey & key) = 0;

   private:

    /*! \brief Get parent controller
     */
    AbstractController *abstractParentController() const override
    {
      return mParentController;
    }

    /*! \brief Get child controller
     */
    AbstractController* abstractChildController() const override
    {
      return mChildController;
    }

    Derived *impl()
    {
      return static_cast<Derived*>(this);
    }

    QPointer<Controller> mParentController;
    QPointer<Controller> mChildController;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_CONTROLLER_RELATION_IMPL_BASE_H
