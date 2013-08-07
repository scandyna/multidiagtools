-- phpMyAdmin SQL Dump
-- version 3.4.5deb1
-- http://www.phpmyadmin.net
--
-- Base de données: `cablelist`
--

-- --------------------------------------------------------

--
-- Structure de la table `CableLink_tbl`
--

CREATE TABLE IF NOT EXISTS "CableLink_tbl" (
  "Id_PK" integer NOT NULL ,
  "CableNumber" varchar(50) NOT NULL,
  "CableType" varchar(50) DEFAULT NULL,
  "CableSection" float DEFAULT NULL,
  "Version" varchar(50) NOT NULL,
  "Modification" varchar(50) DEFAULT NULL,
  "ConnectionIdStart_FK" integer NOT NULL,
  "ConnectionIdEnd_FK" integer NOT NULL,
  "CableTorchId_FK" integer DEFAULT NULL,
  PRIMARY KEY ("Id_PK")
) ;

--
-- RELATIONS POUR LA TABLE `CableLink_tbl`:
--   `CableTorchId_FK`
--       `CableTorch_tbl` -> `Id_PK`
--   `ConnectionIdEnd_FK`
--       `Connection_tbl` -> `Id_PK`
--   `ConnectionIdStart_FK`
--       `Connection_tbl` -> `Id_PK`
--

-- --------------------------------------------------------

--
-- Structure de la table `CableTorch_tbl`
--

CREATE TABLE IF NOT EXISTS "CableTorch_tbl" (
  "Id_PK" integer NOT NULL ,
  "Number" varchar(50) DEFAULT NULL,
  "Name" varchar(50) DEFAULT NULL,
  "Type" varchar(50) DEFAULT NULL,
  PRIMARY KEY ("Id_PK")
) ;

-- --------------------------------------------------------

--
-- Structure de la table `Connection_tbl`
--

CREATE TABLE IF NOT EXISTS "Connection_tbl" (
  "Id_PK" integer NOT NULL ,
  "ConnectorName" varchar(50) NOT NULL,
  "ContactName" varchar(50) DEFAULT NULL,
  "IsAnEnd" tinyint(1) NOT NULL,
  "SchemaPage" varchar(50) NOT NULL,
  "UnitId_FK" integer NOT NULL,
  PRIMARY KEY ("Id_PK")
) ;

--
-- RELATIONS POUR LA TABLE `Connection_tbl`:
--   `UnitId_FK`
--       `Unit_tbl` -> `Id_PK`
--

-- --------------------------------------------------------

--
-- Structure de la table `Unit_tbl`
--

CREATE TABLE IF NOT EXISTS "Unit_tbl" (
  "Id_PK" integer NOT NULL ,
  "SchemaPosition" varchar(50) DEFAULT NULL,
  "Coordinate" varchar(50) DEFAULT NULL,
  "Cabinet" varchar(50) DEFAULT NULL,
  PRIMARY KEY ("Id_PK")
) ;

-- --------------------------------------------------------

--
-- Structure de la table `VehicleType_tbl`
--

CREATE TABLE IF NOT EXISTS "VehicleType_tbl" (
  "Type_PK" varchar(50) NOT NULL,
  "SubType_PK" varchar(50) NOT NULL,
  "SeriesNumber_PK" varchar(50) NOT NULL,
  PRIMARY KEY ("Type_PK","SubType_PK","SeriesNumber_PK")
);

-- --------------------------------------------------------

--
-- Structure de la table `VehicleType_Unit_tbl`
--

CREATE TABLE IF NOT EXISTS "VehicleType_Unit_tbl" (
  "VehicleType_FK" varchar(50) NOT NULL,
  "VehicleSubType_FK" varchar(50) NOT NULL,
  "VehicleSeriesNumber_FK" varchar(50) NOT NULL,
  "UnitId_FK" integer NOT NULL,
  PRIMARY KEY ("VehicleType_FK","VehicleSubType_FK","VehicleSeriesNumber_FK","UnitId_FK")
);

--
-- RELATIONS POUR LA TABLE `VehicleType_Unit_tbl`:
--   `UnitId_FK`
--       `Unit_tbl` -> `Id_PK`
--   `VehicleSeriesNumber_FK`
--       `VehicleType_tbl` -> `SeriesNumber_PK`
--   `VehicleSubType_FK`
--       `VehicleType_tbl` -> `SubType_PK`
--   `VehicleType_FK`
--       `VehicleType_tbl` -> `Type_PK`
--

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
