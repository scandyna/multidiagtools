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
#ifndef MDT_SQL_DATA_VALIDATOR_H
#define MDT_SQL_DATA_VALIDATOR_H

#include "mdtUiMessageHandler.h"
#include <QObject>
#include <QSqlError>
#include <QString>
#include <memory>

/*! \brief Helper class for data validation
 *
 * The SQL widget controllers witch are usefull to create database GUI
 *  are event driven.
 *  A internal state machine tries to keep GUI coherent.
 *  When, f.ex., the user wants to save data, a trigger is activated
 *  and state changes (if current state permit it) to a sort of saving state.
 *  The drawback of this solution is that there is no way to handle the save steps.
 *  This class was made to help on such process.
 *
 * For example, if you need to do some check before the real submit beginns,
 *  you can subclass mdtSqlDataValidator and reimplement checkBeforeSubmit().
 *  If this method fails, the submit is not done.
 *
 * \sa mdtAbstractSqlTableController
 */
class mdtSqlDataValidator : public QObject
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlDataValidator(QObject *parent = 0);

  /*! \brief Destructor
   */
  virtual ~mdtSqlDataValidator();

  /*! \brief Set message handler
   */
  void setMessageHandler(std::shared_ptr<mdtUiMessageHandler> handler);

  /*! \brief Checks to be done before data are submitted
   *
   * \return This default implementation does nothing and returns allways false.
   */
  virtual bool checkBeforeSubmit();

 protected:

  /*! \brief Get message handler
   *
   * Can return a Null pointer if no message handler was set (witch is acceptable case).
   */
  std::shared_ptr<mdtUiMessageHandler> messageHandler() { return pvMessageHandler; }

 private:

  Q_DISABLE_COPY(mdtSqlDataValidator);

  std::shared_ptr<mdtUiMessageHandler> pvMessageHandler;
};

#endif  // #ifndef MDT_SQL_DATA_VALIDATOR_H
