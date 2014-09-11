/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDateTimeEdit>
#include <QAbstractButton>
#include <QComboBox>
#include <QString>
#include <QColor>
#include <QLocale>
#include <QDate>
#include <QTime>
#include <QDateTime>

#include <QDebug>

/// \todo Implement other data editors

/*
 * QLabel specialisation
 */
class mdtSqlFieldHandlerLabel : public mdtSqlFieldHandlerAbstractDataWidget
{
 public:
  mdtSqlFieldHandlerLabel();
  void setLabel(QLabel *lb);
  void setData(const QVariant &data);
  QVariant data() const;
  bool isNull() const;
  void clear();
  QWidget *widget();
  void setReadOnly(bool readOnly);
 private:
  QLabel *pvLabel;
  QVariant pvRawData;
};

mdtSqlFieldHandlerLabel::mdtSqlFieldHandlerLabel()
{
  pvLabel = 0;
}

void mdtSqlFieldHandlerLabel::setLabel(QLabel* lb)
{
  Q_ASSERT(lb != 0);
  pvLabel = lb;
}

void mdtSqlFieldHandlerLabel::setData(const QVariant& data)
{
  Q_ASSERT(pvLabel != 0);

  QLocale locale;
  QString str;

  pvRawData = data;
  switch(pvRawData.type()){
    case QVariant::DateTime:
      str = locale.toString(pvRawData.toDateTime());
      pvLabel->setText(str);
      break;
    case QVariant::Date:
      str = locale.toString(pvRawData.toDate());
      pvLabel->setText(str);
      break;
    case QVariant::Time:
      str = locale.toString(pvRawData.toTime());
      pvLabel->setText(str);
      break;
    default:
      pvLabel->setText(pvRawData.toString());
  }
}

QVariant mdtSqlFieldHandlerLabel::data() const
{
  Q_ASSERT(pvLabel != 0);
  return pvRawData;
}

bool mdtSqlFieldHandlerLabel::isNull() const
{
  Q_ASSERT(pvLabel != 0);
  return data().isNull();
}

void mdtSqlFieldHandlerLabel::clear()
{
  Q_ASSERT(pvLabel != 0);
  pvLabel->clear();
}

QWidget* mdtSqlFieldHandlerLabel::widget()
{
  Q_ASSERT(pvLabel != 0);
  return pvLabel;
}

void mdtSqlFieldHandlerLabel::setReadOnly(bool readOnly)
{
  Q_ASSERT(pvLabel != 0);
}


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
 * QPlainTextEdit specialisation
 */

mdtSqlFieldHandlerPlainTextEdit::mdtSqlFieldHandlerPlainTextEdit(QObject *parent)
 : QObject(parent)
{
  pvPlainTextEdit = 0;
  pvSetDataFromDb = true;
}

void mdtSqlFieldHandlerPlainTextEdit::setPlainTextEdit(QPlainTextEdit* pte)
{
  Q_ASSERT(pte != 0);
  pvPlainTextEdit = pte;
  pvSetDataFromDb = true;
  connect(pvPlainTextEdit, SIGNAL(textChanged()), this, SLOT(onPlainTextChanged()));
}

void mdtSqlFieldHandlerPlainTextEdit::setData(const QVariant& data)
{
  Q_ASSERT(pvPlainTextEdit != 0);
  pvSetDataFromDb = true;
  pvPlainTextEdit->setPlainText(data.toString());
}

QVariant mdtSqlFieldHandlerPlainTextEdit::data() const
{
  Q_ASSERT(pvPlainTextEdit != 0);

  QString text = pvPlainTextEdit->toPlainText();
  if(text.trimmed().isEmpty()){
    return QVariant();
  }
  return text;
}

bool mdtSqlFieldHandlerPlainTextEdit::isNull() const
{
  Q_ASSERT(pvPlainTextEdit != 0);
  return data().isNull();
}

void mdtSqlFieldHandlerPlainTextEdit::clear()
{
  Q_ASSERT(pvPlainTextEdit != 0);
  pvSetDataFromDb = true;
  pvPlainTextEdit->clear();
}

QWidget* mdtSqlFieldHandlerPlainTextEdit::widget()
{
  Q_ASSERT(pvPlainTextEdit != 0);
  return pvPlainTextEdit;
}

void mdtSqlFieldHandlerPlainTextEdit::setReadOnly(bool readOnly)
{
  Q_ASSERT(pvPlainTextEdit != 0);
  pvPlainTextEdit->setReadOnly(readOnly);
}

void mdtSqlFieldHandlerPlainTextEdit::setMaxLength(int maxLength)
{
  Q_ASSERT(pvPlainTextEdit != 0);
  qDebug() << "mdtSqlFieldHandlerPlainTextEdit::setMaxLength(): currently not implmeneted !";
}

void mdtSqlFieldHandlerPlainTextEdit::onPlainTextChanged()
{
  if(!pvSetDataFromDb){
    emit dataEdited();
  }
  pvSetDataFromDb = false;
}

/*
 * QComboBox specialisation
 */
class mdtSqlFieldHandlerComboBox : public mdtSqlFieldHandlerAbstractDataWidget
{
 public:
  mdtSqlFieldHandlerComboBox();
  void setComboBox(QComboBox *cb);
  void setData(const QVariant &data);
  QVariant data() const;
  bool isNull() const;
  void clear();
  QWidget *widget();
  void setReadOnly(bool readOnly);
  void setMaxLength(int maxLength);
 private:
  QComboBox *pvComboBox;
};

mdtSqlFieldHandlerComboBox::mdtSqlFieldHandlerComboBox()
{
  pvComboBox = 0;
}

void mdtSqlFieldHandlerComboBox::setComboBox(QComboBox* cb)
{
  Q_ASSERT(cb != 0);
  pvComboBox = cb;
}

void mdtSqlFieldHandlerComboBox::setData(const QVariant& data)
{
  Q_ASSERT(pvComboBox != 0);

  int index;

  index = pvComboBox->findData(data, Qt::DisplayRole);
  pvComboBox->setCurrentIndex(index);
  if((index < 0)&&(pvComboBox->isEditable())){
    pvComboBox->setEditText(data.toString());
  }
}

QVariant mdtSqlFieldHandlerComboBox::data() const
{
  Q_ASSERT(pvComboBox != 0);

  if(pvComboBox->currentText().trimmed().isEmpty()){
    return QVariant();
  }
  return pvComboBox->currentText();
}

bool mdtSqlFieldHandlerComboBox::isNull() const
{
  Q_ASSERT(pvComboBox != 0);
  return data().isNull();
}

void mdtSqlFieldHandlerComboBox::clear()
{
  Q_ASSERT(pvComboBox != 0);
  setData(QVariant());
}

QWidget* mdtSqlFieldHandlerComboBox::widget()
{
  return pvComboBox;
}

void mdtSqlFieldHandlerComboBox::setReadOnly(bool readOnly)
{
  Q_ASSERT(pvComboBox != 0);
  pvComboBox->setEnabled(!readOnly);
}

void mdtSqlFieldHandlerComboBox::setMaxLength(int maxLength)
{
  Q_ASSERT(pvComboBox != 0);
  if(pvComboBox->lineEdit() != 0){
    pvComboBox->lineEdit()->setMaxLength(maxLength);
  }
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
 * QDateTimeEdit specialisation
 */

class mdtSqlFieldHandlerDateTimeEdit : public mdtSqlFieldHandlerAbstractDataWidget
{
 public:
  mdtSqlFieldHandlerDateTimeEdit();
  void setDateTimeEdit(QDateTimeEdit *e);
  void setData(const QVariant &data);
  QVariant data() const;
  bool isNull() const;
  void clear();
  QWidget *widget();
  void setReadOnly(bool readOnly);
 private:
  QDateTimeEdit *pvDateTimeEdit;
};

mdtSqlFieldHandlerDateTimeEdit::mdtSqlFieldHandlerDateTimeEdit()
{
  pvDateTimeEdit = 0;
}

void mdtSqlFieldHandlerDateTimeEdit::setDateTimeEdit(QDateTimeEdit* e)
{
  Q_ASSERT(e != 0);
  pvDateTimeEdit = e;
}

void mdtSqlFieldHandlerDateTimeEdit::setData(const QVariant& data)
{
  Q_ASSERT(pvDateTimeEdit != 0);
  pvDateTimeEdit->setDateTime(data.toDateTime());
}

QVariant mdtSqlFieldHandlerDateTimeEdit::data() const
{
  Q_ASSERT(pvDateTimeEdit != 0);
  return pvDateTimeEdit->dateTime();
}

bool mdtSqlFieldHandlerDateTimeEdit::isNull() const
{
  Q_ASSERT(pvDateTimeEdit != 0);
  return data().isNull();
}

void mdtSqlFieldHandlerDateTimeEdit::clear()
{
  Q_ASSERT(pvDateTimeEdit != 0);
  pvDateTimeEdit->clear();
}

QWidget* mdtSqlFieldHandlerDateTimeEdit::widget()
{
  Q_ASSERT(pvDateTimeEdit != 0);
  return pvDateTimeEdit;
}

void mdtSqlFieldHandlerDateTimeEdit::setReadOnly(bool readOnly)
{
  Q_ASSERT(pvDateTimeEdit != 0);
  pvDateTimeEdit->setReadOnly(readOnly);
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

  QLabel *lb;
  QLineEdit *le;
  QPlainTextEdit *pte;
  QComboBox *cb;
  mdtDoubleEdit *dble;
  QDateTimeEdit *dte;

  // Search what type of widget we have
  lb = dynamic_cast<QLabel*>(widget);
  if(lb != 0){
    setDataWidget(lb);
    return;
  }
  le = dynamic_cast<QLineEdit*>(widget);
  if(le != 0){
    setDataWidget(le);
    return;
  }
  pte = dynamic_cast<QPlainTextEdit*>(widget);
  if(pte != 0){
    setDataWidget(pte);
    return;
  }
  cb = dynamic_cast<QComboBox*>(widget);
  if(cb != 0){
    setDataWidget(cb);
    return;
  }
  dble = dynamic_cast<mdtDoubleEdit*>(widget);
  if(dble != 0){
    setDataWidget(dble);
    return;
  }
  dte = dynamic_cast<QDateTimeEdit*>(widget);
  if(dte != 0){
    setDataWidget(dte);
    return;
  }
  // Not supported widget type
  mdtError e("Cannot find type of widget, object name: " + widget->objectName(), mdtError::Warning);
  MDT_ERROR_SET_SRC(e, "mdtSqlFieldHandler");
  e.commit();
}

void mdtSqlFieldHandler::setDataWidget(QLabel* widget)
{
  Q_ASSERT(widget != 0);

  mdtSqlFieldHandlerLabel *w;
  clear();
  w = new mdtSqlFieldHandlerLabel;
  w->setLabel(widget);
  pvDataWidget = w;
  setDataWidgetAttributes();
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

void mdtSqlFieldHandler::setDataWidget(QPlainTextEdit* widget)
{
  Q_ASSERT(widget != 0);

  mdtSqlFieldHandlerPlainTextEdit *w;
  clear();
  w = new mdtSqlFieldHandlerPlainTextEdit;
  w->setPlainTextEdit(widget);
  pvDataWidget = w;
  setDataWidgetAttributes();
  connect(w, SIGNAL(dataEdited()), this, SLOT(onDataEdited()));
}

void mdtSqlFieldHandler::setDataWidget(QComboBox* widget)
{
  Q_ASSERT(widget != 0);

  mdtSqlFieldHandlerComboBox *w;
  clear();
  w = new mdtSqlFieldHandlerComboBox;
  w->setComboBox(widget);
  pvDataWidget = w;
  setDataWidgetAttributes();
  connect(widget, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onDataEdited(const QString&)));
  connect(widget, SIGNAL(editTextChanged(const QString&)), this, SLOT(onDataEdited(const QString&)));
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

  mdtSqlFieldHandlerDateTimeEdit *w;
  clear();
  w = new mdtSqlFieldHandlerDateTimeEdit;
  w->setDateTimeEdit(widget);
  pvDataWidget = w;
  setDataWidgetAttributes();
  connect(widget, SIGNAL(dateTimeChanged(const QDateTime&)), this, SLOT(onDataEdited(const QDateTime&)));
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
}

void mdtSqlFieldHandler::clear()
{
  delete pvDataWidget;
  pvDataWidget = 0;
  // Flags
  pvIsReadOnly = false;
  pvDataEdited = false;
}

void mdtSqlFieldHandler::clearWidgetData()
{
  Q_ASSERT(pvDataWidget != 0);

  pvDataWidget->clear();

  return;
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

void mdtSqlFieldHandler::setData(const QVariant & data)
{
  Q_ASSERT(pvDataWidget != 0);

  // Try to handle some date/time format
  if(data.type() == QVariant::String){
    switch(pvSqlField.type()){
      case QVariant::DateTime:
        pvDataWidget->setData(QDateTime::fromString(data.toString(), Qt::ISODate));
        break;
      case QVariant::Date:
        pvDataWidget->setData(QDate::fromString(data.toString(), Qt::ISODate));
        break;
      case QVariant::Time:
        pvDataWidget->setData(QTime::fromString(data.toString(), Qt::ISODate));
        break;
      default:
        pvDataWidget->setData(data);
    }
  }else{
    pvDataWidget->setData(data);
  }
  updateFlags();
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
  /**
  switch(pvSqlField.type()){
    case QVariant::String:
      if(pvSqlField.requiredStatus() != QSqlField::Required){
        return "";
      }
      break;
    default:
      return data;
  }
  */

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

void mdtSqlFieldHandler::onDataEdited()
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
  ///qDebug() << "mdtSqlFieldHandler::onDataEdited() - x: " << x << ", valid: " << isValid << ", pvDataEdited: " << pvDataEdited;
  if(!pvDataEdited){
    pvDataEdited = true;
    ///qDebug() << "mdtSqlFieldHandler::onDataEdited() - emit dataEdited() ...";
    emit dataEdited();
  }
  setDataWidgetOk();
}

void mdtSqlFieldHandler::onDataEdited(bool state)
{
}

void mdtSqlFieldHandler::onDataEdited(const QDateTime & datetime)
{
  // Set the data edited flag
  if(!pvDataEdited){
    pvDataEdited = true;
    emit dataEdited();
  }
  setDataWidgetOk();
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
