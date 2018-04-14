/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "SQLiteDatabaseFileDialog.h"
#include <QStringList>
#include <QChar>
#include <QStringBuilder>
#include <QLatin1String>

namespace Mdt{ namespace Sql{

SQLiteDatabaseFileDialog::SQLiteDatabaseFileDialog(QWidget* parent)
 : SQLiteDatabaseFileDialog(parent, DbOpenExisting)
{
}

SQLiteDatabaseFileDialog::SQLiteDatabaseFileDialog(QWidget* parent, SQLiteDatabaseFileDialog::DbOpenMode openMode)
 : QFileDialog(parent)
{
  setDbOpenMode(openMode);
  setFileSuffix(QLatin1String("sqlite"), tr("SQLite files"));
}

void SQLiteDatabaseFileDialog::setDbOpenMode(SQLiteDatabaseFileDialog::DbOpenMode mode)
{
  switch(mode){
    case DbOpenExisting:
      setAcceptMode(AcceptOpen);
      setFileMode(ExistingFile);
      setWindowTitle(tr("Open a SQLite database file"));
      break;
    case DbCreateNew:
      setAcceptMode(AcceptSave);
      setFileMode(AnyFile);
      setWindowTitle(tr("Create a SQLite database file"));
      break;
  }
}

SQLiteDatabaseFileDialog::DbOpenMode SQLiteDatabaseFileDialog::dbOpenMode() const
{
  switch(acceptMode()){
    case AcceptOpen:
      return DbOpenExisting;
    case AcceptSave:
      return DbCreateNew;
  }
  return DbOpenExisting;
}

void SQLiteDatabaseFileDialog::setFileSuffix(const QString& suffix, const QString& label)
{
  Q_ASSERT(!suffix.trimmed().isEmpty());
  Q_ASSERT(!label.trimmed().isEmpty());

  if(suffix.at(0) == QChar::fromLatin1('.')){
    mFileSuffix = suffix.right( suffix.length()-1 );
  }else{
    mFileSuffix = suffix;
  }
  mFileSuffixLabel = label;
  setNameFilters({label % QLatin1String(" (*.") % mFileSuffix % QChar::fromLatin1(')')});
}

// QString SQLiteDatabaseFileDialog::fileSuffix() const
// {
//   Q_ASSERT( nameFilters().size() == 1 );
// 
//   QString suffix;
//   const auto parts = nameFilters().at(0).split(QChar::fromLatin1('('));
//   Q_ASSERT(parts.size() == 2);
//   suffix = parts.at(1).trimmed().remove(QChar::fromLatin1(')'));
//   
//   return suffix;
// }

// QString SQLiteDatabaseFileDialog::fileSuffixLabel() const
// {
// 
// }

}} // namespace Mdt{ namespace Sql{
