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
#ifndef MDT_SQL_ASYNC_QUERY_THREAD_H
#define MDT_SQL_ASYNC_QUERY_THREAD_H

#include "ConnectionParameters.h"
#include "Mdt/Container/VariantRecord.h"
#include "Mdt/Error.h"
#include "MdtSql_CoreExport.h"
#include <QSqlDatabase>
#include <QThread>
#include <QObject>
#include <QString>
#include <QStringList>
#include <memory>

class QSqlQuery;

namespace Mdt{ namespace Sql{

  /*! \internal
   */
  class MDT_SQL_CORE_EXPORT AsyncQueryThreadWorker : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit AsyncQueryThreadWorker(QObject *parent = nullptr);

    /*! \brief Destructor
     */
    ~AsyncQueryThreadWorker();

    AsyncQueryThreadWorker(const AsyncQueryThreadWorker &) = delete;
    AsyncQueryThreadWorker & operator=(const AsyncQueryThreadWorker &) = delete;
    AsyncQueryThreadWorker(AsyncQueryThreadWorker &&) = delete;
    AsyncQueryThreadWorker & operator=(AsyncQueryThreadWorker &&) = delete;

    /*! \brief Setup database
     */
    bool setup(const ConnectionParameters & parameters);

    /*! \brief Get setup error
     */
    Mdt::Error setupError() const
    {
      return mSetupError;
    }

    /*! \brief Get connection name
     */
    QString connectionName() const
    {
      return mDatabase->connectionName();
    }

    /*! \brief Generate a connection name regarding the existing onces
     */
    static QString generateConnectionName(const QStringList & existingNames);

   public slots:

    /*! \brief Process a new query
     */
    void processQuery(const QString & sql, int instanceId);

   signals:

    /*! \brief Emitted whenever a new record is available
     */
    void newRecordAvailable(const Mdt::Container::VariantRecord & record, int instanceId);

    /*! \brief Emitted whenever a error occured
     *
     * If \a instanceId is 0, the error does not concern a specific query,
     *  but it is a global error (sort of broadcast).
     */
    void errorOccured(const Mdt::Error & error, int instanceId);

   private:

    static Mdt::Container::VariantRecord variantRecordFromQuery(const QSqlQuery & query, int columnCount);

    std::unique_ptr<QSqlDatabase> mDatabase;
    Mdt::Error mSetupError;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_ASYNC_QUERY_THREAD_H
