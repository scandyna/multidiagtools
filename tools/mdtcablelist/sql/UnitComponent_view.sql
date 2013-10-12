DROP VIEW IF EXISTS UnitComponent_view;
CREATE VIEW UnitComponent_view AS
SELECT
 UnitComposite.Id_PK AS Unit_Id_PK ,
 Unit_tbl.Id_PK AS UnitComponent_Id_PK ,
 Unit_tbl.Article_Id_FK ,
 Unit_tbl.Coordinate ,
 Unit_tbl.Cabinet ,
 Unit_tbl.SchemaPosition ,
 Article_tbl.Id_PK AS Article_Id_PK ,
 Article_tbl.ArticleCode ,
 Article_tbl.DesignationEN
FROM Unit_tbl AS UnitComposite
 JOIN Unit_tbl 
  ON Unit_tbl.Composite_Id_FK = UnitComposite.Id_PK 
 LEFT JOIN Article_tbl
  ON Unit_tbl.Article_Id_FK = Article_tbl.Id_PK
