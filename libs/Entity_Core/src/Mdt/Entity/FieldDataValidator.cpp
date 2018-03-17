/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "FieldDataValidator.h"
#include <QCoreApplication>

namespace Mdt{ namespace Entity{

  namespace ImplNs{ namespace FieldDataValidator{

    Mdt::Error toGenericError(FieldDataValidatorState state, const QString & fieldName)
    {
      if(state == FieldDataValidatorState::Ok){
        return Mdt::Error();
      }
      QString msg;
      switch(state){
        case FieldDataValidatorState::RequiredButNull:
          {
            msg = QCoreApplication::translate("FieldDataValidator::toGenericError()", "field '%1' is required, but no data provided").arg(fieldName);
          }
          break;
        case FieldDataValidatorState::MaxLengthExeeded:
        {
          msg = QCoreApplication::translate("FieldDataValidator::toGenericError()", "maximum allowed length has been exeeded for field '%1'").arg(fieldName);
        }
        case FieldDataValidatorState::Ok:
          break;
      }
      return mdtErrorNewT(FieldDataValidatorState, state, msg, Mdt::Error::Warning, "FieldDataValidator");
    }

  }} // namespace Impl{ namespace FieldDataValidator{

}} // namespace Mdt{ namespace Entity{
