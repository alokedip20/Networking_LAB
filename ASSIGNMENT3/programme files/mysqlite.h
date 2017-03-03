#ifndef mysqlite_h_
#define mysqlite_h_
extern struct user_data
{  
   string username,messege,status;
   int id;
}dummy;
typedef struct user_data record;
void create_table(string table_name);
void create_database(string database_name);
void insert(string user_name,string data,int state,int id);
void update(int id,string data);
//record retrieve(int id);
string TABLE_NAME = "";
string DATABASE_NAME = "";
record result;
static int call_back(void *data,int argc,char *argv[],char **azColName){
   int i;
   result.id = atoi(argv[0]);
   result.username = argv[1];
   result.messege = argv[2];
   result.status = argv[3];
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
         "ID           INT    PRIMARY KEY   NOT NULL," \
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
void insert(string user_name,string data,int state,int id){
   int db;
   sqlite3 *database_object=NULL;
   char *error = 0;
   db = sqlite3_open(DATABASE_NAME.c_str(),&database_object);
   if(db == SQLITE_OK){
      stringstream temp,temp2;
      string s,s2;
      temp << state;
      temp2 << id;
      s2 = temp2.str();
      s = temp.str();
      cout<<TABLE_NAME<<endl;
      string sql_query = "INSERT INTO "+TABLE_NAME +"(ID,USERNAME,DATA,STATUS)" \
                           " VALUES ("+s2+","+"'"+user_name+"'"+","+"'"+data+"'"+","+"'"+s+"'"+");";
      db = sqlite3_exec(database_object,(const char *)sql_query.c_str(),call_back, 0,&error);
      if(db == SQLITE_OK){
         cout<<"ID : "<<id<<" User name : "<<user_name<<"\n Data : "<<data<<"\n state : "<<state<<endl;
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
void update(int id,string data){
   int db;
   stringstream temp2;
   string s2;
   temp2 << id;
   s2 = temp2.str();
   sqlite3 *database_object=NULL;
   char *error = 0;
   db = sqlite3_open(DATABASE_NAME.c_str(),&database_object);
   if(db == SQLITE_OK){
      string sql_query = "UPDATE "+TABLE_NAME+" SET DATA"+" = "+"'"+data+"'"+" WHERE ID = "+s2+";";
      cout<<sql_query<<endl;
      db = sqlite3_exec(database_object,(const char *)sql_query.c_str(),call_back,0,&error);
      if(db == SQLITE_OK){
         cout<<" data for the user : "<<id<<" has been updated"<<endl;
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
record retrieve(int id){
   int db;
   stringstream temp2;
   string s2;
   temp2 << id;
   s2 = temp2.str();
   sqlite3 *database_object=NULL;
   char *error = 0;
   db = sqlite3_open(DATABASE_NAME.c_str(),&database_object);
   if(db == SQLITE_OK){
      string sql_query = "SELECT * FROM "+TABLE_NAME+" WHERE id = "+s2+";";
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