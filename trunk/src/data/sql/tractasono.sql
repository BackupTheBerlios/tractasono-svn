-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ --
-- tractasono Datenbank Struktur v 0.2 --
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ --

--+====================================================================+
--| Tabellen                                                           |
--+====================================================================+

-- Alben
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

-- Artisten
CREATE TABLE "tbl_artist" (
    "IDartist" INTEGER PRIMARY KEY,
    "artistname" TEXT NOT NULL
);

-- Genre
CREATE TABLE "tbl_genre" (
    "IDgenre" INTEGER PRIMARY KEY,
    "genrename" TEXT NOT NULL
);

-- Einstellungen
CREATE TABLE "tbl_settings" (
    "IDsettings" INTEGER PRIMARY KEY,
    "settingsgroup" TEXT,
    "settingskey" TEXT,
    "settingstype" INTEGER,
    "settingsvalue" TEXT
);

-- Tracks
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


--+====================================================================+
--| Views                                                              |
--+====================================================================+

-- Verknüpfung Artist - Album (zB für Album Übersicht)
CREATE VIEW "view_artist_album" AS
	SELECT	ar.IDartist AS IDartist, ar.artistname AS artistname,
			al.IDalbum AS IDalbum, al.albumname AS albumname
	FROM	tbl_artist AS ar
			JOIN tbl_track AS tr ON tr.IDartist = ar.IDartist
			JOIN tbl_album AS al ON al.IDalbum = tr.IDalbum
	GROUP BY al.IDalbum;

-- Verknüpfung Artist - Album - Track (zB für Track Übersicht)
CREATE VIEW "view_track_tree" AS
SELECT	tr.IDtrack AS IDtrack,
			tr.IDalbum AS IDalbum,
			tr.tracknumber AS tracknumber,
			tr.trackdiscnumber AS discnumber,
			tr.trackname AS trackname,
			ar.artistname AS artistname,
			al.albumname AS albumname,
		ar.IDartist AS IDartist
	FROM	tbl_track AS tr
			JOIN tbl_artist AS ar ON ar.IDartist = tr.IDartist
			JOIN tbl_album AS al ON al.IDalbum = tr.IDalbum
	ORDER BY tr.trackdiscnumber, tr.tracknumber;


--+====================================================================+
--| Default Daten werden ab hier eingefügt                             |
--+====================================================================+

-- Starttext des Displays
INSERT INTO "tbl_settings"
	("settingsgroup","settingskey","settingstype","settingsvalue") VALUES
	("LCD","StartText","1","Willkommen bei tractasono!")
