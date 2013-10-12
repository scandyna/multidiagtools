DROP VIEW IF EXISTS VehicleType_Unit_view;
CREATE VIEW VehicleType_Unit_view AS
SELECT
 VehicleType_tbl.Type ,
 VehicleType_tbl.SubType ,
 VehicleType_tbl.SeriesNumber ,
 Unit_tbl.Coordinate ,
 Unit_tbl.Cabinet ,
 Unit_tbl.SchemaPosition ,
 VehicleType_Unit_tbl.Id_PK AS VehicleType_Unit_Id_PK ,
 VehicleType_Unit_tbl.VehicleType_Id_FK ,
 VehicleType_Unit_tbl.Unit_Id_FK
FROM VehicleType_Unit_tbl
 JOIN VehicleType_tbl
  ON VehicleType_Unit_tbl.VehicleType_Id_FK = VehicleType_tbl.Id_PK
 JOIN Unit_tbl
  ON VehicleType_Unit_tbl.Unit_Id_FK = Unit_tbl.Id_PK
