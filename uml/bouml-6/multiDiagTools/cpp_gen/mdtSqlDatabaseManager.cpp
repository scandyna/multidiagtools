
#include "mdtSqlDatabaseManager.h"
#include "mdtSqlSchemaTable.h"

mdtSqlDatabaseManager::mdtSqlDatabaseManager(const QObject & parent) 
{
}

mdtSqlDatabaseManager::~mdtSqlDatabaseManager() 
{
}

void mdtSqlDatabaseManager::setDatabase(const QSqlDatabase & db) 
{
}

QSqlDatabase mdtSqlDatabaseManager::database() 
{
}

void mdtSqlDatabaseManager::setForDialogParentWidget(QWidget value) 
{
  pvForDialogParentWidget = value;
}

void mdtSqlDatabaseManager::setInteractiveMode(bool value) 
{
  pvInteractiveMode = value;
}

QFileInfo mdtSqlDatabaseManager::chooseDatabaseSqlite(const QDir & directory) 
{
}

bool mdtSqlDatabaseManager::openDatabaseSqlite(const QFileInfo & fileInfo, const QString & connectionName) 
{
}

bool mdtSqlDatabaseManager::createDatabaseSqlite(const createMode_t & createMode, const QString & connectionName) 
{
}

bool mdtSqlDatabaseManager::createDatabaseSqlite(const QFileInfo & fileInfo, const createMode_t & createMode, const QString & connectionName) 
{
}

bool mdtSqlDatabaseManager::createTable(const mdtSqlSchemaTable & table, const createMode_t & createMode) 
{
}

