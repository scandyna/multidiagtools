/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#ifndef MDT_ITEM_EDITOR_ABSTRACT_SQL_TABLE_MODEL_CONTROLLER_H
#define MDT_ITEM_EDITOR_ABSTRACT_SQL_TABLE_MODEL_CONTROLLER_H

#include "AbstractSqlController.h"
#include <QSqlTableModel>
#include <QPointer>

class QAbstractItemModel;

namespace Mdt{ namespace Sql{ namespace Schema{

  class Table;

  template<typename T>
  class TableTemplate;

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

namespace Mdt{ namespace ItemEditor{

  /*! \brief Base class to create controllers acting on a QSqlTableModel
   */
  class AbstractSqlTableModelController : public AbstractSqlController
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit AbstractSqlTableModelController(QObject* parent = nullptr);

    // Copy disabled
    AbstractSqlTableModelController(const AbstractSqlTableModelController &) = delete;
    AbstractSqlTableModelController & operator=(const AbstractSqlTableModelController &) = delete;
    // Move disabled
    AbstractSqlTableModelController(AbstractSqlTableModelController &&) = delete;
    AbstractSqlTableModelController & operator=(AbstractSqlTableModelController &&) = delete;

    /*! \brief Check if it is allowed to select for current controller state
     */
    bool canSelect() const;

    /*! \brief Set model
     *
     * \note Because model can be shared with several objects (f.ex. other views),
     *        the controller does not take ownership of it (it will not delete it).
     * \note \a model 's edit strategy will be set to QSqlTableModel::OnManualSubmit
     * \pre \a model must be a valid pointer
     * \pre \a model must be a QSqlTableModel (or a subclass of it)
     */
    void setModel(QAbstractItemModel *model) override;

    /*! \brief Set default model
     *
     * Will use a QSqlTableModel that acts on \a db connection
     *
     * \note If a model was previously set with setModel(),
     *        it will be replaced but not be deleted.
     */
    void setDefaultModel(const QSqlDatabase & db);

    /*! \brief Get model
     *
     * Returns the model set with setModel(),
     *  or a nullptr if no one was set,
     *  or model was delete elsewhere in application.
     */
    QSqlTableModel *model() const;

    /*! \brief Set table name
     *
     * Will fetch fields informations from table,
     *  set primary key and foreign keys.
     *
     * Entity name will also be set to \a name.
     *
     * Does not select data from the database table.
     *
     * \pre A model must be set before calling this method
     * \sa setModel()
     * \sa setDefaultModel()
     * \sa setTable(const Mdt::Sql::Schema::TableTemplate)
     * \sa setTable(const Mdt::Sql::Schema::Table)
     * \sa select()
     */
    bool setTableName(const QString & name);

    /*! \brief Set table
     *
     * Does not select data from the database table.
     *
     * \pre A model must be set before calling this method
     * \sa setModel()
     * \sa setDefaultModel()
     * \sa setTable(const Mdt::Sql::Schema::TableTemplate)
     * \sa setTableName()
     * \sa select()
     */
    bool setTable(const Mdt::Sql::Schema::Table & table);

    /*! \brief Set table
     *
     * Does not select data from the database table.
     *
     * \pre A model must be set before calling this method
     * \sa setModel()
     * \sa setDefaultModel()
     * \sa setTable(const Mdt::Sql::Schema::Table)
     * \sa setTableName()
     * \sa select()
     */
    template<typename T>
    bool setTable(const Mdt::Sql::Schema::TableTemplate<T> & table)
    {
      Q_ASSERT(model() != nullptr);
      return setTableName(table.tableName());
    }

    /*! \brief Select data from database
     *
     * If current controller state does not permit to select now
     *  (for example, if some edition was not submitted),
     *  this method will do nothing and return false.
     *
     * If a error occured, returns false.
     *  See lastError() for details.
     *
     * \pre A model must be set before calling this method
     * \sa setModel()
     * \sa setDefaultModel()
     * \sa setTable(const Mdt::Sql::Schema::TableTemplate)
     * \sa setTable(const Mdt::Sql::Schema::Table)
     * \sa setTableName()
     * \sa lastError()
     */
    bool select();

   private:

    /*! \brief Remove selected rows
     */
    bool removeSelectedRows() override;

    bool submitChangesToStorage() override;
    void revertChangesFromStorage() override;

    QPointer<QSqlTableModel> mOwningModel;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ABSTRACT_SQL_TABLE_MODEL_CONTROLLER_H
