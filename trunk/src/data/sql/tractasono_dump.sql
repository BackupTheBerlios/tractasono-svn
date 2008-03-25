BEGIN TRANSACTION;
CREATE TABLE "tbl_artist" (
    "IDartist" INTEGER PRIMARY KEY,
    "artistname" TEXT NOT NULL
);
INSERT INTO "tbl_artist" VALUES(1,'HammerFall');
INSERT INTO "tbl_artist" VALUES(2,'Krokus');
INSERT INTO "tbl_artist" VALUES(3,'Blutengel');
INSERT INTO "tbl_artist" VALUES(4,'Daft Punk');
CREATE TABLE "tbl_genre" (
    "IDgenre" INTEGER PRIMARY KEY,
    "genrename" TEXT NOT NULL
);
INSERT INTO "tbl_genre" VALUES(1,'Metal');
INSERT INTO "tbl_genre" VALUES(2,'Industrial');
INSERT INTO "tbl_genre" VALUES(3,'Electro');
CREATE TABLE "tbl_album" (
    "IDalbum" INTEGER PRIMARY KEY,
    "IDgenre" INTEGER NOT NULL,
    "albumname" TEXT NOT NULL,
    "albumyear" TEXT,
    "albumaddetat" TEXT,
    "albumvaluation" TEXT,
    "albumcover" TEXT,
    "albumdiscid" TEXT,
    "albumiscompilation" INTEGER
);
INSERT INTO "tbl_album" VALUES(1,1,'Threshold',NULL,NULL,NULL,NULL,NULL,NULL);
INSERT INTO "tbl_album" VALUES(2,1,'Glory to the Brave',NULL,NULL,NULL,NULL,NULL,NULL);
INSERT INTO "tbl_album" VALUES(3,2,'Seelenschmerz',NULL,NULL,NULL,NULL,NULL,NULL);
INSERT INTO "tbl_album" VALUES(4,1,'Hellraiser',NULL,NULL,NULL,NULL,NULL,NULL);
INSERT INTO "tbl_album" VALUES(5,3,'Alive 2007',NULL,NULL,NULL,NULL,NULL,NULL);
CREATE TABLE "tbl_track" (
    "IDtrack" INTEGER PRIMARY KEY,
    "IDartist" INTEGER NOT NULL,
    "IDalbum" INTEGER NOT NULL,
    "trackname" TEXT NOT NULL,
    "trackpath" TEXT,
    "tracknumber" INTEGER,
    "trackdiscnumber" INTEGER,
    "trackcomposer" TEXT,
    "trackyear" TEXT,
    "trackaddetat" TEXT,
    "trackplaycounter" INTEGER,
    "trackvaluation" INTEGER,
    "trackcomment" TEXT
);
INSERT INTO "tbl_track" VALUES(1,1,1,'Track 1',NULL,1,1,NULL,NULL,NULL,NULL,NULL,NULL);
INSERT INTO "tbl_track" VALUES(2,1,1,'Track 2',NULL,2,1,NULL,NULL,NULL,NULL,NULL,NULL);
INSERT INTO "tbl_track" VALUES(3,1,2,'Track 1',NULL,1,1,NULL,NULL,NULL,NULL,NULL,NULL);
INSERT INTO "tbl_track" VALUES(4,2,4,'Track 1',NULL,1,1,NULL,NULL,NULL,NULL,NULL,NULL);
INSERT INTO "tbl_track" VALUES(5,2,4,'Track 2',NULL,2,1,NULL,NULL,NULL,NULL,NULL,NULL);
INSERT INTO "tbl_track" VALUES(6,3,3,'Track 1',NULL,1,1,NULL,NULL,NULL,NULL,NULL,NULL);
INSERT INTO "tbl_track" VALUES(7,4,5,'Robot Rock',NULL,1,1,NULL,NULL,NULL,NULL,NULL,NULL);
CREATE TABLE "tbl_settings" (
    "IDsettings" INTEGER PRIMARY KEY,
    "settingsgroup" TEXT,
    "settingskey" TEXT,
    "settingstype" INTEGER,
    "settingsvalue" TEXT
);
INSERT INTO "tbl_settings" VALUES(1,'Allgemein','Version',1,'0.1');
INSERT INTO "tbl_settings" VALUES(2,'Allgemein','DBVersion',1,'0.1');
INSERT INTO "tbl_settings" VALUES(3,'Interface','AktuellerTab',2,'23');
INSERT INTO "tbl_settings" VALUES(4,'LCD','StartText',1,'Willkommen!');
INSERT INTO "tbl_settings" VALUES(5,'Allgemein','Firstrun',3,'1');
INSERT INTO "tbl_settings" VALUES(6,'Interface','AktTab1',2,'40');
INSERT INTO "tbl_settings" VALUES(7,'Interface','AktTab1',2,'40');
INSERT INTO "tbl_settings" VALUES(8,'Interface','AktTab1',2,'40');
CREATE VIEW "view_artist_album" AS
	SELECT	ar.IDartist AS IDartist, ar.artistname AS artistname,
			al.IDalbum AS IDalbum, al.albumname AS albumname
	FROM	tbl_artist AS ar
			JOIN tbl_track AS tr ON tr.IDartist = ar.IDartist
			JOIN tbl_album AS al ON al.IDalbum = tr.IDalbum
	GROUP BY al.IDalbum;
CREATE VIEW "view_track_tree" AS
	SELECT	tr.IDtrack AS IDtrack,
			tr.IDalbum AS IDalbum,
			tr.tracknumber AS tracknumber,
			tr.trackdiscnumber AS discnumber,
			tr.trackname AS trackname,
			ar.artistname AS artistname,
			al.albumname AS albumname
	FROM	tbl_track AS tr
			JOIN tbl_artist AS ar ON ar.IDartist = tr.IDartist
			JOIN tbl_album AS al ON al.IDalbum = tr.IDalbum
	ORDER BY tr.trackdiscnumber, tr.tracknumber;
COMMIT;
