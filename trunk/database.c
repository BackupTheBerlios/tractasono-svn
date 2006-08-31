#include <stdio.h>
#include <mysql/mysql.h>

#include "database.h"

// Globale database Variable
MYSQL *mysql;

void database_init()
{
	mysql = NULL;
	
	mysql = mysql_init(mysql);
	if (mysql != NULL) {
		printf("MySQl wurde initialisiert!\n");
	} else {
		printf("MySQL konnte nicht initialisiert werden!\n");
	}
}

void database_close()
{
	if (mysql != NULL) {
		mysql_close(mysql);
		printf("Die MySQL Verbindung wurde geschlossen!\n");
	}
}

void database_test()
{
	database_init();

	printf("MySQL Client Version: %s\n", mysql_get_client_info());

  	mysql_real_connect(mysql, "localhost", "tractasono", "monosono", "tractasono", 0, NULL, 0);
	printf("MySQL Server Version: %s\n", mysql_get_server_info(mysql));

	database_close();
}
