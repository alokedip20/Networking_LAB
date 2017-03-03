#include <iostream>
#include <cstdlib>
#include <sqlite3.h>
#include <string>
#include <stdio.h>
#include <sstream>
using namespace std;
#include "mysqlite.h"
//typedef struct user_data record;
int main(int argc, char const *argv[])
{
	record R;
	create_database("Networking_Assignmenet3.db");
	create_table("USERS");
	insert("aloke57","Hello",1);
	R = retrieve("aloke57");
	cout<<" In main programme : \n Username : "<<R.username<<" Messege : "<<R.messege<<" Status : "<<R.status<<endl;
	update("aloke57","hi");
	R = retrieve("aloke57");
	cout<<" In main programme updated : \n Username : "<<R.username<<" Messege : "<<R.messege<<" Status : "<<R.status<<endl;
	return 0;
}