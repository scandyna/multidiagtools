DROP VIEW IF EXISTS ArticleComponent_view;
CREATE VIEW ArticleComponent_view AS
SELECT
 ArticleComponent_tbl.Composite_Id_FK AS Article_Id_PK ,
 ArticleComponent_tbl.Component_Id_FK AS Component_Id_PK ,
 ArticleComponent_tbl.ComponentQty ,
 ArticleComponent_tbl.ComponentQtyUnit ,
 Article_tbl.ArticleCode ,
 Article_tbl.DesignationEN
FROM ArticleComponent_tbl
 JOIN Article_tbl
  ON Article_tbl.Id_PK = ArticleComponent_tbl.Component_Id_FK
 
