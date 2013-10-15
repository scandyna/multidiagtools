DROP VIEW IF EXISTS UnitLink_view;
CREATE VIEW UnitLink_view AS
SELECT
 Link_tbl.Id_PK ,
 UCS.Unit_Id_FK AS StartUnit_Id_FK ,
 UCE.Unit_Id_FK AS EndUnit_Id_FK ,
 Link_tbl.LinkType_Code_FK ,
 Link_tbl.LinkDirection_Code_FK ,
 Link_tbl.ArticleConnectionStart_Id_FK ,
 Link_tbl.ArticleConnectionEnd_Id_FK ,
 Link_tbl.UnitConnectionStart_Id_FK ,
 Link_tbl.UnitConnectionEnd_Id_FK ,
 Link_tbl.SinceVersion ,
 Link_tbl.Modification ,
 Link_tbl.Identification ,
 LinkType_tbl.NameEN AS LinkTypeNameEN ,
 Link_tbl.Value ,
 LinkType_tbl.ValueUnit ,
 UCS.UnitConnectorName AS StartUnitConnectorName ,
 UCS.UnitContactName AS StartUnitContactName ,
 UCS.SchemaPage AS StartSchemaPage ,
 UCS.FunctionEN AS StartFunctionEN ,
 UCS.SignalName AS StartSignalName ,
 UCS.SwAddress AS StartSwAddress ,
 LinkDirection_tbl.PictureAscii AS LinkDirectionPictureAscii ,
 UCE.UnitConnectorName AS EndUnitConnectorName ,
 UCE.UnitContactName AS EndUnitContactName ,
 UCE.SchemaPage AS EndSchemaPage ,
 UCE.FunctionEN AS EndFunctionEN ,
 UCE.SignalName AS EndSignalName ,
 UCE.SwAddress AS EndSwAddress
FROM Link_tbl
JOIN UnitConnection_tbl UCS
 ON Link_tbl.UnitConnectionStart_Id_FK = UCS.Id_PK
JOIN UnitConnection_tbl UCE
 ON Link_tbl.UnitConnectionEnd_Id_FK = UCE.Id_PK
JOIN LinkType_tbl
 ON LinkType_tbl.Code_PK = Link_tbl.LinkType_Code_FK
JOIN LinkDirection_tbl
 ON LinkDirection_tbl.Code_PK = Link_tbl.LinkDirection_Code_FK