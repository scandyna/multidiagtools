DROP VIEW IF EXISTS Unit_VehicleType_view;
CREATE VIEW Unit_VehicleType_view AS
SELECT
 VehicleType_tbl.Type ,
 VehicleType_tbl.SubType ,
 VehicleType_tbl.SeriesNumber ,
 VehicleType_Unit_tbl.VehicleType_Id_FK ,
 VehicleType_Unit_tbl.Unit_Id_FK
FROM VehicleType_tbl
JOIN VehicleType_Unit_tbl
 ON VehicleType_tbl.Id_PK = VehicleType_Unit_tbl.VehicleType_Id_FK
