DROP VIEW IF EXISTS LinkList_view;
CREATE VIEW LinkList_view AS
SELECT 
 Link_tbl.UnitConnectionStart_Id_FK , 
 Link_tbl.UnitConnectionEnd_Id_FK , 
 VS.Type AS StartVehicleType ,
 VS.SubType AS StartVehicleSubType ,
 VS.SeriesNumber AS StartVehicleSerie,
 Link_tbl.SinceVersion , 
 Link_tbl.Modification , 
 Link_tbl.Identification ,
 Link_tbl.LinkDirection_Code_FK ,
 Link_tbl.LinkType_Code_FK ,
 US.Id_PK AS UnitStart_Id_PK ,
 US.SchemaPosition AS StartSchemaPosition, 
 US.Cabinet AS StartCabinet, 
 US.Coordinate AS StartCoordinate , 
 CS.UnitConnectorName AS StartUnitConnectorName ,
 CS.UnitContactName AS StartUnitContactName ,
 VE.Type AS EndVehicleType ,
 VE.SubType AS EndVehicleSubType ,
 VE.SeriesNumber AS EndVehicleSerie,
 UE.Id_PK AS UnitEnd_Id_PK ,
 UE.SchemaPosition AS EndSchemaPosition, 
 UE.Cabinet AS EndCabinet, 
 UE.Coordinate AS EndCoordinate , 
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
 JOIN VehicleType_Link_tbl
  ON VehicleType_Link_tbl.UnitConnectionStart_Id_FK = Link_tbl.UnitConnectionStart_Id_FK
  AND VehicleType_Link_tbl.UnitConnectionEnd_Id_FK = Link_tbl.UnitConnectionEnd_Id_FK
 JOIN VehicleType_tbl VS
  ON VS.Id_PK = VehicleType_Link_tbl.VehicleTypeStart_Id_FK
 JOIN VehicleType_tbl VE
  ON VE.Id_PK = VehicleType_Link_tbl.VehicleTypeEnd_Id_FK
