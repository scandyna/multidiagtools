#ifndef _MDTSQLDATABASEMANAGER_H
#define _MDTSQLDATABASEMANAGER_H


class mdtSqlSchemaTable;

class mdtSqlDatabaseManager : public QObject
{
  private:
    QSqlDatabase pvDatabase;

    QString pvDatabaseName;

    QWidget pvForDialogParentWidget;

    bool pvInteractiveMode;

    mdtError pvLastError;


  public:
  mdtSqlDatabaseManager(const QObject & parent);

  ~mdtSqlDatabaseManager();

  void setDatabase(const QSqlDatabase & db);

  QSqlDatabase database();

  void setForDialogParentWidget(QWidget value);

  void setInteractiveMode(bool value);

  inline const bool InteractiveMode() const;

  inline const mdtError LastError() const;

  QFileInfo chooseDatabaseSqlite(const QDir & directory);

  bool openDatabaseSqlite(const QFileInfo & fileInfo, const QString & connectionName);

  bool createDatabaseSqlite(const createMode_t & createMode, const QString & connectionName);

  bool createDatabaseSqlite(const QFileInfo & fileInfo, const createMode_t & createMode, const QString & connectionName);

  bool createTable(const mdtSqlSchemaTable & table, const createMode_t & createMode);

};
inline const bool mdtSqlDatabaseManager::InteractiveMode() const 
{
  return pvInteractiveMode;
}

inline const mdtError mdtSqlDatabaseManager::LastError() const 
{
  return pvLastError;
}


#endif // #ifndef MDTSQLDATABASEMANAGER_H
