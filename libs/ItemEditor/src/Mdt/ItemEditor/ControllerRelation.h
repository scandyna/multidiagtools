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
#ifndef MDT_ITEM_EDITOR_CONTROLLER_RELATION_H
#define MDT_ITEM_EDITOR_CONTROLLER_RELATION_H

#include <QPointer>

namespace Mdt{ namespace ItemEditor{

  /*! \brief Relation between a parent controller and a child controller
   *
   * ControllerRelation is a class template used by ControllerRelationList
   *  to handle relations between controllers.
   *
   * To be usefull, a class that implements some of concrete controller's
   *  method in a specific way must be created as subclass of this.
   *
   *  The implementation subclass must provide following methods:
   * \code
   * // Make signal/slot connections and whatever is required.
   * // Filter is of implementation specific filter expression type
   * void registerChildController(const Filter & conditions);
   *
   * // Disconnect signal/slots and do whatever is required.
   * void unregisterChildController();
   *
   * // This method is called when parent controller's model changed.
   * void setParentControllerModelToChildController();
   * \endcode
   * \todo Check if setParentControllerModelToChildController() is not obelete
   *
   * ControllerRelation does not own any controller,
   *  deleting them is the responsability of the user of this class.
   *
   * \tparam Controller Must be AbstractController or subclass
   * \tparam Derived Class that inherits from this and that implements specific methods
   */
  template<typename Controller, typename Derived>
  class ControllerRelation
  {
   public:

    /*! \brief Construct a relation
     *
     * \pre \a parentController must be a valid pointer
     */
    explicit ControllerRelation(Controller *parentController)
     : mParentController(parentController)
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
     * \pre \a conditions must be a valid relation filter expression
     */
    template<typename T>
    void setChildController(Controller *controller, const T & conditions)
    {
      Q_ASSERT(controller != nullptr);

      if(!mChildController.isNull()){
        impl()->unregisterChildController();
      }
      mChildController = controller;
      impl()->registerChildController(conditions);
      impl()->setParentControllerModelToChildController();
    }

   private:

     Derived *impl()
     {
       return static_cast<Derived*>(this);
     }

    QPointer<Controller> mParentController;
    QPointer<Controller> mChildController;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_CONTROLLER_RELATION_H
