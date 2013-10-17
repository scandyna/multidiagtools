DROP VIEW IF EXISTS TerminalBlockLink_view;
CREATE VIEW TerminalBlockLink_view AS
SELECT 
 Link_tbl.UnitConnectionStart_Id_FK , 
 Link_tbl.UnitConnectionEnd_Id_FK , 
 US.Id_PK AS UnitStart_Id_PK ,
 LinkType_tbl.NameEN AS LinkTypeNameEN,
 CS.UnitConnectorName AS StartUnitConnectorName ,
 CS.UnitContactName AS StartUnitContactName ,
 LinkDirection_tbl.PictureAscii AS LinkDirectionPictureAscii ,
 LinkDirection_tbl.NameEN AS LinkDirectionNameEN,
 UE.Id_PK AS UnitEnd_Id_PK ,
 CE.UnitConnectorName AS EndUnitConnectorName ,
 CE.UnitContactName AS EndUnitContactName 
FROM Link_tbl
 JOIN UnitConnection_tbl CS
  ON CS.Id_PK = Link_tbl.UnitConnectionStart_Id_FK
 JOIN UnitConnection_tbl CE
  ON CE.Id_PK = Link_tbl.UnitConnectionEnd_Id_FK
 JOIN Unit_tbl US
  ON US.Id_PK = CS.Unit_Id_FK
 JOIN Unit_tbl UE
  ON UE.Id_PK = CE.Unit_Id_FK
 JOIN LinkDirection_tbl
  ON Link_tbl.LinkDirection_Code_FK = LinkDirection_tbl.Code_PK
 JOIN LinkType_tbl
  ON Link_tbl.LinkType_Code_FK = LinkType_tbl.Code_PK
WHERE
 Link_tbl.LinkType_Code_FK = 'TERMRANGEBRIDGE'
OR 
 Link_tbl.LinkType_Code_FK = 'TERMBRIDGE'
