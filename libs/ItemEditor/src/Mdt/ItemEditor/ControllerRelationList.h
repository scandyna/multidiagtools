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
#ifndef MDT_ITEM_EDITOR_CONTROLLER_LIST_H
#define MDT_ITEM_EDITOR_CONTROLLER_LIST_H

// #include "ControllerRelation.h"
#include <QPointer>
#include <vector>

namespace Mdt{ namespace ItemEditor{

  /*! \brief Container of controller relations
   *
   * Controllers are only referenced in this list,
   *  they will never be destroyed.
   *
   * \tparam Controller Must be AbstractController or subclass
   * \tparam RelationImpl Specifi implementation of ControllerRelation
   */
  template<typename Controller, typename RelationImpl>
  class ControllerRelationList
  {
   public:

    /*! \brief STL-style const iterator
     */
    typedef typename std::vector<RelationImpl*>::const_iterator const_iterator;

    /*! \brief Construct a relation list
     *
     * \pre \a parentController must be a valid pointer
     */
    explicit ControllerRelationList(Controller *parentController)
     : mParentController(parentController)
    {
      Q_ASSERT(!mParentController.isNull());
      /*
       * parentController is not yet completely constructed,
       * so we only reference it in this constructor,
       * and avoid using it now.
       */
    }

    /*! \brief Clear created relations
     */
    ~ControllerRelationList()
    {
      clearRelations();
    }

    // Disable copy
    ControllerRelationList(const ControllerRelationList &) = delete;
    ControllerRelationList & operator=(const ControllerRelationList &) = delete;
    // Disable move
    ControllerRelationList(ControllerRelationList &&) = delete;
    ControllerRelationList & operator=(ControllerRelationList &&) = delete;

    /*! \brief Add a child controller
     *
     * \pre \a controller must be a valid pointer
     * \pre \a conditions must be a valid relation filter expression
     */
    template<typename T>
    void addChildController(Controller *controller, const T & conditions)
    {
      Q_ASSERT(controller != nullptr);
      auto *relation = new RelationImpl(mParentController);
      relation->setChildController(controller);
      relation->setRelationFilter(conditions);
      mList.push_back(relation);
    }

    /*! \brief Add a child controller
     *
     * \pre \a controller must be a valid pointer
     * \pre parent controller must have a non null primary key set
     * \pre \a controller must have a non null foreign key set
     * \pre Both primary of parent controller and foreign key of \a controller must have the same count of columns, and max 4
     */
    void addChildController(Controller *controller)
    {
      Q_ASSERT(controller != nullptr);
      Q_ASSERT(!mParentController->getPrimaryKey().isNull());
      Q_ASSERT(!controller->getForeignKey().isNull());
      Q_ASSERT(mParentController->getPrimaryKey().columnCount() == controller->getForeignKey().columnCount());
      auto *relation = new RelationImpl(mParentController);
      relation->setChildController(controller);
      relation->setRelationFilterFromPkFk();
      mList.push_back(relation);
    }

    /*! \brief Set parent controller's model to all child controllers
     */
    void setParentControllerModelToChildControllers()
    {
      for(auto *relation : mList){
        Q_ASSERT(relation != nullptr);
        relation->setParentControllerModelToChildController();
      }
    }

    /*! \brief Get count of child controllers
     */
    int childControllerCount() const
    {
      return mList.size();
    }

    /*! \brief Get begin const iterator
     */
    const_iterator cbegin() const
    {
      return mList.cbegin();
    }

    /*! \brief Get end const iterator
     */
    const_iterator cend() const
    {
      return mList.cend();
    }

    /*! \brief Remove all relations
     */
    void clearRelations()
    {
      for(auto relation : mList){
        delete relation;
      }
      mList.clear();
    }

   private:

    QPointer<Controller> mParentController;
    std::vector<RelationImpl*> mList;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_CONTROLLER_LIST_H
