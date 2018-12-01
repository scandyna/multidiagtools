/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_ITEM_MODEL_ABSTRACT_EDITABLE_CACHED_TABLE_MODEL_H
#define MDT_ITEM_MODEL_ABSTRACT_EDITABLE_CACHED_TABLE_MODEL_H

#include "AbstractReadOnlyCachedTableModel.h"
#include "Mdt/Container/TableCacheOperation.h"
#include "Mdt/Container/TableCacheTransaction.h"
#include "Mdt/Container/TableCacheRowTransaction.h"
#include "Mdt/Container/TableCacheRowTransactionList.h"
#include "Mdt/Container/TableCacheOperationMap.h"

#include "Mdt/Error.h"

#include "MdtItemModelExport.h"

namespace Mdt{ namespace ItemModel{

  /*! \brief Cache between a storage and a view
   *
   * The main goal is the same as AbstractReadOnlyCachedTableModel.
   *
   * AbstractEditableCachedTableModel does not know how to fetch the data,
   *  and therefore it can not be instantiated.
   *
   * A concrete class has to be implemented:
   * \code
   * class EditPersonTableModel : public Mdt::ItemModel::AbstractEditableCachedTableModel
   * {
   *  Q_OBJECT
   *
   *  public:
   *
   *   EditPersonTableModel(QObject *parent = nullptr);
   *
   *   int columnCount(const QModelIndex & parent = QModelIndex()) const override
   *   {
   *     Q_UNUSED(parent)
   *     return 5;
   *   }
   *
   *  private:
   *
   *   QVariant horizontalHeaderDisplayRoleData(int column) const override;
   *   bool fetchRecords(int count) override;
   *   bool addRecordToBackend(int row) override;
   * };
   * \endcode
   */
  class MDT_ITEMMODEL_EXPORT AbstractEditableCachedTableModel : public AbstractReadOnlyCachedTableModel
  {
   Q_OBJECT

   using BaseClass = AbstractReadOnlyCachedTableModel;

  public:

    /*! \brief Constructor
     */
    AbstractEditableCachedTableModel(QObject *parent = nullptr);

    AbstractEditableCachedTableModel(const AbstractEditableCachedTableModel &) = delete;
    AbstractEditableCachedTableModel & operator=(const AbstractEditableCachedTableModel &) = delete;
    AbstractEditableCachedTableModel(AbstractEditableCachedTableModel &&) = delete;
    AbstractEditableCachedTableModel & operator=(AbstractEditableCachedTableModel &&) = delete;

    /*! \brief Get flags for \a index
     */
    Qt::ItemFlags flags(const QModelIndex & index) const override;

    /*! \brief Set data for \a role at \a index to \a value
     */
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;

    /*! \brief Insert \a count rows before \a row into the model
     *
     * If the cache of this model cannot store \a count element
     *  this method does nothing and returns false.
     *
     * \pre \a row must be in valid range ( 0 <= \a row <= rowCount() )
     * \pre \a count muste be >= 1
     */
    bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex()) override;

    /*! \brief Submit changes
     *
     * Will submit changes done in the cache to the backend.
     */
    bool submitChanges();

//     /*! \brief Fetch the record at \a row from the backend
//      *
//      * \pre \a row must be in valid range ( 0 <= \a row < rowCount() )
//      */
//     bool fetchRow(int row);

   protected:

    /*! \brief Insert \a count copies of \a record before \a row to the cache of this model
     *
     * \pre \a row must be >= 0 and <= rowCount()
     * \pre \a count must be >= 1
     * \pre rowCount() + \a count must be <= cachedRowCountLimit()
     *   \todo Full cache must no longer be a precondition.
     *         If the cache is full, the record will simply not be stored
     * \pre \a record 's columnt count must be the same as columnCount()
     */
    void fromBackendInsertRecords(int row, int count, const Mdt::Container::VariantRecord & record) override;

    /*! \brief Get the display role data for the vertical header
     *
     * This method is called by headerData() if all preconditions ar satisfied.
     *
     * This implementation returns a character corresponding to a operation if one is pending for \a row ,
     *  otherwise it returns the implementation from QAbstractTableModel.
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     */
    QVariant verticalHeaderDisplayRoleData(int row) const override;

    /*! \brief Add the record at \a row to the backend
     *
     * This method must be implemented in the concrete class.
     *
     * Once the record has been successfully processed from the backend,
     *  fromBackendSetRecord() should be called to update the corresponding
     *  record to the new values.
     *
     * This method can be implemented in a synchronous (blocking) way:
     * \code
     * // Create a person from the data at given row
     * const Person person = makePerson(row);
     *
     * if(!mRepository.addPerson(person)){
     *   return false;
     * }
     *
     * return true;
     * \endcode
     *
     * It is also possible to 
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     */
//     virtual bool addRecordToBackend(int row) = 0;

    /*! \brief Update the state of the row corresponding to \a transaction
     *
     * \pre \a transaction must not be null
     * \pre \a rocord must have columnCount() columns
     */
    void transactionSucceeded(const Mdt::Container::TableCacheTransaction & transaction, const Mdt::Container::VariantRecord record);

    /*! \brief Update the state of the row corresponding to \a transaction
     *
     * \pre \a transaction must not be null
     */
    void transactionFailed(const Mdt::Container::TableCacheTransaction & transaction, const Mdt::Error & error);

//     /*! \brief Fetch the record at row from the backend
//      *
//      * If the concrete model supports fetching a single row,
//      *  this method should be implemented.
//      *
//      * This default implementation does nothing and returns false.
//      *
//      * Once the record has been successfully processed from the backend,
//      *  transactionSucceeded() should be called to update the corresponding
//      *  record to the new values,
//      *  or transactionFailed() on error.
//      *
//      * This method can be implemented in a synchronous (blocking) way:
//      * \code
//      * bool MyTableModel::fetchRecordFromBackend(const Mdt::Container::TableCacheRowTransaction & rowTransaction) override
//      * {
//      *   const int personId = index(rowTransaction.row(), 0).data().toInt();
//      *   const auto person = mRepository.getById(personId);
//      *   if(!person){
//      *     transactionFailed(rowTransaction.transaction(), mRepository.lastError());
//      *     return false;
//      *   }
//      *   const VariantRecord record = makeRecord(person);
//      *   transactionSucceeded(rowTransaction.transaction(), record);
//      *
//      *   return true;
//      * }
//      * \endcode
//      *
//      * It is also possible to implement this method in a asynchronous way.
//      *  For this example, we will use Qt signal/slots, and a transaction id.
//      *  We will have 2 slots: one that is called on success, the other on failure.
//      *  A example of those slots is described in addRecordToBackend() .
//      *
//      * The asynchronous implementation could look like:
//      * \code
//      * bool MyTableModel::fetchRecordFromBackend(const Mdt::Container::TableCacheRowTransaction & rowTransaction) override
//      * {
//      *   const int personId = index(rowTransaction.row(), 0).data().toInt();
//      *
//      *   mRepository.submitGetById(rowTransaction.transactionId(), personId));
//      *
//      *   return true;
//      * }
//      * \endcode
//      *
//      * \pre the row given in \a rowTransaction must be in valid range ( 0 <= \a row < rowCount() ).
//      */
//     virtual bool fetchRecordFromBackend(const Mdt::Container::TableCacheRowTransaction & rowTransaction);

    /*! \brief Add the record at row to the backend
     *
     * This method must be implemented in the concrete class.
     *
     * Once the record has been successfully processed from the backend,
     *  taskSucceeded() should be called to update the corresponding
     *  record to the new values,
     *  or taskFailed() on error.
     *
     * This method can be implemented in a synchronous (blocking) way:
     * \code
     * bool MyTableModel::addRecordToBackend(const Mdt::Container::TableCacheRowTransaction & rowTask) override
     * {
     *   using namespace Mdt::Container;
     *
     *   // Create a person from the data at given row
     *   const Person person = makePerson(rowTask.row());
     *
     *   if(!mRepository.addPerson(person)){
     *     taskFailed(rowTask.task(), mRepository.lastError());
     *     return false;
     *   }
     *   // The repository probably have assigned a id to person
     *   const VariantRecord record = makeRecord(person);
     *   taskSucceeded(rowTask.task(), record);
     *
     *   return true;
     * }
     * \endcode
     *
     * It is also possible to implement this method in a asynchronous way.
     *  For this example, we will use Qt signal/slots, and a task id.
     *  We will have 2 slots: one that is called on success, the other on failure.
     *
     * Here is a possible implementation of a slot that is called on success:
     * \code
     * void MyTableModel::onRepositoryTaskSucceeded(int taskId, const Person & person)
     * {
     *   using namespace Mdt::Container;
     *
     *   const TableCacheTask task(taskId);
     *   const VariantRecord record = makeRecord(person);
     *
     *   taskSucceeded(task, record);
     * }
     * \endcode
     *
     * Here is a example of the slot that is called on failure:
     * \code
     * void MyTableModel::onRepositoryTaskFailed(int taskId, const Mdt::Error & error)
     * {
     *   using namespace Mdt::Container;
     *
     *   const TableCacheTask task(taskId);
     *
     *   taskFailed(task, error);
     * }
     * \endcode
     *
     * The asynchronous implementation could look like:
     * \code
     * bool MyTableModel::addRecordToBackend(const Mdt::Container::TableCacheRowTransaction & rowTask) override
     * {
     *   // Create a person from the data at given row
     *   const Person person = makePerson(rowTask.row());
     *
     *   mRepository.submitAddPerson(rowTask.taskId(), person));
     *
     *   return true;
     * }
     * \endcode
     *
     * \pre the row given in \a rowTask must be in valid range ( 0 <= \a row < rowCount() ).
     */
    virtual bool addRecordToBackend(const Mdt::Container::TableCacheRowTransaction & rowTask) = 0;

    /*! \brief Add records for \a rowTransactions to the backend
     *
     * This method can be implemented to add all records
     *  for \a rowTransactions .
     *
     * The default implementation will call addRecordToBackend()
     *  for each row transaction.
     *
     * \pre each row in \a rowTransactions must be in valid range ( 0 <= \a row < rowCount() ).
     */
    virtual bool addRecordsToBackend(const Mdt::Container::TableCacheRowTransactionList & rowTransactions);

    /*! \brief Update the record at row in the backend
     *
     * Once the record has been successfully processed from the backend,
     *  transactionSucceeded() should be called to update the corresponding
     *  record to the new values,
     *  or transactionFailed() on error.
     *
     * \pre the row given in \a rowTransaction must be in valid range ( 0 <= \a row < rowCount() ).
     */
    virtual bool updateRecordInBackend(const Mdt::Container::TableCacheRowTransaction & rowTransaction) = 0;

    /*! \brief Update records for \a rowTransactions in the backend
     *
     * This method can be implemented to update all records
     *  for \a rowTransactions .
     *
     * The default implementation will call updateRecordInBackend()
     *  for each row transaction.
     *
     * \pre each row in \a rowTransactions must be in valid range ( 0 <= \a row < rowCount() ).
     */
    virtual bool updateRecordsInBackend(const Mdt::Container::TableCacheRowTransactionList & rowTransactions);

   private:

    bool addNewRecordsToBackend();
    bool updateModifiedRowsInBackend();

//     Mdt::Container::TableCacheOperation operationAtRow(int row) const;

    Mdt::Container::TableCacheOperationMap mOperationMap;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_ABSTRACT_EDITABLE_CACHED_TABLE_MODEL_H
