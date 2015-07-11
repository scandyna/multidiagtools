/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#ifndef MDT_SQL_FOREIGN_KEY_SETTING
#define MDT_SQL_FOREIGN_KEY_SETTING

#include "mdtError.h"
#include <QSqlDatabase>

/*! \brief Foreign key setting
 *
 * Usefull to switch foreign key constraints ON or OFF
 */
class mdtSqlForeignKeySetting
{
 public:

  /*! \brief Foreign key setting type
   */
  enum Type_t
  {
    Permanent,  /*!< Setting is keeped when this object dies */
    Temporary   /*!< Setting is reverted when this object dies */
  };

  /*! \brief Constructor
   *
   * \param db Database instance on witch to act
   * \param type Type of foreign key setting
   * \pre db must be open
   */
  mdtSqlForeignKeySetting(QSqlDatabase db, Type_t type);

  /*! \brief Destructor
   *
   * If setting was switched (enabled or disabled) and not switched back,
   *  and keep parameter was false in constructor,
   *  setting will be switched back.
   */
  ~mdtSqlForeignKeySetting();

  /*! \brief Enable foreign key constraint setting
   */
  bool enable();

  /*! \brief Disable foreign key constraint setting
   */
  bool disable();

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 private:

  /*! \brief Setting state
   */
  enum State_t
  {
    Original,   /*!< Setting was never changed, or is back to original state */
    Enabled,    /*!< Foreign key constraint setting was enabled */
    Disabled,   /*!< Foreign key constraint setting was disabled */
    Keep        /*!< Setting must not be reversed in destructor */
  };

  /*! \brief Enable - Sqlite version
   */
  bool enableSqlite();

  /*! \brief Disable - Sqlite version
   */
  bool disableSqlite();

  Q_DISABLE_COPY(mdtSqlForeignKeySetting)

  State_t pvState;
  QSqlDatabase pvDatabase;
  mdtError pvLastError;
};

#endif // #ifndef MDT_SQL_FOREIGN_KEY_SETTING
