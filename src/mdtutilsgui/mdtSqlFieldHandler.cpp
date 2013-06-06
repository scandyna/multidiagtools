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
#include "mdtSqlFieldHandler.h"
#include "mdtError.h"
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDateTimeEdit>
#include <QAbstractButton>
#include <QComboBox>
#include <QString>
#include <QColor>

#include <QDebug>

/// \todo Implement other data editors (now only QLineEdit is implemented)

mdtSqlFieldHandler::mdtSqlFieldHandler(QObject *parent)
 : QObject(parent)
{
  clear();
}

mdtSqlFieldHandler::~mdtSqlFieldHandler()
{
}

void mdtSqlFieldHandler::setField(const QSqlField &field)
{
  qDebug() << "mdtSqlFieldHandler::setField(): " << field;
  qDebug() << "-> requiredStatus(): " << field.requiredStatus();
  pvSqlField = field;
  setDataWidgetAttributes();
}

void mdtSqlFieldHandler::setReadOnly(bool readOnly)
{
  pvIsReadOnly = readOnly;
  setDataWidgetAttributes();
}

bool mdtSqlFieldHandler::isReadOnly() const
{
  return pvIsReadOnly;
}

void mdtSqlFieldHandler::setDataWidget(QWidget *widget)
{
  Q_ASSERT(widget != 0);

  QLineEdit *le;

  // Search what type of widget we have
  le = dynamic_cast<QLineEdit*>(widget);
  if(le != 0){
    setDataWidget(le);
  }else{
    mdtError e(MDT_DATABASE_ERROR, "Cannot find type of widget, object name: " + widget->objectName(), mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtSqlFieldHandler");
    e.commit();
  }
}

void mdtSqlFieldHandler::setDataWidget(QLineEdit *widget)
{
  Q_ASSERT(widget != 0);

  clear();
  pvLineEdit = widget;
  setDataWidgetAttributes();
  connect(widget, SIGNAL(textChanged(const QString&)), this, SLOT(onDataEdited(const QString&)));
}

void mdtSqlFieldHandler::setDataWidget(QAbstractButton *widget)
{
  Q_ASSERT(widget != 0);
}

void mdtSqlFieldHandler::setDataWidget(QDateTimeEdit *widget)
{
  Q_ASSERT(widget != 0);
}

void mdtSqlFieldHandler::setDataWidget(QDoubleSpinBox *widget)
{
  Q_ASSERT(widget != 0);
}

void mdtSqlFieldHandler::setDataWidget(QSpinBox *widget)
{
  Q_ASSERT(widget != 0);
}

void mdtSqlFieldHandler::clear()
{
  pvLineEdit = 0;
  pvAbstractButton = 0;
  pvDateTimeEdit = 0;
  pvDoubleSpinBox = 0;
  pvSpinBox = 0;
  pvComboBox = 0;
  // Flags
  pvIsNull = true;
  pvIsReadOnly = false;
  pvDataEdited = false;
  pvUpdatingDataFromModel = false;
}

void mdtSqlFieldHandler::clearWidgetData()
{
  if(pvLineEdit != 0){
    pvLineEdit->clear();
  }else if(pvAbstractButton != 0){
    pvAbstractButton->setChecked(false);
  }else if(pvDateTimeEdit != 0){
    pvDateTimeEdit->clear();
  }else if(pvDoubleSpinBox != 0){
    pvDoubleSpinBox->clear();
  }else if(pvSpinBox != 0){
    pvSpinBox->clear();
  }else if(pvComboBox != 0){
    pvComboBox->clear();
  }
}

bool mdtSqlFieldHandler::isNull() const
{
  return pvIsNull;
}

bool mdtSqlFieldHandler::dataWasEdited() const
{
  return pvDataEdited;
}

bool mdtSqlFieldHandler::checkBeforeSubmit()
{
  // If Null flag is set, we clear widget
  if(pvIsNull){
    clearDataWidget();
  }
  // Check the requiered state
  if((pvSqlField.requiredStatus() == QSqlField::Required)&&(pvIsNull)){
    setDataWidgetNotOk(tr("This field is requiered"));
    return false;
  }

  return true;
}

void mdtSqlFieldHandler::onCurrentIndexChanged(int index)
{
  pvUpdatingDataFromModel = true;
}

void mdtSqlFieldHandler::onDataEdited(const QString &text)
{
  // Set the null flag
  if(text.trimmed().isEmpty()){
    pvIsNull = true;
  }else{
    pvIsNull = false;
  }
  // Set the data edited flag
  ///if(!pvIsNull){
  if(pvUpdatingDataFromModel){
    pvDataEdited = false;
    pvUpdatingDataFromModel = false;
  }else{
    if(!pvDataEdited){
      pvDataEdited = true;
      emit dataEdited();
    }
  }
  setDataWidgetOk();
  ///}
}

void mdtSqlFieldHandler::onDataEdited(bool state)
{
}

void mdtSqlFieldHandler::onDataEdited(const QDateTime & datetime)
{
}

void mdtSqlFieldHandler::clearDataWidget()
{
  setDataWidgetOk();
  if(pvLineEdit != 0){
    pvLineEdit->setText("");
    return;
  }
}

void mdtSqlFieldHandler::setDataWidgetAttributes()
{
  qDebug() << "mdtSqlFieldHandler::setDataWidgetAttributes() field: " << pvSqlField;
  qDebug() << "-> RequiredStatus: " << pvSqlField.requiredStatus();
  // Setup line edit (if not null)
  if(pvLineEdit != 0){
    // Max length
    if(pvSqlField.length() > -1){
      pvLineEdit->setMaxLength(pvSqlField.length());
    }
    // Read only
    if(pvIsReadOnly){
      pvLineEdit->setReadOnly(true);
    }else{
      pvLineEdit->setReadOnly(pvSqlField.isReadOnly());
    }
    // Original palette
    pvDataWidgetOriginalPalette = pvLineEdit->palette();
    return;
  }
}

void mdtSqlFieldHandler::setDataWidgetNotOk(const QString &toolTip)
{
  QPalette palette;

  palette.setColor(QPalette::Base, QColor(255, 190, 180));

  // Change line edit (if not null)
  if(pvLineEdit != 0){
    pvLineEdit->setPalette(palette);
    pvLineEdit->setToolTip(toolTip);
    return;
  }

}

void mdtSqlFieldHandler::setDataWidgetOk()
{
  // Restore line edit (if not null)
  if(pvLineEdit != 0){
    pvLineEdit->setPalette(pvDataWidgetOriginalPalette);
    pvLineEdit->setToolTip("");
    return;
  }
}
