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
#ifndef MDT_ABSTRACT_SQL_TABLE_CONTROLLER_H
#define MDT_ABSTRACT_SQL_TABLE_CONTROLLER_H

#include "mdtError.h"
#include <QObject>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QString>
#include <QVariant>
#include <memory>

/*! \brief Base class for SQL table controllers
 *
 * 
 */
class mdtAbstractSqlTableController : public QObject
{
  Q_OBJECT

 public:

  /*! \brief State of edit/view widget
   *
   * This state is set by internal state machine.
   */
  enum State_t {
                Visualizing = 0,  /*!< Visualizing state */
                Reverting,        /*!< Reverting state */
                Editing,          /*!< Editing state */
                Submitting,       /*!< Submitting state */
                Inserting,        /*!< Inserting state */
                EditingNewRow,    /*!< Editing a new row */
                RevertingNewRow,  /*!< RevertingNewRow state */
                SubmittingNewRow, /*!< SubmittingNewRow state */
                Removing          /*!< Removing state */
               };

  /*! \brief Constructor
   */
  mdtAbstractSqlTableController(QObject * parent = 0);

  /*! \brief Destructor
   */
  virtual ~mdtAbstractSqlTableController();

  /*! \brief Set table name
   *
   * Will replace internal model if allready set.
   */
  void setTableName(const QString & tableName, QSqlDatabase db, const QString & userFriendlyTableName = QString());

  /*! \brief Set table model
   *
   * Will replace internal model if allready set.
   */
  void setModel(std::shared_ptr<QSqlTableModel> m, const QString & userFriendlyTableName = QString());

  /*! \brief Select data in main table
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  bool select();

  /*! \brief Get model
   */
  inline std::shared_ptr<QSqlTableModel> model() { return pvModel; }

  /*! \brief Get user friendly table name
   */
  QString userFriendlyTableName() const { return pvUserFriendlyTableName; }

  /*! \brief Get last error
   */
  mdtError lastError() const { return pvLastError; }

 protected:

  /*! \brief Last error (for write access)
   */
  mdtError pvLastError;

 private:

  Q_DISABLE_COPY(mdtAbstractSqlTableController);

  std::shared_ptr<QSqlTableModel> pvModel;
  QString pvUserFriendlyTableName;
};


#endif  // #ifndef MDT_ABSTRACT_SQL_TABLE_CONTROLLER_H
