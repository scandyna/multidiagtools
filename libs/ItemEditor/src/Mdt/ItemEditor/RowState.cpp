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
#include "RowState.h"
#include <QByteArray>

namespace Mdt{ namespace ItemEditor{

QByteArray rowStateText(RowState rowState)
{
  QByteArray text;

  if(rowState.isNull()){
    text.append("RowState(null)");
  }else{
    text.append("RowState(n:");
    text.append( QByteArray::number(rowState.rowCount()) );
    text.append(",currentRow:");
    text.append( QByteArray::number(rowState.currentRow()) );
    text.append(")");
  }

  return text;
}

}} // namespace Mdt{ namespace ItemEditor{