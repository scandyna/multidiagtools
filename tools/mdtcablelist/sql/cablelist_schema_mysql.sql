-- phpMyAdmin SQL Dump
-- version 3.4.5deb1
-- http://www.phpmyadmin.net
--
-- Client: localhost
-- Généré le : Mar 15 Octobre 2013 à 08:36
-- Version du serveur: 5.1.69
-- Version de PHP: 5.3.6-13ubuntu3.10

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Base de données: `cablelist`
--

-- --------------------------------------------------------

--
-- Doublure de structure pour la vue `ArticleComponentUsage_view`
--
CREATE TABLE IF NOT EXISTS `ArticleComponentUsage_view` (
`Component_Id_PK` int(11)
,`Article_Id_PK` int(11)
,`ArticleCode` varchar(50)
,`DesignationEN` varchar(150)
);
-- --------------------------------------------------------

--
-- Structure de la table `ArticleComponent_tbl`
--

CREATE TABLE IF NOT EXISTS `ArticleComponent_tbl` (
  `Composite_Id_FK` int(11) NOT NULL,
  `Component_Id_FK` int(11) NOT NULL,
  `ComponentQty` double NOT NULL DEFAULT '1',
  `ComponentQtyUnit` varchar(10) COLLATE utf8_unicode_ci NOT NULL DEFAULT 'pce',
  PRIMARY KEY (`Composite_Id_FK`,`Component_Id_FK`),
  KEY `Component_Id_FK` (`Component_Id_FK`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Doublure de structure pour la vue `ArticleComponent_view`
--
CREATE TABLE IF NOT EXISTS `ArticleComponent_view` (
`Article_Id_PK` int(11)
,`Component_Id_PK` int(11)
,`ComponentQty` double
,`ComponentQtyUnit` varchar(10)
,`ArticleCode` varchar(50)
,`DesignationEN` varchar(150)
);
-- --------------------------------------------------------

--
-- Structure de la table `ArticleConnection_tbl`
--

CREATE TABLE IF NOT EXISTS `ArticleConnection_tbl` (
  `Id_PK` int(11) NOT NULL AUTO_INCREMENT,
  `Article_Id_FK` int(11) NOT NULL,
  `ArticleConnectorName` varchar(30) DEFAULT NULL,
  `ArticleContactName` varchar(20) DEFAULT NULL,
  `IoType` varchar(50) DEFAULT NULL,
  `FunctionEN` varchar(100) DEFAULT NULL,
  PRIMARY KEY (`Id_PK`,`Article_Id_FK`),
  KEY `Article_Id_FK` (`Article_Id_FK`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=84 ;


--
-- Structure de la table `Article_tbl`
--

CREATE TABLE IF NOT EXISTS `Article_tbl` (
  `Id_PK` int(11) NOT NULL AUTO_INCREMENT,
  `ArticleCode` varchar(50) DEFAULT NULL,
  `DesignationEN` varchar(150) DEFAULT NULL,
  `Unit` varchar(10) NOT NULL DEFAULT 'pce',
  PRIMARY KEY (`Id_PK`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=12 ;


--
-- Structure de la table `LinkDirection_tbl`
--

CREATE TABLE IF NOT EXISTS `LinkDirection_tbl` (
  `Code_PK` varchar(10) NOT NULL,
  `PictureAscii` varchar(50) NOT NULL,
  `NameEN` varchar(50) NOT NULL,
  PRIMARY KEY (`Code_PK`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


--
-- Structure de la table `LinkType_tbl`
--

CREATE TABLE IF NOT EXISTS `LinkType_tbl` (
  `Code_PK` varchar(10) NOT NULL,
  `NameEN` varchar(50) NOT NULL,
  `ValueUnit` varchar(10) DEFAULT NULL,
  PRIMARY KEY (`Code_PK`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


--
-- Structure de la table `Link_tbl`
--

CREATE TABLE IF NOT EXISTS `Link_tbl` (
  `Id_PK` int(11) NOT NULL AUTO_INCREMENT,
  `ArticleConnectionStart_Id_FK` int(11) DEFAULT NULL,
  `ArticleConnectionEnd_Id_FK` int(11) DEFAULT NULL,
  `UnitConnectionStart_Id_FK` int(11) DEFAULT NULL,
  `UnitConnectionEnd_Id_FK` int(11) DEFAULT NULL,
  `SinceVersion` float DEFAULT NULL,
  `Modification` varchar(20) DEFAULT NULL,
  `Identification` varchar(50) DEFAULT NULL,
  `LinkDirection_Code_FK` varchar(10) NOT NULL,
  `LinkType_Code_FK` varchar(10) NOT NULL,
  `Value` double DEFAULT NULL,
  PRIMARY KEY (`Id_PK`),
  KEY `UnitConnectionEnd_Id_FK` (`UnitConnectionEnd_Id_FK`),
  KEY `Direction_Id_FK` (`LinkDirection_Code_FK`),
  KEY `Type_Id_FK` (`LinkType_Code_FK`),
  KEY `UnitConnectionStart_Id_FK` (`UnitConnectionStart_Id_FK`),
  KEY `ArticleConnectionStart_Id_FK` (`ArticleConnectionStart_Id_FK`),
  KEY `ArticleConnectionEnd_Id_FK` (`ArticleConnectionEnd_Id_FK`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=34 ;


--
-- Structure de la table `UnitConnection_tbl`
--

CREATE TABLE IF NOT EXISTS `UnitConnection_tbl` (
  `Id_PK` int(11) NOT NULL AUTO_INCREMENT,
  `Unit_Id_FK` int(11) NOT NULL,
  `ArticleConnection_Id_FK` int(11) DEFAULT NULL,
  `IsATestPoint` tinyint(1) DEFAULT NULL,
  `SchemaPage` varchar(20) DEFAULT NULL,
  `FunctionEN` varchar(100) DEFAULT NULL,
  `SignalName` varchar(50) DEFAULT NULL,
  `SwAddress` int(11) DEFAULT NULL,
  `UnitConnectorName` varchar(30) DEFAULT NULL,
  `UnitContactName` varchar(20) DEFAULT NULL,
  PRIMARY KEY (`Id_PK`),
  UNIQUE KEY `Unit_Id_FK_ArticleConnection_Id_FK` (`Unit_Id_FK`,`ArticleConnection_Id_FK`),
  KEY `ArticleConnection_Id_FK` (`ArticleConnection_Id_FK`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=43 ;


--
-- Structure de la table `Unit_tbl`
--

CREATE TABLE IF NOT EXISTS `Unit_tbl` (
  `Id_PK` int(11) NOT NULL AUTO_INCREMENT,
  `Composite_Id_FK` int(11) DEFAULT NULL,
  `Article_Id_FK` int(11) DEFAULT NULL,
  `Coordinate` varchar(10) DEFAULT NULL,
  `Cabinet` varchar(50) DEFAULT NULL,
  `SchemaPosition` varchar(20) DEFAULT NULL,
  PRIMARY KEY (`Id_PK`),
  KEY `Article_Id_FK` (`Article_Id_FK`),
  KEY `Composite_Id_FK` (`Composite_Id_FK`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=29 ;


--
-- Structure de la table `VehicleType_Link_tbl`
--

CREATE TABLE IF NOT EXISTS `VehicleType_Link_tbl` (
  `VehicleTypeStart_Id_FK` int(11) NOT NULL,
  `VehicleTypeEnd_Id_FK` int(11) NOT NULL,
  `Link_Id_FK` int(11) NOT NULL,
  PRIMARY KEY (`VehicleTypeStart_Id_FK`,`VehicleTypeEnd_Id_FK`,`Link_Id_FK`),
  KEY `VehicleTypeEnd_Id_FK` (`VehicleTypeEnd_Id_FK`),
  KEY `Link_Id_FK` (`Link_Id_FK`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;


--
-- Structure de la table `VehicleType_tbl`
--

CREATE TABLE IF NOT EXISTS `VehicleType_tbl` (
  `Id_PK` int(11) NOT NULL AUTO_INCREMENT,
  `Type` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  `SubType` varchar(50) COLLATE utf8_unicode_ci DEFAULT NULL,
  `SeriesNumber` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`Id_PK`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=14 ;


--
-- Structure de la table `VehicleType_Unit_tbl`
--

CREATE TABLE IF NOT EXISTS `VehicleType_Unit_tbl` (
  `Id_PK` int(11) NOT NULL AUTO_INCREMENT,
  `VehicleType_Id_FK` int(11) NOT NULL,
  `Unit_Id_FK` int(11) NOT NULL,
  PRIMARY KEY (`Id_PK`),
  KEY `VehicleType_Id_FK` (`VehicleType_Id_FK`),
  KEY `Unit_Id_FK` (`Unit_Id_FK`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=57 ;


--
-- Contraintes pour la table `ArticleComponent_tbl`
--
ALTER TABLE `ArticleComponent_tbl`
  ADD CONSTRAINT `ArticleComponent_tbl_ibfk_1` FOREIGN KEY (`Composite_Id_FK`) REFERENCES `Article_tbl` (`Id_PK`) ON UPDATE CASCADE,
  ADD CONSTRAINT `ArticleComponent_tbl_ibfk_2` FOREIGN KEY (`Component_Id_FK`) REFERENCES `Article_tbl` (`Id_PK`) ON UPDATE CASCADE;

--
-- Contraintes pour la table `ArticleConnection_tbl`
--
ALTER TABLE `ArticleConnection_tbl`
  ADD CONSTRAINT `ArticleConnection_tbl_ibfk_1` FOREIGN KEY (`Article_Id_FK`) REFERENCES `Article_tbl` (`Id_PK`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Contraintes pour la table `Link_tbl`
--
ALTER TABLE `Link_tbl`
  ADD CONSTRAINT `Link_tbl_ibfk_10` FOREIGN KEY (`ArticleConnectionStart_Id_FK`) REFERENCES `ArticleConnection_tbl` (`Id_PK`) ON UPDATE CASCADE,
  ADD CONSTRAINT `Link_tbl_ibfk_11` FOREIGN KEY (`ArticleConnectionEnd_Id_FK`) REFERENCES `ArticleConnection_tbl` (`Id_PK`) ON UPDATE CASCADE,
  ADD CONSTRAINT `Link_tbl_ibfk_12` FOREIGN KEY (`LinkDirection_Code_FK`) REFERENCES `LinkDirection_tbl` (`Code_PK`) ON UPDATE CASCADE,
  ADD CONSTRAINT `Link_tbl_ibfk_13` FOREIGN KEY (`LinkType_Code_FK`) REFERENCES `LinkType_tbl` (`Code_PK`) ON UPDATE CASCADE,
  ADD CONSTRAINT `Link_tbl_ibfk_5` FOREIGN KEY (`UnitConnectionStart_Id_FK`) REFERENCES `UnitConnection_tbl` (`Id_PK`) ON UPDATE CASCADE,
  ADD CONSTRAINT `Link_tbl_ibfk_6` FOREIGN KEY (`UnitConnectionEnd_Id_FK`) REFERENCES `UnitConnection_tbl` (`Id_PK`) ON UPDATE CASCADE;

--
-- Contraintes pour la table `UnitConnection_tbl`
--
ALTER TABLE `UnitConnection_tbl`
  ADD CONSTRAINT `UnitConnection_tbl_ibfk_2` FOREIGN KEY (`ArticleConnection_Id_FK`) REFERENCES `ArticleConnection_tbl` (`Id_PK`) ON UPDATE CASCADE,
  ADD CONSTRAINT `UnitConnection_tbl_ibfk_1` FOREIGN KEY (`Unit_Id_FK`) REFERENCES `Unit_tbl` (`Id_PK`) ON UPDATE CASCADE;

--
-- Contraintes pour la table `Unit_tbl`
--
ALTER TABLE `Unit_tbl`
  ADD CONSTRAINT `Unit_tbl_ibfk_2` FOREIGN KEY (`Article_Id_FK`) REFERENCES `Article_tbl` (`Id_PK`) ON UPDATE CASCADE,
  ADD CONSTRAINT `Unit_tbl_ibfk_3` FOREIGN KEY (`Composite_Id_FK`) REFERENCES `Unit_tbl` (`Id_PK`) ON UPDATE CASCADE;

--
-- Contraintes pour la table `VehicleType_Link_tbl`
--
ALTER TABLE `VehicleType_Link_tbl`
  ADD CONSTRAINT `VehicleType_Link_tbl_ibfk_1` FOREIGN KEY (`VehicleTypeStart_Id_FK`) REFERENCES `VehicleType_tbl` (`Id_PK`) ON UPDATE CASCADE,
  ADD CONSTRAINT `VehicleType_Link_tbl_ibfk_2` FOREIGN KEY (`VehicleTypeEnd_Id_FK`) REFERENCES `VehicleType_tbl` (`Id_PK`) ON UPDATE CASCADE,
  ADD CONSTRAINT `VehicleType_Link_tbl_ibfk_3` FOREIGN KEY (`Link_Id_FK`) REFERENCES `Link_tbl` (`Id_PK`) ON UPDATE CASCADE;

--
-- Contraintes pour la table `VehicleType_Unit_tbl`
--
ALTER TABLE `VehicleType_Unit_tbl`
  ADD CONSTRAINT `VehicleType_Unit_tbl_ibfk_1` FOREIGN KEY (`VehicleType_Id_FK`) REFERENCES `VehicleType_tbl` (`Id_PK`) ON UPDATE CASCADE,
  ADD CONSTRAINT `VehicleType_Unit_tbl_ibfk_2` FOREIGN KEY (`Unit_Id_FK`) REFERENCES `Unit_tbl` (`Id_PK`) ON UPDATE CASCADE;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
