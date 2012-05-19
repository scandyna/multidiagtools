/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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
#ifndef MDT_UIC_NUMBER_VALIDATOR_H
#define MDT_UIC_NUMBER_VALIDATOR_H

#include <QValidator>

/*! \brief Validator for UIC Number
 */
class mdtUicNumberValidator : public QValidator
{

 public:

  mdtUicNumberValidator(QObject *parent);
  ~mdtUicNumberValidator();

  /*! \brief Reimplementation from QValidator
   */
  QValidator::State validate(QString &input, int &pos) const;
};

#endif  // #ifndef MDT_UIC_NUMBER_VALIDATOR_H
