-- MySQL Administrator dump 1.4
--
-- ------------------------------------------------------
-- Server version	5.0.38-Ubuntu_0ubuntu1-log


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;


--
-- Create schema tractasono
--

CREATE DATABASE IF NOT EXISTS tractasono;
USE tractasono;

--
-- Temporary table structure for view `tractasono`.`view_song`
--
DROP TABLE IF EXISTS `tractasono`.`view_song`;
DROP VIEW IF EXISTS `tractasono`.`view_song`;
CREATE TABLE `tractasono`.`view_song` (
  `IDtrack` int(11),
  `trackname` varchar(32),
  `tracknumber` int(11),
  `trackdiscnumber` int(11),
  `IDgenre` int(11),
  `genrename` varchar(32),
  `IDartist` int(11),
  `artistname` varchar(32),
  `IDalbum` int(11),
  `albumname` varchar(32)
);

--
-- Temporary table structure for view `tractasono`.`view_song_short`
--
DROP TABLE IF EXISTS `tractasono`.`view_song_short`;
DROP VIEW IF EXISTS `tractasono`.`view_song_short`;
CREATE TABLE `tractasono`.`view_song_short` (
  `tracknumber` int(11),
  `artistname` varchar(32),
  `trackname` varchar(32),
  `albumname` varchar(32),
  `genrename` varchar(32)
);

--
-- Definition of table `tractasono`.`tbl_album`
--

DROP TABLE IF EXISTS `tractasono`.`tbl_album`;
CREATE TABLE  `tractasono`.`tbl_album` (
  `IDalbum` int(11) NOT NULL auto_increment,
  `albumname` varchar(32) NOT NULL default '',
  `albumyear` date default '1900-01-01',
  `albumaddetat` datetime NOT NULL default '1900-01-01 00:00:00',
  `albumvaluation` int(11) default '0',
  `albumcover` varchar(32) default '',
  PRIMARY KEY  (`IDalbum`)
) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=latin1;

--
-- Dumping data for table `tractasono`.`tbl_album`
--

/*!40000 ALTER TABLE `tbl_album` DISABLE KEYS */;
LOCK TABLES `tbl_album` WRITE;
INSERT INTO `tractasono`.`tbl_album` VALUES  (1,'Painkiller','1900-01-01','1900-01-01 00:00:00',0,''),
 (2,'The Blackening','1900-01-01','1900-01-01 00:00:00',0,''),
 (3,'Glory to the brave','1900-01-01','1900-01-01 00:00:00',0,''),
 (4,'Legacy of Kings','1900-01-01','1900-01-01 00:00:00',0,'');
UNLOCK TABLES;
/*!40000 ALTER TABLE `tbl_album` ENABLE KEYS */;


--
-- Definition of table `tractasono`.`tbl_artist`
--

DROP TABLE IF EXISTS `tractasono`.`tbl_artist`;
CREATE TABLE  `tractasono`.`tbl_artist` (
  `IDartist` int(11) NOT NULL auto_increment,
  `artistname` varchar(32) NOT NULL default '',
  PRIMARY KEY  (`IDartist`)
) ENGINE=MyISAM AUTO_INCREMENT=7 DEFAULT CHARSET=latin1;

--
-- Dumping data for table `tractasono`.`tbl_artist`
--

/*!40000 ALTER TABLE `tbl_artist` DISABLE KEYS */;
LOCK TABLES `tbl_artist` WRITE;
INSERT INTO `tractasono`.`tbl_artist` VALUES  (1,'HammerFall'),
 (2,'Judas Priest'),
 (3,'Machine Head'),
 (4,'In Flames'),
 (5,'Sonata Arctica'),
 (6,'Thunderstone');
UNLOCK TABLES;
/*!40000 ALTER TABLE `tbl_artist` ENABLE KEYS */;


--
-- Definition of table `tractasono`.`tbl_genre`
--

DROP TABLE IF EXISTS `tractasono`.`tbl_genre`;
CREATE TABLE  `tractasono`.`tbl_genre` (
  `IDgenre` int(11) NOT NULL auto_increment,
  `genrename` varchar(32) NOT NULL default '',
  PRIMARY KEY  (`IDgenre`)
) ENGINE=MyISAM AUTO_INCREMENT=81 DEFAULT CHARSET=latin1;

--
-- Dumping data for table `tractasono`.`tbl_genre`
--

/*!40000 ALTER TABLE `tbl_genre` DISABLE KEYS */;
LOCK TABLES `tbl_genre` WRITE;
INSERT INTO `tractasono`.`tbl_genre` VALUES  (1,'Blues'),
 (2,'Classic Rock'),
 (3,'Country'),
 (4,'Dance'),
 (5,'Disco'),
 (6,'Funk'),
 (7,'Grunge'),
 (8,'Hip-Hop'),
 (9,'Jazz'),
 (10,'Metal'),
 (11,'New Age'),
 (12,'Oldies'),
 (13,'Other'),
 (14,'Pop'),
 (15,'R&B'),
 (16,'Rap'),
 (17,'Reggae'),
 (18,'Rock'),
 (19,'Techno'),
 (20,'Industrial'),
 (21,'Alternative'),
 (22,'Ska'),
 (23,'Death Metal'),
 (24,'Pranks'),
 (25,'Soundtrack'),
 (26,'Euro-Techno'),
 (27,'Ambient'),
 (28,'Trip-Hop'),
 (29,'Vocal'),
 (30,'Jazz+Funk'),
 (31,'Fusion'),
 (32,'Trance'),
 (33,'Classical'),
 (34,'Instrumental'),
 (35,'Acid'),
 (36,'House'),
 (37,'Game'),
 (38,'Sound Clip'),
 (39,'Gospel'),
 (40,'Noise'),
 (41,'AlternRock'),
 (42,'Bass'),
 (43,'Soul'),
 (44,'Punk'),
 (45,'Space'),
 (46,'Meditative'),
 (47,'Instrumental Pop'),
 (48,'Instrumental Rock'),
 (49,'Ethnic'),
 (50,'Gothic'),
 (51,'Darkwave'),
 (52,'Techno-Industrial'),
 (53,'Electronic'),
 (54,'Pop-Folk'),
 (55,'Eurodance'),
 (56,'Dream'),
 (57,'Southern Rock'),
 (58,'Comedy');
INSERT INTO `tractasono`.`tbl_genre` VALUES  (59,'Cult'),
 (60,'Gangsta'),
 (61,'Top 40'),
 (62,'Christian Rap'),
 (63,'Pop/Funk'),
 (64,'Jungle'),
 (65,'Native American'),
 (66,'Cabaret'),
 (67,'New Wave'),
 (68,'Psychadelic'),
 (69,'Rave'),
 (70,'Showtunes'),
 (71,'Trailer'),
 (72,'Lo-Fi'),
 (73,'Tribal'),
 (74,'Acid Punk'),
 (75,'Acid Jazz'),
 (76,'Polka'),
 (77,'Retro'),
 (78,'Musical'),
 (79,'Rock & Roll'),
 (80,'Hard Rock');
UNLOCK TABLES;
/*!40000 ALTER TABLE `tbl_genre` ENABLE KEYS */;


--
-- Definition of table `tractasono`.`tbl_list`
--

DROP TABLE IF EXISTS `tractasono`.`tbl_list`;
CREATE TABLE  `tractasono`.`tbl_list` (
  `IDlist` int(11) NOT NULL auto_increment,
  `listname` varchar(32) NOT NULL default '',
  `listissearchlist` tinyint(4) NOT NULL default '0',
  `listisand` varchar(32) default '',
  `listlimmitto` varchar(32) default '',
  `listsearchsql` varchar(32) default '',
  PRIMARY KEY  (`IDlist`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Dumping data for table `tractasono`.`tbl_list`
--

/*!40000 ALTER TABLE `tbl_list` DISABLE KEYS */;
LOCK TABLES `tbl_list` WRITE;
UNLOCK TABLES;
/*!40000 ALTER TABLE `tbl_list` ENABLE KEYS */;


--
-- Definition of table `tractasono`.`tbl_listcriteria`
--

DROP TABLE IF EXISTS `tractasono`.`tbl_listcriteria`;
CREATE TABLE  `tractasono`.`tbl_listcriteria` (
  `IDcriteria` int(11) NOT NULL auto_increment,
  `criterianame` varchar(32) NOT NULL default '',
  `criteriadbfield` varchar(32) NOT NULL default '',
  `criteriadbtable` varchar(32) NOT NULL default '',
  PRIMARY KEY  (`IDcriteria`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Dumping data for table `tractasono`.`tbl_listcriteria`
--

/*!40000 ALTER TABLE `tbl_listcriteria` DISABLE KEYS */;
LOCK TABLES `tbl_listcriteria` WRITE;
UNLOCK TABLES;
/*!40000 ALTER TABLE `tbl_listcriteria` ENABLE KEYS */;


--
-- Definition of table `tractasono`.`tbl_listorder`
--

DROP TABLE IF EXISTS `tractasono`.`tbl_listorder`;
CREATE TABLE  `tractasono`.`tbl_listorder` (
  `IDlistorder` int(11) NOT NULL auto_increment,
  `IDlist` int(11) default '0',
  `IDcriteria` int(11) default '0',
  `listorderisdesc` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`IDlistorder`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Dumping data for table `tractasono`.`tbl_listorder`
--

/*!40000 ALTER TABLE `tbl_listorder` DISABLE KEYS */;
LOCK TABLES `tbl_listorder` WRITE;
UNLOCK TABLES;
/*!40000 ALTER TABLE `tbl_listorder` ENABLE KEYS */;


--
-- Definition of table `tractasono`.`tbl_listterms`
--

DROP TABLE IF EXISTS `tractasono`.`tbl_listterms`;
CREATE TABLE  `tractasono`.`tbl_listterms` (
  `IDterms` int(11) NOT NULL auto_increment,
  `termname` varchar(32) NOT NULL default '',
  `termmeaning` varchar(32) NOT NULL default '',
  PRIMARY KEY  (`IDterms`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Dumping data for table `tractasono`.`tbl_listterms`
--

/*!40000 ALTER TABLE `tbl_listterms` DISABLE KEYS */;
LOCK TABLES `tbl_listterms` WRITE;
UNLOCK TABLES;
/*!40000 ALTER TABLE `tbl_listterms` ENABLE KEYS */;


--
-- Definition of table `tractasono`.`tbl_lookupcriteriaterms`
--

DROP TABLE IF EXISTS `tractasono`.`tbl_lookupcriteriaterms`;
CREATE TABLE  `tractasono`.`tbl_lookupcriteriaterms` (
  `IDlookupcriteriaterms` int(11) NOT NULL auto_increment,
  `IDcriteria` int(11) default '0',
  `IDterms` int(11) default '0',
  PRIMARY KEY  (`IDlookupcriteriaterms`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Dumping data for table `tractasono`.`tbl_lookupcriteriaterms`
--

/*!40000 ALTER TABLE `tbl_lookupcriteriaterms` DISABLE KEYS */;
LOCK TABLES `tbl_lookupcriteriaterms` WRITE;
UNLOCK TABLES;
/*!40000 ALTER TABLE `tbl_lookupcriteriaterms` ENABLE KEYS */;


--
-- Definition of table `tractasono`.`tbl_playlist`
--

DROP TABLE IF EXISTS `tractasono`.`tbl_playlist`;
CREATE TABLE  `tractasono`.`tbl_playlist` (
  `IDplaylist` int(11) NOT NULL auto_increment,
  `IDlist` int(11) default '0',
  `IDtrack` int(11) default '0',
  PRIMARY KEY  (`IDplaylist`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Dumping data for table `tractasono`.`tbl_playlist`
--

/*!40000 ALTER TABLE `tbl_playlist` DISABLE KEYS */;
LOCK TABLES `tbl_playlist` WRITE;
UNLOCK TABLES;
/*!40000 ALTER TABLE `tbl_playlist` ENABLE KEYS */;


--
-- Definition of table `tractasono`.`tbl_searchlistentry`
--

DROP TABLE IF EXISTS `tractasono`.`tbl_searchlistentry`;
CREATE TABLE  `tractasono`.`tbl_searchlistentry` (
  `IDsearchlistentry` int(11) NOT NULL auto_increment,
  `IDlist` int(11) default '0',
  `IDcriteria` int(11) default '0',
  `IDterms` int(11) default '0',
  `searchlistentryname` varchar(32) default '',
  PRIMARY KEY  (`IDsearchlistentry`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Dumping data for table `tractasono`.`tbl_searchlistentry`
--

/*!40000 ALTER TABLE `tbl_searchlistentry` DISABLE KEYS */;
LOCK TABLES `tbl_searchlistentry` WRITE;
UNLOCK TABLES;
/*!40000 ALTER TABLE `tbl_searchlistentry` ENABLE KEYS */;


--
-- Definition of table `tractasono`.`tbl_settings`
--

DROP TABLE IF EXISTS `tractasono`.`tbl_settings`;
CREATE TABLE  `tractasono`.`tbl_settings` (
  `IDsettings` int(11) NOT NULL auto_increment,
  `settingsgroup` varchar(25) NOT NULL,
  `settingskey` varchar(25) NOT NULL,
  `settingsboolean` tinyint(1) NOT NULL,
  `settingsstring` varchar(100) NOT NULL,
  `settingsinteger` int(11) NOT NULL,
  PRIMARY KEY  (`IDsettings`)
) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=latin1;

--
-- Dumping data for table `tractasono`.`tbl_settings`
--

/*!40000 ALTER TABLE `tbl_settings` DISABLE KEYS */;
LOCK TABLES `tbl_settings` WRITE;
INSERT INTO `tractasono`.`tbl_settings` VALUES  (2,'library','path',0,'/opt/music/',0),
 (3,'library','name',0,'Die geilschti Musig!',0),
 (4,'library','filecount',0,'',122);
UNLOCK TABLES;
/*!40000 ALTER TABLE `tbl_settings` ENABLE KEYS */;


--
-- Definition of table `tractasono`.`tbl_track`
--

DROP TABLE IF EXISTS `tractasono`.`tbl_track`;
CREATE TABLE  `tractasono`.`tbl_track` (
  `IDtrack` int(11) NOT NULL auto_increment,
  `IDgenre` int(11) default '0',
  `IDartist` int(11) default '0',
  `IDalbum` int(11) default '0',
  `trackname` varchar(32) NOT NULL default '',
  `trackpath` varchar(32) NOT NULL default '',
  `tracknumber` int(11) default '0',
  `trackdiscnumber` int(11) default '0',
  `trackcomposer` varchar(32) default '',
  `trackyear` date default '1900-01-01',
  `trackaddetat` datetime default '1900-01-01 00:00:00',
  `trackplaycounter` int(11) default '0',
  `trackvaluation` int(11) default '0',
  `trackcomment` varchar(32) default '',
  `trackencodetby` varchar(32) default '',
  `trackissampler` tinyint(4) default '0',
  PRIMARY KEY  (`IDtrack`)
) ENGINE=MyISAM AUTO_INCREMENT=23 DEFAULT CHARSET=latin1;

--
-- Dumping data for table `tractasono`.`tbl_track`
--

/*!40000 ALTER TABLE `tbl_track` DISABLE KEYS */;
LOCK TABLES `tbl_track` WRITE;
INSERT INTO `tractasono`.`tbl_track` VALUES  (1,10,1,4,'Heeding the call','',1,1,'','1900-01-01','1900-01-01 00:00:00',0,0,'','',0),
 (2,10,1,4,'The fallen one','',10,1,'','1900-01-01','1900-01-01 00:00:00',0,0,'','',0),
 (3,10,1,4,'Warriors of faith','',9,1,'','1900-01-01','1900-01-01 00:00:00',0,0,'','',0),
 (4,10,1,4,'Stronger than all','',8,1,'','1900-01-01','1900-01-01 00:00:00',0,0,'','',0),
 (5,10,1,4,'Back to back','',7,1,'','1900-01-01','1900-01-01 00:00:00',0,0,'','',0),
 (6,10,1,4,'At the end of the rainbow','',6,1,'','1900-01-01','1900-01-01 00:00:00',0,0,'','',0),
 (7,10,1,4,'Remember yesterday','',5,1,'','1900-01-01','1900-01-01 00:00:00',0,0,'','',0),
 (8,10,1,4,'Dreamland','',4,1,'','1900-01-01','1900-01-01 00:00:00',0,0,'','',0),
 (9,10,1,4,'Let the hammer fall','',3,1,'','1900-01-01','1900-01-01 00:00:00',0,0,'','',0),
 (10,10,1,4,'Legacy of kings','',2,1,'','1900-01-01','1900-01-01 00:00:00',0,0,'','',0),
 (11,10,2,1,'Painkiller','',1,1,'','1900-01-01','1900-01-01 00:00:00',0,0,'','',0),
 (12,10,2,1,'Leather Rebel (Live)','',12,1,'','1900-01-01','1900-01-01 00:00:00',0,0,'','',0);
INSERT INTO `tractasono`.`tbl_track` VALUES  (13,10,2,1,'Living Bad Dreams','',11,1,'','1900-01-01','1900-01-01 00:00:00',0,0,'','',0),
 (14,10,2,1,'One Shot At Glory','',10,1,'','1900-01-01','1900-01-01 00:00:00',0,0,'','',0),
 (15,10,2,1,'Battle Hymn','',9,1,'','1900-01-01','1900-01-01 00:00:00',0,0,'','',0),
 (16,10,2,1,'A Touch Of Evil','',8,1,'','1900-01-01','1900-01-01 00:00:00',0,0,'','',0),
 (17,10,2,1,'Between The Hammer & The Anvil','',7,1,'','1900-01-01','1900-01-01 00:00:00',0,0,'','',0),
 (18,10,2,1,'Night Crawler','',6,1,'','1900-01-01','1900-01-01 00:00:00',0,0,'','',0),
 (19,10,2,1,'Metal Meltdown','',5,1,'','1900-01-01','1900-01-01 00:00:00',0,0,'','',0),
 (20,10,2,1,'Leather Rebel','',4,1,'','1900-01-01','1900-01-01 00:00:00',0,0,'','',0),
 (21,10,2,1,'All Guns Blazing','',3,1,'','1900-01-01','1900-01-01 00:00:00',0,0,'','',0),
 (22,10,2,1,'Hell Patrol','',2,1,'','1900-01-01','1900-01-01 00:00:00',0,0,'','',0);
UNLOCK TABLES;
/*!40000 ALTER TABLE `tbl_track` ENABLE KEYS */;


--
-- Definition of view `tractasono`.`view_song`
--

DROP TABLE IF EXISTS `tractasono`.`view_song`;
DROP VIEW IF EXISTS `tractasono`.`view_song`;
CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `tractasono`.`view_song` AS select `t`.`IDtrack` AS `IDtrack`,`t`.`trackname` AS `trackname`,`t`.`tracknumber` AS `tracknumber`,`t`.`trackdiscnumber` AS `trackdiscnumber`,`t`.`IDgenre` AS `IDgenre`,`tg`.`genrename` AS `genrename`,`t`.`IDartist` AS `IDartist`,`ta`.`artistname` AS `artistname`,`t`.`IDalbum` AS `IDalbum`,`tal`.`albumname` AS `albumname` from (((`tractasono`.`tbl_track` `t` left join `tractasono`.`tbl_genre` `tg` on((`tg`.`IDgenre` = `t`.`IDgenre`))) left join `tractasono`.`tbl_artist` `ta` on((`ta`.`IDartist` = `t`.`IDartist`))) left join `tractasono`.`tbl_album` `tal` on((`tal`.`IDalbum` = `t`.`IDalbum`)));

--
-- Definition of view `tractasono`.`view_song_short`
--

DROP TABLE IF EXISTS `tractasono`.`view_song_short`;
DROP VIEW IF EXISTS `tractasono`.`view_song_short`;
CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `tractasono`.`view_song_short` AS select `v`.`tracknumber` AS `tracknumber`,`v`.`artistname` AS `artistname`,`v`.`trackname` AS `trackname`,`v`.`albumname` AS `albumname`,`v`.`genrename` AS `genrename` from `tractasono`.`view_song` `v` order by `v`.`IDartist`,`v`.`IDalbum`,`v`.`trackdiscnumber`,`v`.`tracknumber`;



/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
