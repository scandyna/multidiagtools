DROP VIEW IF EXISTS UnitConnection_view;
CREATE VIEW UnitConnection_view AS
SELECT
 UnitConnection_tbl.Id_PK AS UnitConnection_Id_PK ,
 UnitConnection_tbl.Unit_Id_FK ,
 UnitConnection_tbl.ArticleConnection_Id_FK ,
 UnitConnection_tbl.UnitConnectorName ,
 UnitConnection_tbl.UnitContactName ,
 UnitConnection_tbl.SchemaPage ,
UnitConnection_tbl.FunctionEN AS UnitFunctionEn ,
 UnitConnection_tbl.SignalName ,
 UnitConnection_tbl.SwAddress ,
 ArticleConnection_tbl.ArticleConnectorName ,
 ArticleConnection_tbl.ArticleContactName ,
 ArticleConnection_tbl.IoType ,
 ArticleConnection_tbl.FunctionEN AS ArticleFunctionEn
FROM UnitConnection_tbl
 LEFT JOIN ArticleConnection_tbl
  ON UnitConnection_tbl.ArticleConnection_Id_FK = ArticleConnection_tbl.Id_PK ;
