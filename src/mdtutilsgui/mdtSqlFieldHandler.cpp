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
#include "mdtDoubleEdit.h"
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

/*
 * mdtSqlFieldHandler has to deal with several widgets type
 * (QLineEdit, QSpinBox, ...)
 * To avoid storing several pointer, and each time searchin
 *  wich one was set, we create a abstract class
 *  with common used methods, and specialized subclass for each widget type
 */

class mdtSqlFieldHandlerAbstractDataWidget
{
 public:
  virtual ~mdtSqlFieldHandlerAbstractDataWidget() {}
  virtual void setData(const QVariant &data) = 0;
  virtual QVariant data() const = 0;
  virtual bool isNull() const = 0;
  virtual void clear() = 0;
  virtual QWidget *widget() = 0;
  virtual void setReadOnly(bool readOnly) = 0;
  virtual void setMaxLength(int maxLength) {};
  virtual void setRange(qreal min, qreal max) {};
};

/*
 * QLineEdit specialisation
 */
class mdtSqlFieldHandlerLineEdit : public mdtSqlFieldHandlerAbstractDataWidget
{
 public:
  mdtSqlFieldHandlerLineEdit();
  void setLineEdit(QLineEdit *le);
  void setData(const QVariant &data);
  QVariant data() const;
  bool isNull() const;
  void clear();
  QWidget *widget();
  void setReadOnly(bool readOnly);
  void setMaxLength(int maxLength);
 private:
  QLineEdit *pvLineEdit;
};

mdtSqlFieldHandlerLineEdit::mdtSqlFieldHandlerLineEdit()
{
  pvLineEdit = 0;
}

void mdtSqlFieldHandlerLineEdit::setLineEdit(QLineEdit *le)
{
  Q_ASSERT(le != 0);

  pvLineEdit = le;
}

void mdtSqlFieldHandlerLineEdit::setData(const QVariant &data)
{
  Q_ASSERT(pvLineEdit != 0);

  pvLineEdit->setText(data.toString());
}

QVariant mdtSqlFieldHandlerLineEdit::data() const
{
  Q_ASSERT(pvLineEdit != 0);

  if(pvLineEdit->text().trimmed().isEmpty()){
    return QVariant();
  }
  return pvLineEdit->text();
}

bool mdtSqlFieldHandlerLineEdit::isNull() const
{
  return data().isNull();
}

void mdtSqlFieldHandlerLineEdit::clear()
{
  Q_ASSERT(pvLineEdit != 0);

  pvLineEdit->clear();
}

QWidget *mdtSqlFieldHandlerLineEdit::widget()
{
  return pvLineEdit;
}

void mdtSqlFieldHandlerLineEdit::setReadOnly(bool readOnly)
{
  Q_ASSERT(pvLineEdit != 0);

  pvLineEdit->setReadOnly(readOnly);
}

void mdtSqlFieldHandlerLineEdit::setMaxLength(int maxLength)
{
  Q_ASSERT(pvLineEdit != 0);

  pvLineEdit->setMaxLength(maxLength);
}

/*
 * mdtDoubleEdit specialisation
 */
class mdtSqlFieldHandlerMdtDoubleEdit : public mdtSqlFieldHandlerAbstractDataWidget
{
 public:
  mdtSqlFieldHandlerMdtDoubleEdit();
  void setDoubleEdit(mdtDoubleEdit *e);
  void setData(const QVariant &data);
  QVariant data() const;
  bool isNull() const;
  void clear();
  QWidget *widget();
  void setReadOnly(bool readOnly);
  void setMaxLength(int maxLength);
 private:
  mdtDoubleEdit *pvDoubleEdit;
};

mdtSqlFieldHandlerMdtDoubleEdit::mdtSqlFieldHandlerMdtDoubleEdit()
{
  pvDoubleEdit = 0;
}

void mdtSqlFieldHandlerMdtDoubleEdit::setDoubleEdit(mdtDoubleEdit* e)
{
  Q_ASSERT(e != 0);
  pvDoubleEdit = e;
}

void mdtSqlFieldHandlerMdtDoubleEdit::setData(const QVariant& data)
{
  Q_ASSERT(pvDoubleEdit != 0);
  pvDoubleEdit->setValue(data, false);
}

QVariant mdtSqlFieldHandlerMdtDoubleEdit::data() const
{
  Q_ASSERT(pvDoubleEdit != 0);
  return pvDoubleEdit->value();
}

bool mdtSqlFieldHandlerMdtDoubleEdit::isNull() const
{
  return data().isNull();
}

void mdtSqlFieldHandlerMdtDoubleEdit::clear()
{
  Q_ASSERT(pvDoubleEdit != 0);
  pvDoubleEdit->setValue(QVariant());
}

QWidget* mdtSqlFieldHandlerMdtDoubleEdit::widget()
{
  return pvDoubleEdit;
}

void mdtSqlFieldHandlerMdtDoubleEdit::setReadOnly(bool readOnly)
{
  Q_ASSERT(pvDoubleEdit != 0);
  pvDoubleEdit->setReadOnly(readOnly);
}

void mdtSqlFieldHandlerMdtDoubleEdit::setMaxLength(int maxLength)
{
}


/*
 * ==== mdtSqlFieldHandler implementation  ====
 */

mdtSqlFieldHandler::mdtSqlFieldHandler(QWidget *parent)
 : QWidget(parent)
{
  pvDataWidget = 0;
  clear();
}


mdtSqlFieldHandler::~mdtSqlFieldHandler()
{
  delete pvDataWidget;
}

void mdtSqlFieldHandler::setField(const QSqlField &field)
{
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
  mdtDoubleEdit *de;

  // Search what type of widget we have
  le = dynamic_cast<QLineEdit*>(widget);
  if(le != 0){
    setDataWidget(le);
    return;
  }
  de = dynamic_cast<mdtDoubleEdit*>(widget);
  if(de != 0){
    setDataWidget(de);
    return;
  }
  // Not supported widget type
  mdtError e("Cannot find type of widget, object name: " + widget->objectName(), mdtError::Warning);
  MDT_ERROR_SET_SRC(e, "mdtSqlFieldHandler");
  e.commit();
}

void mdtSqlFieldHandler::setDataWidget(QLineEdit *widget)
{
  Q_ASSERT(widget != 0);

  mdtSqlFieldHandlerLineEdit *w;
  clear();
  w = new mdtSqlFieldHandlerLineEdit;
  w->setLineEdit(widget);
  pvDataWidget = w;
  setDataWidgetAttributes();
  connect(widget, SIGNAL(textEdited(const QString&)), this, SLOT(onDataEdited(const QString&)));
}

void mdtSqlFieldHandler::setDataWidget(mdtDoubleEdit* widget)
{
  Q_ASSERT(widget != 0);

  mdtSqlFieldHandlerMdtDoubleEdit *w;
  clear();
  w = new mdtSqlFieldHandlerMdtDoubleEdit;
  w->setDoubleEdit(widget);
  pvDataWidget = w;
  setDataWidgetAttributes();
  connect(widget, SIGNAL(valueChanged(double,bool)), this, SLOT(onDataEdited(double,bool)));
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

QWidget *mdtSqlFieldHandler::dataWidget()
{
  if(pvDataWidget == 0){
    return 0;
  }
  return pvDataWidget->widget();

  /// \todo Below becomes obselète
  /**
  if(pvAbstractButton != 0){
    return pvAbstractButton;
  }
  if(pvDateTimeEdit != 0){
    return pvDateTimeEdit;
  }
  if(pvDoubleSpinBox != 0){
    return pvDoubleSpinBox;
  }
  if(pvSpinBox != 0){
    return pvSpinBox;
  }
  if(pvComboBox != 0){
    return pvComboBox;
  }
  return 0;
  */
}

void mdtSqlFieldHandler::clear()
{
  delete pvDataWidget;
  pvDataWidget = 0;
  /**
  pvAbstractButton = 0;
  pvDateTimeEdit = 0;
  pvDoubleSpinBox = 0;
  pvSpinBox = 0;
  pvComboBox = 0;
  */
  // Flags
  pvIsReadOnly = false;
  pvDataEdited = false;
}

void mdtSqlFieldHandler::clearWidgetData()
{
  Q_ASSERT(pvDataWidget != 0);

  pvDataWidget->clear();

  return;
  /// \todo Below becomes obselète
  /**
  if(pvAbstractButton != 0){
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
  */
}

bool mdtSqlFieldHandler::isNull() const
{
  if(pvDataWidget == 0){
    return true;
  }
  return pvDataWidget->isNull();
}

bool mdtSqlFieldHandler::dataWasEdited() const
{
  return pvDataEdited;
}

bool mdtSqlFieldHandler::checkBeforeSubmit()
{
  // If Null flag is set, we clear widget
  if(isNull()){
    clearDataWidget();
  }
  // Check the requiered state
  if((pvSqlField.requiredStatus() == QSqlField::Required)&&(isNull())){
    setDataWidgetNotOk(tr("This field is requiered"));
    return false;
  }

  return true;
}

void mdtSqlFieldHandler::setData(const QVariant &data)
{
  Q_ASSERT(pvDataWidget != 0);

  pvDataWidget->setData(data);
}

QVariant mdtSqlFieldHandler::data() const
{
  Q_ASSERT(pvDataWidget != 0);

  QVariant data = pvDataWidget->data();
  if((data.isValid())&&(!data.isNull())){
    return data;
  }
  // If data is null, we must return a empty string for fields that are strings,
  //  and that accept null values, else mapper will revert data
  switch(pvSqlField.type()){
    case QVariant::String:
      if(pvSqlField.requiredStatus() != QSqlField::Required){
        return "";
      }
      break;
    default:
      return data;
  }

  return data;
}

void mdtSqlFieldHandler::updateFlags()
{
  // We reset data edited flag, so that it is re-emitted once user changes something.
  pvDataEdited = false;
  setDataWidgetOk();
}

void mdtSqlFieldHandler::onDataEdited(const QString &text)
{
  // Set the data edited flag
  if(!pvDataEdited){
    pvDataEdited = true;
    emit dataEdited();
  }
  setDataWidgetOk();
}

void mdtSqlFieldHandler::onDataEdited(double x, bool isValid)
{
  // Set the data edited flag
  qDebug() << "mdtSqlFieldHandler::onDataEdited() - x: " << x << ", valid: " << isValid << ", pvDataEdited: " << pvDataEdited;
  if(!pvDataEdited){
    pvDataEdited = true;
    qDebug() << "mdtSqlFieldHandler::onDataEdited() - emit dataEdited() ...";
    emit dataEdited();
  }
  setDataWidgetOk();
}

void mdtSqlFieldHandler::onDataEdited(bool state)
{
}

void mdtSqlFieldHandler::onDataEdited(const QDateTime & datetime)
{
}

void mdtSqlFieldHandler::clearDataWidget()
{
  Q_ASSERT(pvDataWidget != 0);

  setDataWidgetOk();
  pvDataWidget->clear();
}

void mdtSqlFieldHandler::setDataWidgetAttributes()
{
  if(pvDataWidget == 0){
    return;
  }
  Q_ASSERT(pvDataWidget->widget() != 0);

  // Max length
  if(pvSqlField.length() > -1){
    pvDataWidget->setMaxLength(pvSqlField.length());
  }
  // Read only
  if(pvIsReadOnly){
    pvDataWidget->setReadOnly(true);
  }else{
    pvDataWidget->setReadOnly(pvSqlField.isReadOnly());
  }
  // Original palette
  pvDataWidgetOriginalPalette = pvDataWidget->widget()->palette();
}

void mdtSqlFieldHandler::setDataWidgetNotOk(const QString &toolTip)
{
  Q_ASSERT(pvDataWidget != 0);
  Q_ASSERT(pvDataWidget->widget() != 0);

  QPalette palette;

  palette.setColor(QPalette::Base, QColor(255, 190, 180));

  pvDataWidget->widget()->setPalette(palette);
  pvDataWidget->widget()->setToolTip(toolTip);
}

void mdtSqlFieldHandler::setDataWidgetOk()
{
  Q_ASSERT(pvDataWidget != 0);
  Q_ASSERT(pvDataWidget->widget() != 0);

  pvDataWidget->widget()->setPalette(pvDataWidgetOriginalPalette);
  pvDataWidget->widget()->setToolTip("");
}
