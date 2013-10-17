DROP VIEW IF EXISTS UnitLink_view;
CREATE VIEW UnitLink_view AS
SELECT
 Link_tbl.Identification ,
 US.SchemaPosition AS StartSchemaPosition ,
 UCS.UnitConnectorName AS StartUnitConnectorName ,
 UCS.UnitContactName AS StartUnitContactName ,
 UE.SchemaPosition AS EndSchemaPosition ,
 UCE.UnitConnectorName AS EndUnitConnectorName ,
 UCE.UnitContactName AS EndUnitContactName ,
 Link_tbl.SinceVersion ,
 Link_tbl.Modification ,
 LinkType_tbl.NameEN AS LinkTypeNameEN ,
 Link_tbl.Value ,
 LinkType_tbl.ValueUnit ,
 LinkDirection_tbl.PictureAscii AS LinkDirectionPictureAscii ,
 UCS.SchemaPage AS StartSchemaPage ,
 UCS.FunctionEN AS StartFunctionEN ,
 UCS.SignalName AS StartSignalName ,
 UCS.SwAddress AS StartSwAddress ,
 UCE.SchemaPage AS EndSchemaPage ,
 UCE.FunctionEN AS EndFunctionEN ,
 UCE.SignalName AS EndSignalName ,
 UCE.SwAddress AS EndSwAddress ,
 Link_tbl.UnitConnectionStart_Id_FK ,
 Link_tbl.UnitConnectionEnd_Id_FK ,
 Link_tbl.ArticleLink_Id_FK ,
 UCS.Unit_Id_FK AS StartUnit_Id_FK ,
 UCE.Unit_Id_FK AS EndUnit_Id_FK ,
 Link_tbl.LinkType_Code_FK ,
 Link_tbl.LinkDirection_Code_FK ,
 ArticleLink_tbl.ArticleConnectionStart_Id_FK ,
 ArticleLink_tbl.ArticleConnectionEnd_Id_FK
FROM Link_tbl
JOIN UnitConnection_tbl UCS
 ON Link_tbl.UnitConnectionStart_Id_FK = UCS.Id_PK
JOIN UnitConnection_tbl UCE
 ON Link_tbl.UnitConnectionEnd_Id_FK = UCE.Id_PK
JOIN Unit_tbl US
 ON US.Id_PK = UCS.Unit_Id_FK
JOIN Unit_tbl UE
 ON UE.Id_PK = UCE.Unit_Id_FK
JOIN LinkType_tbl
 ON LinkType_tbl.Code_PK = Link_tbl.LinkType_Code_FK
JOIN LinkDirection_tbl
 ON LinkDirection_tbl.Code_PK = Link_tbl.LinkDirection_Code_FK
LEFT JOIN ArticleLink_tbl
 ON ArticleLink_tbl.Id_PK = Link_tbl.ArticleLink_Id_FK