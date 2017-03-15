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
#ifndef MDT_ITEM_EDITOR_NAVIGATION_CONTROLLER_RELATION_H
#define MDT_ITEM_EDITOR_NAVIGATION_CONTROLLER_RELATION_H

#include "AbstractControllerRelation.h"

namespace Mdt{ namespace ItemEditor{

  /*! \brief Relation between a parent controller and a child controller
   *
   * NavigationControllerRelation does not own any controller,
   *  deleting them is the responsability of the user of this class.
   */
  class NavigationControllerRelation : public AbstractControllerRelation
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit NavigationControllerRelation(QObject* parent = nullptr);

   private:

    void childControllerChangedEvent(AbstractController* controller) override;

    QMetaObject::Connection mCurrentRowChangeConnection;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_NAVIGATION_CONTROLLER_RELATION_H
