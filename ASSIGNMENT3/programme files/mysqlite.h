#ifndef mysqlite_h_
#define mysqlite_h_
extern struct user_data
{  
   string username,messege,status;
}dummy;
typedef struct user_data record;
void create_table(string table_name);
void create_database(string database_name);
void insert(string user_name,string data,int state);
void update(string user_name,string data);
record retrieve(string name);
string TABLE_NAME = "";
string DATABASE_NAME = "";
record result;
static int call_back(void *data,int argc,char *argv[],char **azColName){
   int i;
   result.username = argv[0];
   result.messege = argv[1];
   result.status = argv[2];
   return 0;
}
void create_database(string database_name){
   int db;
   sqlite3 *database_object=NULL;
   db= sqlite3_open((const char *)database_name.c_str(),&database_object);
   if(database_object){
      cout<<" Database "<<database_name<<" has been created"<<endl;
      DATABASE_NAME = database_name;
   }
   else{
      cout<<" Error while creating database"<<endl;
      sqlite3_errmsg(database_object);
   }  
   sqlite3_close(database_object); 
}
void create_table(string table_name){
   int db;
   sqlite3 *database_object=NULL;
   char *error = 0;
   db = sqlite3_open(DATABASE_NAME.c_str(),&database_object);
   if(db == SQLITE_OK){
      TABLE_NAME = table_name;
      string sql_query = "CREATE TABLE "+TABLE_NAME+"("  \
         "USERNAME           CHAR(50)    NOT NULL," \
         "DATA           CHAR(256)    NOT NULL," \
         "STATUS        INT      NOT NULL);";
      db = sqlite3_exec(database_object,(const char *)(sql_query.c_str()),call_back,0,&error);
      if(db == SQLITE_OK){
         cout<<" Table "<<table_name<<" has been succesfully created"<<endl;
      }
      else{
         cout<<error<<endl;
      }
   }
   else{
      sqlite3_errmsg(database_object);
   }
   sqlite3_close(database_object);
}
void insert(string user_name,string data,int state){
   int db;
   sqlite3 *database_object=NULL;
   char *error = 0;
   db = sqlite3_open(DATABASE_NAME.c_str(),&database_object);
   if(db == SQLITE_OK){
      stringstream temp;
      string s;
      temp << state;
      s = temp.str();
      cout<<TABLE_NAME<<endl;
      string sql_query = "INSERT INTO "+TABLE_NAME +"(USERNAME,DATA,STATUS)" \
                           " VALUES ("+"'"+user_name+"'"+","+"'"+data+"'"+","+"'"+s+"'"+");";
      db = sqlite3_exec(database_object,(const char *)sql_query.c_str(),call_back, 0,&error);
      if(db == SQLITE_OK){
         cout<<" User name : "<<user_name<<"\n Data : "<<data<<"\n state : "<<state<<endl;
      }
      else{
         cout<<error<<endl;
         exit(0);
      }
   }
   else{
      sqlite3_errmsg(database_object);
   }
   sqlite3_close(database_object);
}
void update(string user_name,string data){
   int db;
   sqlite3 *database_object=NULL;
   char *error = 0;
   db = sqlite3_open(DATABASE_NAME.c_str(),&database_object);
   if(db == SQLITE_OK){
      string sql_query = "UPDATE "+TABLE_NAME+" SET DATA"+" = "+"'"+data+"'"+" WHERE USERNAME = "+"'"+user_name+"'"+";";
      cout<<sql_query<<endl;
      db = sqlite3_exec(database_object,(const char *)sql_query.c_str(),call_back,0,&error);
      if(db == SQLITE_OK){
         cout<<" data for the user : "<<user_name<<" has been updated"<<endl;
      }
      else{
         cout<<error<<endl;
      }
   }
   else{
      sqlite3_errmsg(database_object);
   }
   sqlite3_close(database_object);
}
record retrieve(string name){
   int db;
   sqlite3 *database_object=NULL;
   char *error = 0;
   db = sqlite3_open(DATABASE_NAME.c_str(),&database_object);
   if(db == SQLITE_OK){
      string sql_query = "SELECT * FROM "+TABLE_NAME+" WHERE USERNAME = "+"'"+name+"';";
      db = sqlite3_exec(database_object,(const char *)sql_query.c_str(),call_back,0,&error);
      if(db == SQLITE_OK){
         cout<<" successfully fetched" <<endl;
      }
      else{
         cout<<error<<endl;
      }
   }
   else{
      sqlite3_errmsg(database_object);
   }
   sqlite3_close(database_object);
   return result;
}
#endif