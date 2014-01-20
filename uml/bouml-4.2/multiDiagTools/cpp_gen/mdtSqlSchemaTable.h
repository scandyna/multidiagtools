#ifndef _MDTSQLSCHEMATABLE_H
#define _MDTSQLSCHEMATABLE_H


class mdtSqlSchemaTable
{
  private:
    QString pvDriverName;

    QString pvDatabaseName;

    QString pvTableName;

    bool pvpvDropTableIfExists;

    QString pvMySqlEngineName;

    QList<QSqlField> pvFields;

    hash<QSqlIndex> pvIndexes;

    QSqlIndex pvPrimaryKey;


  public:
  void setDriverName(const QString & name);

  void setDatabaseName(const QString & name);

  void setTableName(const QString & name, bool dropIfExists);

  void setMysqlEngine(const QString & name);

  void addField(const QSqlField & field, bool isPartOfPrimaryKey);

  void addIndex(const QString & name, bool unique);

  bool addFieldToIndex(const QString & indexName, const QString & fieldName);

  void addForeignKey(const QString & name, const QString & referingTableName, const foreignKeyAction_t & actionOnDelete, const foreignKeyAction_t & actionOnUpdate);

  bool addFieldToForeignKey(const QString & foreignKeyName, const QString & fieldName, const QString & refferingFieldName);

  QString sqlForCreateTable();

  QString sqlForDropTable();


  private:
  QString sqlForCreateTableMySql();

  QString sqlForCreateTableSqlite();

    hash<QSqlIndex> pvForeignKeys;

};

#endif // #ifndef MDTSQLSCHEMATABLE_H
