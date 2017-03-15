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

#include "AbstractControllerRelation.h"
#include <QPointer>
#include <vector>

namespace Mdt{ namespace ItemEditor{

  class AbstractController;

  /*! \brief Container of controller relations
   *
   * Controllers are only referenced in this list,
   *  they will never be destroyed.
   */
  class ControllerRelationList
  {
   public:

    /*! \brief STL-style const iterator
     */
    typedef typename std::vector<AbstractControllerRelation*>::const_iterator const_iterator;

    /*! \brief Construct a relation list
     *
     * \pre \a parentController must be a valid pointer
     */
    explicit ControllerRelationList(AbstractController *parentController);

    /*! \brief Clear created relations
     */
    ~ControllerRelationList();

    // Disable copy
    ControllerRelationList(const ControllerRelationList &) = delete;
    ControllerRelationList & operator=(const ControllerRelationList &) = delete;
    // Disable move
    ControllerRelationList(ControllerRelationList &&) = delete;
    ControllerRelationList & operator=(ControllerRelationList &&) = delete;

    /*! \brief Add a child controller
     *
     * This method will set parent parent controller
     *  (the one passed in the contructor)
     *  and \a controller as child controller the created relation.
     *  The crreated relation is returned to finish setup.
     *
     * \pre \a controller must be a valid pointer
     */
    template<typename RelationType>
    RelationType *addChildController(AbstractController *controller)
    {
      Q_ASSERT(controller != nullptr);
      auto relation = new RelationType;
      relation->setParentController(mParentController);
      relation->setChildController(controller);
      mList.push_back(relation);
      return relation;
    }

    /*! \brief Submit data to model for all child controller
     */
    bool submitForEachChild();

    /*! \brief Revert data from model for each child controller
     */
    void revertForEachChild();

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
    void clearRelations();

   private:

    QPointer<AbstractController> mParentController;
    std::vector<AbstractControllerRelation*> mList;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_CONTROLLER_LIST_H
