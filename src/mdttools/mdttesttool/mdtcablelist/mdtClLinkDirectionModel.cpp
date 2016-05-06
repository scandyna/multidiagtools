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
#include "mdtClLinkDirectionModel.h"
#include <QSqlError>
#include <QModelIndex>
#include <QComboBox>

//#include <QDebug>

mdtClLinkDirectionModel::mdtClLinkDirectionModel(QObject *parent, QSqlDatabase db, const QLocale &locale)
 : QSqlQueryModel(parent)
{
  QString fieldName;

  // Select name field regarding language
  switch(locale.language()){
    case QLocale::French:
      fieldName = "NameFR";
      break;
    case QLocale::German:
      fieldName = "NameDE";
      break;
    case QLocale::Italian:
      fieldName = "NameIT";
      break;
    default:
      fieldName = "NameEN";
  }
  // Get text
  pvBaseSql = "SELECT Code_PK, " + fieldName + ", PictureAscii FROM LinkDirection_tbl";
  setQuery(pvBaseSql, db);
}

mdtClLinkDirectionModel::mdtClLinkDirectionModel(QSqlDatabase db, const QLocale &locale)
 : mdtClLinkDirectionModel(nullptr, db, locale)
{
}

int mdtClLinkDirectionModel::row(mdtClLinkDirection_t d)
{
  mdtClLinkDirectionKeyData key;

  key.setDirection(d);

  return row(key);
}

int mdtClLinkDirectionModel::row(const mdtClLinkDirectionKeyData &key)
{
  int row;

  if(isInError()){
    return -1;
  }
  for(row = 0; row < rowCount(); ++row){
    QModelIndex idx = index(row, 0);
    if(data(idx) == key.code){
      return row;
    }
  }
  // Update error only if a error occured
  isInError();

  return -1;
}

mdtClLinkDirectionKeyData mdtClLinkDirectionModel::keyData(int row)
{
  mdtClLinkDirectionKeyData key;

  if(row < 0){
    return key;
  }
  if(isInError()){
    return key;
  }
  QModelIndex idx = index(row, 0);
  key.code = data(idx);
  if(key.isNull()){
    QString msg = QString(tr("Could not find link direction for row '%1'.")).arg(row);
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClLinkDirectionModel");
    pvLastError.commit();
  }

  return key;
}

mdtClLinkDirectionKeyData mdtClLinkDirectionModel::currentKeyData(QComboBox *cb)
{
  Q_ASSERT(cb != nullptr);

  return keyData(cb->currentIndex());
}

QString mdtClLinkDirectionModel::pictureAscii(int row)
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
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClLinkDirectionModel");
    pvLastError.commit();
  }

  return pa;
}

void mdtClLinkDirectionModel::setLinkType(LinkType lt)
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
      sql += " WHERE Code_PK = ''";
      break;
  }
  setQuery(sql);
}


bool mdtClLinkDirectionModel::isInError()
{
  QSqlError sqlError = QSqlQueryModel::lastError();

  if(sqlError.isValid()){
    pvLastError.setError(tr("A error occured on table 'LinkDirection_tbl'."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClLinkDirectionModel");
    pvLastError.commit();
    return true;
  }

  return false;
}
