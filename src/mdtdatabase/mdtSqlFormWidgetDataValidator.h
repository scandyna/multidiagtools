/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#ifndef MDT_SQL_FORM_WIDGET_DATA_VALIDATOR_H
#define MDT_SQL_FORM_WIDGET_DATA_VALIDATOR_H

#include "mdtSqlDataValidator.h"
#include <QList>

/*! \brief Data validator for mdtSqlFormWidget
 *
 * This class is used by mdtSqlFormWidget .
 */
class mdtSqlFieldHandler;

class mdtSqlFormWidgetDataValidator : public mdtSqlDataValidator
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlFormWidgetDataValidator(QObject *parent, const QList<mdtSqlFieldHandler*> &fieldHandlers);

  /*! \brief Destructor
   */
  ~mdtSqlFormWidgetDataValidator();

  /*! \brief Checks to be done before submit
   *
   * Will call checkBeforeSubmit() for every field Handlers.
   */
  bool checkBeforeSubmit();

 private:

  Q_DISABLE_COPY(mdtSqlFormWidgetDataValidator);

  QList<mdtSqlFieldHandler*> pvFieldHandlers;
};

#endif  // #ifndef MDT_SQL_FORM_WIDGET_DATA_VALIDATOR_H
