#ifndef _MDTCLDATABASESCHEMA_H
#define _MDTCLDATABASESCHEMA_H


#include "mdtSqlDatabaseManager.h"

class mdtSqlSchemaTable;

class mdtClDatabaseSchema
{
  private:
    QList<mdtSqlSchemaTable> pvTables;


  public:
  mdtClDatabaseSchema();

  ~mdtClDatabaseSchema();

  bool createSchemaSqlite();


  private:
  void setupTables();

  bool createTablesSqlite();

  bool createViews();

  void setupVehicleTypeTable();

  void setupVehicleTypeUnitTable();

  void setupVehicleTypeLinkTable();

  void setupArticleTable();

  void setupArticleComponentTable();

  void setupArticleConnectionTable();

  void setupArticleLinkTable();

  void setupUnitTable();

  void setupUnitConnectionTable();

  void setupLinkTable();

  void setupLinkDirectionTable();

  void setupLinkTypeTable();

  bool createView(const QString & viewName, const QString & sql);

  bool createVehicleTypeUnitView();

  bool createArticleComponentUsageView();

  bool createArticleComponentView();

  bool createArticleLinkView();

  bool createUnitView();

  bool createUnitComponentView();

  bool createUnitConnectionView();

  bool createUnitLinkView();

  bool createUnitVehicleTypeView();

  bool createLinkListView();

    mdtSqlDatabaseManager pvDatabaseManager;

};

#endif // #ifndef MDTCLDATABASESCHEMA_H
