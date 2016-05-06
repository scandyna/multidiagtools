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
#include "LinkDirectionModel.h"
#include "mdt/sql/error/Error.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QModelIndex>
#include <QComboBox>

namespace Mdt{ namespace CableList{

LinkDirectionModel::LinkDirectionModel(QObject *parent, const QSqlDatabase & db)
 : QSqlQueryModel(parent),
   pvDatabase(db)
{
  setLocale(QLocale::system());
}

LinkDirectionModel::LinkDirectionModel(const QSqlDatabase & db)
 : LinkDirectionModel(nullptr, db)
{
}

void LinkDirectionModel::setLocale(const QLocale &locale)
{
  switch(locale.language()){
    case QLocale::French:
      pvNameFieldName = "NameFR";
      break;
    case QLocale::German:
      pvNameFieldName = "NameDE";
      break;
    case QLocale::Italian:
      pvNameFieldName = "NameIT";
      break;
    default:
      pvNameFieldName = "NameEN";
  }
  updateQuery();
}

int LinkDirectionModel::row(LinkDirectionType d)
{
  return row(LinkDirectionPk(d));
}

int LinkDirectionModel::row(LinkDirectionPk pk)
{
  if(isInError()){
    return -1;
  }
  const int n = rowCount();
  for(int row = 0; row < n; ++row){
    QModelIndex idx = index(row, 0);
    if(data(idx) == pk.code()){
      return row;
    }
  }
  // Update error only if a error occured
  isInError();

  return -1;
}

LinkDirectionPk LinkDirectionModel::directionPk(int row)
{
  LinkDirectionPk pk;

  if(row < 0){
    return pk;
  }
  if(isInError()){
    return pk;
  }
  QModelIndex idx = index(row, 0);
  pk = LinkDirectionPk::fromQVariant( data(idx) );
  if(pk.isNull()){
    QString msg = QString(tr("Could not find link direction for row '%1'.")).arg(row);
    pvLastError = mdtErrorNewQ(msg, mdtError::Error, this);
    pvLastError.commit();
  }

  return pk;
}

LinkDirectionPk LinkDirectionModel::currentDirectionPk(QComboBox *cb)
{
  Q_ASSERT(cb != nullptr);

  return directionPk(cb->currentIndex());
}

QString LinkDirectionModel::pictureAscii(int row)
{
  QString pa;

  if(row < 0){
    return pa;
  }
  if(isInError()){
    pa = tr("<Error>");
    return pa;
  }
  QModelIndex idx = index(row, 2);
  pa = data(idx).toString();
  if(pa.isEmpty()){
    QString msg = QString(tr("Could not find link direction for row '%1'.")).arg(row);
    pvLastError = mdtErrorNewQ(msg, mdtError::Error, this);
    pvLastError.commit();
  }

  return pa;
}

void LinkDirectionModel::setLinkType(LinkType lt)
{
  QString sql = pvBaseSql;

  switch(lt){
    case LinkType::CableLink:
    case LinkType::Connection:
    case LinkType::InternalLink:
    case LinkType::TestLink:
      sql += " WHERE Code_PK = 'BID'";
      break;
//     case mdtClLinkType_t::Diode:
//       sql += " WHERE Code_PK = 'STE' OR Code_PK = 'ETS'";
//       break;
    case LinkType::Undefined:
      break;
  }
  setQuery(sql);
}

void LinkDirectionModel::updateQuery()
{
  Q_ASSERT(!pvNameFieldName.isEmpty());

  pvBaseSql = "SELECT Code_PK, " + pvNameFieldName + ", PictureAscii FROM LinkDirection_tbl";
  setQuery(pvBaseSql, pvDatabase);
}

bool LinkDirectionModel::isInError()
{
  QSqlError sqlError = QSqlQueryModel::lastError();

  if(sqlError.isValid()){
    pvLastError = mdtErrorNewQ(tr("A error occured on table 'LinkDirection_tbl'."), mdtError::Error, this);
    pvLastError.stackError(ErrorFromQSqlQuery(query()));
    pvLastError.commit();
    return true;
  }

  return false;
}

}} // namespace Mdt{ namespace CableList{
