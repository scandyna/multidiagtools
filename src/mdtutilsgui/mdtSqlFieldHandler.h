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
#ifndef MDT_SQL_FIELD_HANDLER_H
#define MDT_SQL_FIELD_HANDLER_H

#include <QSqlField>
#include <QDateTime>
#include <QPalette>
#include <QVariant>
#include <QWidget>

class mdtSqlFieldHandlerAbstractDataWidget;
class QLineEdit;
class QSpinBox;
class QDoubleSpinBox;
class QDateTimeEdit;
class QAbstractButton;
class QComboBox;
class mdtDoubleEdit;

/*! \brief Handle events between a SQL field and a edition widget
 *
 * When writing a GUI for a SQL database, Qt offers several classes.
 *  For example, QSqlTableModel, QDataWidgetMapper, QLineEdit, ...
 *
 * mdtSqlFieldHandler add some functions that will help keeping the GUI
 *  more coherent with the underlaying database.
 *
 * To use this class, first set the field with setField(),
 *  then add your edition/view widget with setDataWidget().
 *
 * If field must be read only (independent of field access set in QSqlField),
 *  use setReaOnly().
 */
class mdtSqlFieldHandler : public QWidget
{
 Q_OBJECT
 Q_PROPERTY(QVariant data READ data WRITE setData USER true)

 public:

  /*! \brief Construct a null field handler
   *
   * See clear() for default flags.
   */
  mdtSqlFieldHandler(QWidget *parent = 0);

  /*! \brief Destructor
   */
  ~mdtSqlFieldHandler();

  /*! \brief Set SQL field
   *
   * Store the sql field.
   *  Some field's attributes will be used to handle edit/view widget.
   */
  void setField(const QSqlField &field);

  /*! \brief Set the readOnly flag
   *
   * If set, readOnly flag will force
   *  the edit/view widget to be not editable.
   */
  void setReadOnly(bool readOnly);

  /*! \brief Get the readOnly flag
   */
  bool isReadOnly() const;

  /*! \brief Set the edit/view widget
   *
   * Will search what type given widget is,
   *  cast it and store if found.
   *
   * \pre widget must be a valid pointer.
   */
  void setDataWidget(QWidget *widget);

  /*! \brief Set the edit/view widget
   *
   * \pre widget must be a valid pointer.
   */
  void setDataWidget(QLineEdit *widget);

  /*! \brief Set the edit/view widget
   *
   * \pre widget must be a valid pointer.
   */
  void setDataWidget(QComboBox *widget);

  /*! \brief Set the edit/view widget
   *
   * \pre widget must be a valid pointer.
   */
  void setDataWidget(mdtDoubleEdit *widget);

  /*! \brief Set the edit/view widget
   *
   * \pre widget must be a valid pointer.
   */
  void setDataWidget(QAbstractButton *widget);

  /*! \brief Set the edit/view widget
   *
   * \pre widget must be a valid pointer.
   */
  void setDataWidget(QDateTimeEdit *widget);

  /*! \brief Set the edit/view widget
   *
   * \pre widget must be a valid pointer.
   */
  void setDataWidget(QDoubleSpinBox *widget);

  /*! \brief Set the edit/view widget
   *
   * \pre widget must be a valid pointer.
   */
  void setDataWidget(QSpinBox *widget);

  /*! \brief Get data widget
   * Get current edit/view widget
   *
   * \return A QWidget of current edit/view, or 0 if none was set.
   */
  QWidget *dataWidget();

  /*! \brief Clear
   *
   * Will unref (set to null) all internall widget pointers
   *  and set default flags.
   *
   * Default flags are:
   *  - isReadOnly: false
   *  - isNull: true
   *  - dataWasEdited: false
   */
  void clear();

  /*! \brief Clear widget's data
   *
   * Will only clear the widget's content.
   */
  void clearWidgetData();

  /*! \brief Get the null flag
   */
  bool isNull() const;

  /*! \brief Get the dataWasEdited flag
   */
  bool dataWasEdited() const;

  /*! \brief Make some checks before submit data.
   */
  bool checkBeforeSubmit();

  /*! \brief Set data widget's data
   */
  void setData(const QVariant &data);

  /*! \brief Get data widget's data
   */
  QVariant data() const;

 signals:

  /*! \brief Emitted the first time the user has edited data.
   */
  void dataEdited();

 public slots:

  /*! \brief Update some internal flags
   *
   * Some flags, like Null flag,
   *  or data edited flag are 
   *  updated here.
   *
   * QDataWidgetMapper::currentIndexChanged() signal should be connected to this slot.
   */
  void updateFlags();

 private slots:

  /*! \brief Used together with edit widget to set flags
   */
  void onDataEdited(const QString &text);

  /*! \brief Used together with edit widget to set flags
   */
  void onDataEdited(double x, bool isValid);

  /*! \brief Used together with edit widget to set flags
   */
  void onDataEdited(bool state);

  /*! \brief Used together with edit widget to set flags
   */
  void onDataEdited(const QDateTime &datetime);

 private:

  /*! \brief Clear the edit/view widget
   */
  void clearDataWidget();

  /*! \brief Set edit/view widget attributes regarding SQL field and internal flags
   */
  void setDataWidgetAttributes();

  /*! \brief Set the widget marked as not ok
   *
   * This will change the baground color
   *  and set toolTip.
   */
  void setDataWidgetNotOk(const QString &toolTip);

  /*! \brief Set the widget marked as ok
   *
   * This will restore the original background.
   */
  void setDataWidgetOk();

  // Edition/view widgets (only one will be used)

  mdtSqlFieldHandlerAbstractDataWidget *pvDataWidget;

  /**
  QAbstractButton *pvAbstractButton;
  QDateTimeEdit *pvDateTimeEdit;
  QDoubleSpinBox *pvDoubleSpinBox;
  QSpinBox *pvSpinBox;
  QComboBox *pvComboBox;
  */
  QSqlField pvSqlField;
  // Flags
  bool pvIsReadOnly;
  bool pvDataEdited;
  QPalette pvDataWidgetOriginalPalette;

  Q_DISABLE_COPY(mdtSqlFieldHandler);
};

#endif  // #ifndef MDT_SQL_FIELD_HANDLER_H
