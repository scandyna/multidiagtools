DROP VIEW IF EXISTS ArticleLink_view;
CREATE VIEW ArticleLink_view AS
SELECT
 Link_tbl.Id_PK ,
 Link_tbl.LinkType_Code_FK ,
 Link_tbl.LinkDirection_Code_FK ,
 Link_tbl.ArticleConnectionStart_Id_FK ,
 Link_tbl.ArticleConnectionEnd_Id_FK ,
 Link_tbl.UnitConnectionStart_Id_FK ,
 Link_tbl.UnitConnectionEnd_Id_FK ,
 Link_tbl.SinceVersion ,
 Link_tbl.Modification ,
 LinkType_tbl.NameEN AS LinkTypeNameEN ,
 Link_tbl.Value ,
 LinkType_tbl.ValueUnit ,
 ACS.Article_Id_FK AS StartArticle_Id_FK ,
 ACS.ArticleConnectorName AS StartArticleConnectorName ,
 ACS.ArticleContactName AS StartArticleContactName ,
 ACS.IoType AS StartIoType ,
 ACS.FunctionEN AS StartFunctionEN ,
 LinkDirection_tbl.PictureAscii AS LinkDirectionPictureAscii ,
 ACE.Article_Id_FK AS EndArticle_Id_FK ,
 ACE.ArticleConnectorName AS EndArticleConnectorName ,
 ACE.ArticleContactName AS EndArticleContactName ,
 ACE.IoType AS EndIoType ,
 ACE.FunctionEN AS EndFunctionEN
FROM Link_tbl
JOIN ArticleConnection_tbl ACS
 ON Link_tbl.ArticleConnectionStart_Id_FK = ACS.Id_PK
JOIN ArticleConnection_tbl ACE
 ON Link_tbl.ArticleConnectionEnd_Id_FK = ACE.Id_PK
JOIN LinkType_tbl
 ON LinkType_tbl.Code_PK = Link_tbl.LinkType_Code_FK
JOIN LinkDirection_tbl
 ON LinkDirection_tbl.Code_PK = Link_tbl.LinkDirection_Code_FK