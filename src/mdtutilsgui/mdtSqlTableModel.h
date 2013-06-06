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
#ifndef MDT_SQL_TABLE_MODEL_H
#define MDT_SQL_TABLE_MODEL_H

#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QObject>

/// \todo Check if not obselete ?

/*! \brief Sql table model
 */
class mdtSqlTableModel : public QSqlTableModel
{
 Q_OBJECT

 public:

  mdtSqlTableModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase() );

};

#endif  // #ifndef MDT_SQL_TABLE_MODEL_H
