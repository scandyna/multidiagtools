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
#ifndef MDT_ABSTRACT_SQL_WIDGET_H
#define MDT_ABSTRACT_SQL_WIDGET_H

#include <QWidget>

class QSqlTableModel;

/*! \brief Base class to create database table GUI
 *
 * Main goal is to provide a standard way to use
 *  GUI object for database table.
 */
class mdtAbstractSqlWidget : public QWidget
{
 Q_OBJECT

  public:

  /*! \brief Constructor
   */
  mdtAbstractSqlWidget(QWidget *parent = 0);

  /*! \brief Destructor
   */
  virtual ~mdtAbstractSqlWidget();

  /*! \brief Set the table model
   *
   * Subclass must implement this method.
   *
   * \pre model must be a valid pointer
   */
  virtual void setModel(QSqlTableModel *model) = 0;

  /*! \brief Get the current row
   *
   * Current row can be the currently selected row
   *  (case of a table view),
   *  or simply the current row that is displayed in a form view.
   *
   * Subclass must implement this method.
   */
  virtual int currentRow() const = 0;

 public slots:

  /*! \brief Submit current record to model
   *
   * Subclass must implement this method.
   *  Errors and user interactions must be handled by subclass.
   *
   * Depending on subclass choosen EditStrategy,
   *  this method has potentially no effect.
   */
  virtual void submit() = 0;

  /*! \brief Revert current record from model
   *
   * Subclass must implement this method.
   *  Errors and user interactions must be handled by subclass.
   *
   * Depending on subclass choosen EditStrategy,
   *  this method has potentially no effect.
   */
  virtual void revert() = 0;

  /*! \brief Insert a new row to model
   *
   * Subclass must implement this method.
   *  Errors and user interactions must be handled by subclass.
   */
  virtual void insert() = 0;

  /*! \brief Remove a row from model
   *
   * Subclass must implement this method.
   *  Errors and user interactions must be handled by subclass.
   */
  virtual void remove() = 0;

  /*! \brief Set first record as current record
   *
   * Subclass must implement this method.
   *  Errors and user interactions must be handled by subclass.
   */
  virtual void toFirst() = 0;

  /*! \brief Set last record as current record
   *
   * Subclass must implement this method.
   *  Errors and user interactions must be handled by subclass.
   */
  virtual void toLast() = 0;

  /*! \brief Set previous record as current record
   *
   * Subclass must implement this method.
   *  Errors and user interactions must be handled by subclass.
   */
  virtual void toPrevious() = 0;

  /*! \brief Set next record as current record
   *
   * Subclass must implement this method.
   *  Errors and user interactions must be handled by subclass.
   */
  virtual void toNext() = 0;
};

#endif  // #ifndef MDT_ABSTRACT_SQL_WIDGET_H
