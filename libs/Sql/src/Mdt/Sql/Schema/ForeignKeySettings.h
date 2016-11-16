/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_SQL_SCHEMA_FOREIGN_KEY_SETTINGS_H
#define MDT_SQL_SCHEMA_FOREIGN_KEY_SETTINGS_H

#include "ForeignKeyAction.h"

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Store settings for foreign key declaration
   */
  class ForeignKeySettings
  {
   public:

    /*! \brief Construct default settings
     *
     * \sa clear()
     */
    ForeignKeySettings()
     : mCreateIndex(false),
       mOnDeleteAction(ForeignKeyAction::NoAction),
       mOnUpdateAction(ForeignKeyAction::NoAction)
    {
    }

    /*! \brief Set if a index must be created
     *
     * If set, a index will be created on the
     *  fields that are part of the foreign key.
     *  Note: this has no impact on foreign table,
     *        only on fields of the (child) table.
     */
    void setCreateIndex(bool create)
    {
      mCreateIndex = create;
    }

    /*! \brief Check if a index must be created
     */
    bool createIndex() const
    {
      return mCreateIndex;
    }

    /*! \brief Set on delete action
     */
    void setOnDeleteAction(ForeignKeyAction action)
    {
      mOnDeleteAction = action;
    }

    /*! \brief Get on delete action
     */
    ForeignKeyAction onDeleteAction() const
    {
      return mOnDeleteAction;
    }

    /*! \brief Set on update action
     */
    void setOnUpdateAction(ForeignKeyAction action)
    {
      mOnUpdateAction = action;
    }

    /*! \brief Get on update action
     */
    ForeignKeyAction onUpdateAction() const
    {
      return mOnUpdateAction;
    }

    /*! \brief Clear
     *
     * Will restore default settings, which are:
     *  - createIndex : false
     *  - onDeleteAction : NoAction
     *  - onUpdateAction : NoAction
     */
    void clear()
    {
      mCreateIndex = false;
      mOnDeleteAction = ForeignKeyAction::NoAction;
      mOnUpdateAction = ForeignKeyAction::NoAction;
    }

   private:

    bool mCreateIndex;
    ForeignKeyAction mOnDeleteAction;
    ForeignKeyAction mOnUpdateAction;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_FOREIGN_KEY_SETTINGS_H
