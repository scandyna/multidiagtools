/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtClArticleLinkDialog.h"
#include "mdtSqlSelectionDialog.h"
#include <QSqlQueryModel>

mdtClArticleLinkDialog::mdtClArticleLinkDialog(QWidget *parent, QSqlDatabase db)
 : QDialog(parent)
{
  pvDatabase = db;;
  pvLinkTypeModel = new QSqlQueryModel(this);
  pvLinkDirectionModel = new QSqlQueryModel(this);
  pvArticleConnectionModel = new QSqlQueryModel(this);
  setupUi(this);
}

mdtClArticleLinkDialog::~mdtClArticleLinkDialog()
{
}

void mdtClArticleLinkDialog::setLinkTypeCode(const QVariant & code)
{
  pvLinkTypeCode = code;
}

QVariant mdtClArticleLinkDialog::linkTypeCode() const
{
  return pvLinkTypeCode;
}

void mdtClArticleLinkDialog::setLinkDirectionCode(const QVariant & code)
{
  pvLinkDirectionCode = code;
}

QVariant mdtClArticleLinkDialog::linkDirectionCode() const
{
  return pvLinkDirectionCode;
}

void mdtClArticleLinkDialog::setValue(const QVariant & value)
{
  pvValue = value.toDouble();
}

QVariant mdtClArticleLinkDialog::value() const
{
  return pvValue;
}

void mdtClArticleLinkDialog::setStartConnectionId(const QVariant & id)
{
  pvStartConnectionId = id;
}

QVariant mdtClArticleLinkDialog::startConnectionId() const
{
  return pvStartConnectionId;
}

void mdtClArticleLinkDialog::setEndConnectionId(const QVariant & id)
{
  pvEndConnectionId = id;
}

QVariant mdtClArticleLinkDialog::endConnectionId() const
{
  return pvEndConnectionId;
}

void mdtClArticleLinkDialog::onCbLinkTypeCurrentIndexChanged(int index)
{
}

void mdtClArticleLinkDialog::onCbLinkDirectionCurrentIndexChanged(int index)
{
}

void mdtClArticleLinkDialog::selectStartConnection()
{
}

void mdtClArticleLinkDialog::selectEndConnection()
{
}
