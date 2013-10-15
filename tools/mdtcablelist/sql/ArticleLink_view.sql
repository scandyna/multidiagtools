DROP VIEW IF EXISTS ArticleLink_view;
CREATE VIEW ArticleLink_view AS
SELECT
 ArticleLink_tbl.Id_PK ,
 ArticleLink_tbl.LinkType_Code_FK ,
 ArticleLink_tbl.LinkDirection_Code_FK ,
 ArticleLink_tbl.ArticleConnectionStart_Id_FK ,
 ArticleLink_tbl.ArticleConnectionEnd_Id_FK ,
 ArticleLink_tbl.Identification ,
 LinkType_tbl.NameEN AS LinkTypeNameEN ,
 ArticleLink_tbl.Value ,
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
FROM ArticleLink_tbl
JOIN ArticleConnection_tbl ACS
 ON ArticleLink_tbl.ArticleConnectionStart_Id_FK = ACS.Id_PK
JOIN ArticleConnection_tbl ACE
 ON ArticleLink_tbl.ArticleConnectionEnd_Id_FK = ACE.Id_PK
JOIN LinkType_tbl
 ON LinkType_tbl.Code_PK = ArticleLink_tbl.LinkType_Code_FK
JOIN LinkDirection_tbl
 ON LinkDirection_tbl.Code_PK = ArticleLink_tbl.LinkDirection_Code_FK