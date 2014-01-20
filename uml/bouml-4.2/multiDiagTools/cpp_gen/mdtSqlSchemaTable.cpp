
#include "mdtSqlSchemaTable.h"

void mdtSqlSchemaTable::setDriverName(const QString & name) 
{
}

void mdtSqlSchemaTable::setDatabaseName(const QString & name) 
{
}

void mdtSqlSchemaTable::setTableName(const QString & name, bool dropIfExists) 
{
}

void mdtSqlSchemaTable::setMysqlEngine(const QString & name) 
{
}

void mdtSqlSchemaTable::addField(const QSqlField & field, bool isPartOfPrimaryKey) 
{
}

void mdtSqlSchemaTable::addIndex(const QString & name, bool unique) 
{
}

bool mdtSqlSchemaTable::addFieldToIndex(const QString & indexName, const QString & fieldName) 
{
}

void mdtSqlSchemaTable::addForeignKey(const QString & name, const QString & referingTableName, const foreignKeyAction_t & actionOnDelete, const foreignKeyAction_t & actionOnUpdate) 
{
}

bool mdtSqlSchemaTable::addFieldToForeignKey(const QString & foreignKeyName, const QString & fieldName, const QString & refferingFieldName) 
{
}

QString mdtSqlSchemaTable::sqlForCreateTable() 
{
}

QString mdtSqlSchemaTable::sqlForDropTable() 
{
}

QString mdtSqlSchemaTable::sqlForCreateTableMySql() 
{
}

QString mdtSqlSchemaTable::sqlForCreateTableSqlite() 
{
}

