DROP VIEW IF EXISTS Unit_view;
CREATE VIEW Unit_view AS
SELECT
 Unit_tbl.Id_PK AS Unit_Id_PK , 
 Unit_tbl.Coordinate ,
 Unit_tbl.Cabinet ,
 Unit_tbl.SchemaPosition ,
 Article_tbl.Id_PK AS Article_Id_PK ,
 Article_tbl.ArticleCode ,
 Article_tbl.DesignationEN
FROM Unit_tbl
 LEFT JOIN Article_tbl
  ON Unit_tbl.Article_Id_FK = Article_tbl.Id_PK
