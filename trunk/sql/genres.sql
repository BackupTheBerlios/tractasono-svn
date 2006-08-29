-- phpMyAdmin SQL Dump
-- version 2.8.2.4
-- http://www.phpmyadmin.net
-- 
-- Host: localhost
-- Erstellungszeit: 29. August 2006 um 17:16
-- Server Version: 4.1.20
-- PHP-Version: 4.3.9
-- 
-- Datenbank: `whatthef_tractasono`
-- 

-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `tbl_genre`
-- 

DROP TABLE IF EXISTS `tbl_genre`;
CREATE TABLE `tbl_genre` (
  `IDgenre` int(11) NOT NULL auto_increment,
  `genrename` varchar(32) collate latin1_german1_ci NOT NULL default '',
  PRIMARY KEY  (`IDgenre`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci AUTO_INCREMENT=81 ;

-- 
-- Daten für Tabelle `tbl_genre`
-- 

INSERT INTO `tbl_genre` (`IDgenre`, `genrename`) VALUES (1, 'Blues'),
(2, 'Classic Rock'),
(3, 'Country'),
(4, 'Dance'),
(5, 'Disco'),
(6, 'Funk'),
(7, 'Grunge'),
(8, 'Hip-Hop'),
(9, 'Jazz'),
(10, 'Metal'),
(11, 'New Age'),
(12, 'Oldies'),
(13, 'Other'),
(14, 'Pop'),
(15, 'R&B'),
(16, 'Rap'),
(17, 'Reggae'),
(18, 'Rock'),
(19, 'Techno'),
(20, 'Industrial'),
(21, 'Alternative'),
(22, 'Ska'),
(23, 'Death Metal'),
(24, 'Pranks'),
(25, 'Soundtrack'),
(26, 'Euro-Techno'),
(27, 'Ambient'),
(28, 'Trip-Hop'),
(29, 'Vocal'),
(30, 'Jazz+Funk'),
(31, 'Fusion'),
(32, 'Trance'),
(33, 'Classical'),
(34, 'Instrumental'),
(35, 'Acid'),
(36, 'House'),
(37, 'Game'),
(38, 'Sound Clip'),
(39, 'Gospel'),
(40, 'Noise'),
(41, 'AlternRock'),
(42, 'Bass'),
(43, 'Soul'),
(44, 'Punk'),
(45, 'Space'),
(46, 'Meditative'),
(47, 'Instrumental Pop'),
(48, 'Instrumental Rock'),
(49, 'Ethnic'),
(50, 'Gothic'),
(51, 'Darkwave'),
(52, 'Techno-Industrial'),
(53, 'Electronic'),
(54, 'Pop-Folk'),
(55, 'Eurodance'),
(56, 'Dream'),
(57, 'Southern Rock'),
(58, 'Comedy'),
(59, 'Cult'),
(60, 'Gangsta'),
(61, 'Top 40'),
(62, 'Christian Rap'),
(63, 'Pop/Funk'),
(64, 'Jungle'),
(65, 'Native American'),
(66, 'Cabaret'),
(67, 'New Wave'),
(68, 'Psychadelic'),
(69, 'Rave'),
(70, 'Showtunes'),
(71, 'Trailer'),
(72, 'Lo-Fi'),
(73, 'Tribal'),
(74, 'Acid Punk'),
(75, 'Acid Jazz'),
(76, 'Polka'),
(77, 'Retro'),
(78, 'Musical'),
(79, 'Rock & Roll'),
(80, 'Hard Rock');
