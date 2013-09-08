#include "database.h"
#include <mysql.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static MYSQL *sql_connect;

void connect_database ()
{
	const char *server = "localhost";
	const char *user = "root";
	const char *password = "16889188";
	const char *database = "user";

    sql_connect = mysql_init (NULL);
	if (!mysql_real_connect (sql_connect, server, user, password, database, 0, NULL, 0))
	{
		fprintf (stderr, "%s\n", mysql_error (sql_connect));
		exit (-1);
	}
}
void add_user (struct user_info *user)
{
	char query[128] = "insert into user_info values(\"";
	strcat (query, user->name);
	strcat (query, "\",\"");
	strcat (query, user->password);
	strcat (query, "\",\"");
	strcat (query, user->email);
	strcat (query, "\")");

	if (mysql_query (sql_connect, query))
	{
		fprintf (stderr, "%s\n", mysql_error (sql_connect));
		exit (-1);
	}
}
void change_user_info (struct user_info *user)
{
	char query[128] = "update user_info set password=\"";
	strcat (query, user->password);
	strcat (query, "\",email=\"");
	strcat (query, user->email);
	strcat (query, "\" where name=\"");
	strcat (query, user->name);
	strcat (query, "\"");

	if (mysql_query (sql_connect, query))
	{
		fprintf (stderr, "%s\n", mysql_error (sql_connect));
		exit (-1);
	}
	
}
struct user_info *get_user_info (const char *name)
{
	static struct user_info user;
	MYSQL_RES *res;
	MYSQL_ROW *row;
	char query[128] = "select * from user_info where name=\"";
	strcat (query, name);
	strcat (query, "\"");
	
	if (mysql_query (sql_connect, query))
	{
		fprintf (stderr, "%s\n", mysql_error (sql_connect));
		exit (-1);
	}
	res = mysql_store_result (sql_connect);
	row = (MYSQL_ROW *)mysql_fetch_row (res);

	if (row == NULL)
		return NULL;
	
	strcpy (user.name, (char *)row[0]);
	strcpy (user.password, (char *)row[1]);
	strcpy (user.email, (char *)row[2]);

	mysql_free_result (res);
	return &user;
}
