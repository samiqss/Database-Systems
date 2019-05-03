#include <iostream>
#include <sstream>
#include <occi.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
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
    int size = 4;
    statements = new STMT[size];
    
    // retrieve  all Client ID from donations with ClientId
    statements[0].name = "checkID";
    string queryStr = "select cid from Clients C where cid = :1";
    statements[0].stmt = conn->createStatement(queryStr);

    // get max did
    statements[1].name = "maxDid";
    queryStr = "select max(did) from Donations D";
    statements[1].stmt = conn->createStatement(queryStr);

    // insert new donations
    statements[2].name = "insertDonation";
    queryStr = "INSERT INTO Donations (did, cid, donationDate, amount) VALUES (:2, :1, CURRENT_DATE, :3)";
    statements[2].stmt = conn->createStatement(queryStr);

    // commit
    statements[3].name = "commit";
    queryStr = "commit";
    statements[3].stmt = conn->createStatement(queryStr);
    
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

// check whether a given id exists
bool checkID(Statement *stmt, string cid)
{
    stmt->setString(1, cid);

    ResultSet *rs = stmt->executeQuery();
    bool exists = rs->next();
    
    stmt->closeResultSet(rs);
    return exists;
}

bool validAmount(string amount)
{
    int x = atoi(amount.c_str());
    if (amount.length() < 2 || amount.length() > 8 || x < 0){
	cout<< "donation amount has to be positive, between 2 to 8 in length";
	return false;
    }
    return true;
}

// read in a phone number and
// display all phone calls originated from this number
void insertDonation(STMT *statements, int size)
{
    string cid, amount;
    int  did;
    cout << "enter clients id and amount: ";
    cin >> cid >> amount;
    
    if (cid == "0" && amount =="0"){
	 cout<< "program shutdown \n";
	 return;
    }

    Statement *stmt = findStatement("checkID", statements, size);
    if (!checkID(stmt, cid)) {
        cout << "Client id"<< " doesn't exist.\n"<<"note: even if it exists needs to be CHAR(4) \n";
        return;
    }
    
    stmt = findStatement("maxDid", statements, size);
    ResultSet *rs = stmt->executeQuery();
    while (rs->next()) {
	   did = rs->getInt(1);
	   did++;
    }
    stmt->closeResultSet(rs);

    stmt = findStatement("insertDonation", statements, size);
    if (!validAmount(amount)) {
        return;
    }
    
    stmt->setInt(1, did);
    stmt->setString(2, cid);
    stmt->setString(3, amount);
    rs = stmt->executeQuery();
    stmt->closeResultSet(rs);

    stmt = findStatement("commit", statements, size);
    rs = stmt->executeQuery();
    stmt->closeResultSet(rs);

    cout<<"thank you donation has been accepted.\n";
    insertDonation(statements, size);
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
    
    insertDonation(statements, size);
    
    // clean up environment before terminating
    terminateStatements(conn, statements, size);
    env->terminateConnection(conn);
    Environment::terminateEnvironment(env);
    
    return 0;
}
