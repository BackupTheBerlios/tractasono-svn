-- phpMyAdmin SQL Dump
-- version 2.8.2.4
-- http://www.phpmyadmin.net
-- 
-- Host: localhost
-- Erstellungszeit: 29. August 2006 um 16:33
-- Server Version: 4.1.20
-- PHP-Version: 4.3.9
-- 
-- Datenbank: `whatthef_tractasono`
-- 

-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `tbl_lookupcriteriaterms`
-- 

DROP TABLE IF EXISTS `tbl_lookupcriteriaterms`;
CREATE TABLE `tbl_lookupcriteriaterms` (
  `IDlookupcriteriaterms` int(11) NOT NULL auto_increment,
  `IDcriteria` int(11) default '0',
  `IDterms` int(11) default '0',
  PRIMARY KEY  (`IDlookupcriteriaterms`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci AUTO_INCREMENT=70 ;

-- 
-- Daten für Tabelle `tbl_lookupcriteriaterms`
-- 

INSERT INTO `tbl_lookupcriteriaterms` (`IDlookupcriteriaterms`, `IDcriteria`, `IDterms`) VALUES (1, 1, 3),
(2, 1, 6),
(3, 1, 7),
(4, 1, 8),
(5, 1, 9),
(6, 1, 10),
(7, 2, 3),
(8, 2, 6),
(9, 2, 8),
(10, 2, 9),
(11, 2, 10),
(12, 2, 7),
(15, 3, 6),
(14, 3, 3),
(16, 3, 7),
(17, 3, 8),
(18, 3, 9),
(19, 3, 10),
(20, 4, 3),
(21, 4, 6),
(22, 4, 7),
(23, 4, 8),
(24, 4, 9),
(25, 4, 10),
(26, 5, 3),
(27, 5, 6),
(28, 5, 7),
(29, 5, 8),
(30, 5, 9),
(31, 5, 10),
(32, 6, 3),
(33, 6, 4),
(34, 6, 5),
(35, 6, 9),
(36, 7, 3),
(37, 7, 4),
(38, 7, 5),
(39, 7, 9),
(40, 8, 3),
(41, 8, 4),
(42, 8, 5),
(43, 8, 9),
(44, 9, 3),
(45, 9, 4),
(46, 9, 5),
(47, 9, 9),
(48, 10, 1),
(49, 10, 2),
(50, 10, 11),
(51, 10, 12),
(52, 11, 1),
(53, 11, 2),
(54, 11, 11),
(55, 11, 12),
(56, 12, 1),
(57, 12, 2),
(58, 12, 11),
(59, 12, 12),
(60, 13, 1),
(61, 13, 2),
(62, 13, 11),
(63, 13, 12),
(64, 14, 3),
(65, 14, 6),
(66, 14, 7),
(67, 14, 8),
(68, 14, 9),
(69, 14, 10);
