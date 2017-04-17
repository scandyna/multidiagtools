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
#ifndef MDT_ITEM_EDITOR_ABSTRACT_SQL_CONTROLLER_H
#define MDT_ITEM_EDITOR_ABSTRACT_SQL_CONTROLLER_H

#include "Mdt/ItemEditor/AbstractController.h"
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QPointer>
#include <QString>

namespace Mdt{ namespace Sql{ namespace Schema{

  class Table;

  template<typename T>
  class TableTemplate;

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

namespace Mdt{ namespace ItemEditor{

  /*! \brief Common base for SQL controllers
   */
  class AbstractSqlController : public AbstractController
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit AbstractSqlController(QObject* parent = nullptr);

    /*! \brief Construct a controller that acts on \a db
     */
//     explicit AbstractSqlController(QObject *parent, const QSqlDatabase & db);

    /*! \brief Construct a controller that acts on \a db
     */
//     explicit AbstractSqlController(const QSqlDatabase & db);

    // Copy disabled
    AbstractSqlController(const AbstractSqlController &) = delete;
    AbstractSqlController & operator=(const AbstractSqlController &) = delete;
    // Move disabled
    AbstractSqlController(AbstractSqlController &&) = delete;
    AbstractSqlController & operator=(AbstractSqlController &&) = delete;

    /*! \brief Check if it is allowed to select for current controller state
     */
    bool canSelect() const;

    /*! \brief Set default model
     *
     * Will use a QSqlTableModel that acts on \a db
     *
     * \note If a model was previously set with setModel(),
     *        it will not be deleted.
     */
    void setDefaultModel(const QSqlDatabase & db);

    /*! \brief Set model
     *
     * \note Because model can be shared with several objects (f.ex. other views),
     *        the controller does not take ownership of it (it will not delete it).
     * \pre model must be a valid pointer
     */
    void setModel(QSqlTableModel *model);

    /*! \brief Get model
     *
     * Returns the model set with setModel(),
     *  or a nullptr if no one was set,
     *  or model was delete elsewhere in application.
     */
    QSqlTableModel *model() const
    {
      return mModel;
    }

    /*! \brief Set table name
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
    void setTableName(const QString & name);

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
    void setTable(const Mdt::Sql::Schema::Table & table);

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
    void setTable(const Mdt::Sql::Schema::TableTemplate<T> & table)
    {
      setTableName(table.tableName());
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

    QPointer<QSqlTableModel> mModel;
    QPointer<QSqlTableModel> mOwningModel;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ABSTRACT_SQL_CONTROLLER_H
