#include <iostream>
#include <sstream>
#include <occi.h>
#include <termios.h>
#include <unistd.h>
#include <iomanip>
#include <stdlib.h>
#include <string>
using namespace std;
using namespace oracle::occi;

// Read Oracle password to establish connection
string readPassword()
{
    struct termios settings;
    tcgetattr( STDIN_FILENO, &settings );
    settings.c_lflag =  (settings.c_lflag & ~(ECHO));
    tcsetattr( STDIN_FILENO, TCSANOW, &settings );
    
    string password = "";
    getline(cin, password);
    
    settings.c_lflag = (settings.c_lflag |   ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &settings );
    return password;
}

// Give each prepared statement a name
struct STMT {
    string name;
    Statement *stmt;
};

// Prepare the statements that would be used repeatedly
// in this program
int initStatements(Connection *conn, STMT * & statements)
{
    int size = 3;
    statements = new STMT[size];
    
    // retrieve  all Client ID from donations with ClientId
    statements[0].name = "addServer";
    string queryStr = "INSERT INTO Server (sid,sname) VALUES (:2,:1)";
    statements[0].stmt = conn->createStatement(queryStr);

    // get max did
    statements[1].name = "maxSid";
    queryStr = "select max(sid) from Server S";
    statements[1].stmt = conn->createStatement(queryStr);

    // commit
    statements[2].name = "commit";
    queryStr = "commit";
    statements[2].stmt = conn->createStatement(queryStr);
    return size;
}

// Given the name, find the corresponding prepared sql statement
Statement * findStatement(string name, STMT *statements, int size)
{
    for(int i = 0; i < size; i++) {
        if (statements[i].name == name)
        return statements[i].stmt;
    }
    return 0;
}

// Terminate all the prepared statements
void terminateStatements(Connection *conn, STMT *statements, int size)
{
    for(int i = 0; i < size; i++)
    conn->terminateStatement(statements[i].stmt);
}

// read in a phone number and
// display all phone calls originated from this number
void addServer(STMT *statements, int size)
{
    string name, sid,id;
    cout << "enter name for your new employee ";
    cin >> name;
    
    Statement *stmt = findStatement("maxSid", statements, size);
    ResultSet *rs = stmt->executeQuery();
    while (rs->next()) {
          id= rs->getString(1) ;
    }
    stmt->closeResultSet(rs);

    int temp =atoi(id.c_str());
    temp++;
    
    std::stringstream ss;
    ss << std::setw(4) <<std:: setfill('0') << temp;
    sid =ss.str();
    
    stmt = findStatement("addServer", statements, size);
    stmt->setString(2, name);
    stmt->setString(1, sid);
    rs = stmt->executeQuery();
    stmt->closeResultSet(rs);

    stmt = findStatement("commit", statements, size);
    rs = stmt->executeQuery();
    stmt->closeResultSet(rs);

    cout<<"Thier ID is:"<<sid<<"\n";
}


int main()
{
    string userName;
    string password;
    const string connectString = "sunfire.csci.viu.ca";
    
    // Establish connection using userName and password
    cout << "Your user name: ";
    getline(cin, userName);
    
    cout << "Your password: ";
    password = readPassword();
    cout << endl;
    
    Environment *env = Environment::createEnvironment();
    Connection *conn = env->createConnection
    (userName, password, connectString);
    
    STMT *statements;
    int size = initStatements(conn, statements);
    
    addServer(statements, size);
    
    // clean up environment before terminating
    terminateStatements(conn, statements, size);
    env->terminateConnection(conn);
    Environment::terminateEnvironment(env);
    
    return 0;
}
