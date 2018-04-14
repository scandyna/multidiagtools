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
#ifndef MDT_SQL_SQLITE_DATABASE_FILE_DIALOG_H
#define MDT_SQL_SQLITE_DATABASE_FILE_DIALOG_H

#include "MdtSql_WidgetsExport.h"
#include <QFileDialog>
#include <QString>

namespace Mdt{ namespace Sql{

  /*! \brief Provides a dialog that allow users to select SQLite database files
   */
  class MDT_SQL_WIDGETS_EXPORT SQLiteDatabaseFileDialog : public QFileDialog
  {
   Q_OBJECT

   public:

    /*! \brief Database file open mode
     */
    enum DbOpenMode
    {
      DbOpenExisting, /*!< Open a existing SQLite file */
      DbCreateNew     /*!< Create a new SQLite file */
    };

    /*! \brief Construct a file dialog to open a existing database file
     */
    SQLiteDatabaseFileDialog(QWidget *parent = nullptr);

    /*! \brief Construct a file dialog
     */
    SQLiteDatabaseFileDialog(QWidget *parent, DbOpenMode openMode);

    /*! \brief Set database file open mode
     */
    void setDbOpenMode(DbOpenMode mode);

    /*! \brief Get database file open mode
     */
    DbOpenMode dbOpenMode() const;

    /*! \brief Set file suffix
     *
     * By default, the suffix is sqlite and label is tr(SQLite files).
     *
     * \note \a suffix can be passed with or without the dot ('.').
     *     For example, passing .sqlite or sqlite is the same.
     *
     * \pre \a suffix must not be empty
     * \pre \a label must not be empty
     * \sa fileSuffix()
     * \sa fileSuffixLabel()
     */
    void setFileSuffix(const QString & suffix, const QString & label);

    /*! \brief Get file suffix
     *
     * \note the file suffix is allways returned without the dot ('.')
     *
     * \sa setFileSuffix()
     * \sa fileSuffixLabel()
     */
    QString fileSuffix() const
    {
      return mFileSuffix;
    }

    /*! \brief Get file suffix label
     */
    QString fileSuffixLabel() const
    {
      return mFileSuffixLabel;
    }

   private:

    QString mFileSuffix;
    QString mFileSuffixLabel;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_SQLITE_DATABASE_FILE_DIALOG_H
