#include <iostream>
#include <occi.h>
#include <fstream>
#include <string>
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
    int size = 2;
    statements = new STMT[size];

    // retrieve  all Client ID from donations with ClientId
    statements[0].name = "checkID";
    string queryStr = "select D.cid, name, address, sum(amount) as total from Donations D, Clients C where D.cid=C.cid and D.cid = :1 group by D.cid, name, address";
    //string queryStr = "select cid from Clients where cid = :1";
    statements[0].stmt = conn->createStatement(queryStr);
    
    // retrieve information of a ClientId
    statements[1].name = "printTaxR";
    queryStr = "select donationDate, amount  from Clients C, Donations D where C.cid=D.cid and D.cid = :1";
    statements[1].stmt = conn->createStatement(queryStr);

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


// check whether a given cid exists
bool checkID(Statement *stmt, string cid)
{
    stmt->setString(1, cid);
    
    ResultSet *rs = stmt->executeQuery();
    bool exists = rs->next();
    
    stmt->closeResultSet(rs);
    return exists;
}

// read in clients id then generate a tax receipt
// with client id, client name, client's address, the donation date 
// and amount of each donation made in 2017,
// the total amount of donation made in 2017

void displayCallRecord(STMT *statements, int size)
{
    string cid;
    cout << "Enter one client id to print a tax receipt: ";
    cin >> cid;
    string filename="TAX-"+cid+"-2017.txt";
    
    Statement *stmt = findStatement("checkID", statements, size);
    
    if (!checkID(stmt, cid)) {
        cout<< "cid: "<< cid 
        << " doesn't exist as a clients id for any donation.\n"<<"note: even if it exists cid needs to be CHAR(4)\n";
          return;
    }
    
    ofstream outputFile;
    outputFile.open(filename.c_str());

    stmt = findStatement("checkID", statements, size);
    stmt->setString(1, cid);
    ResultSet *rs = stmt->executeQuery();
    while (rs->next()) {
       // outputFile
        outputFile<< "TAX RECEIPT (2017)\n" <<"CLIENT ID: "<< rs->getString(1) << "\n"
        << "CLIENT NAME:" << rs->getString(2) <<"\n"<<"CLIENT ADDRESS:"
        << rs->getString(3) << "\n"<<"TOTAL DONATION AMOUNT IN 2017:"
        << rs->getString(4)<< endl<< "2017 DONATION DETAILS: \n"
	<<"DATE (YYYY-MM-DD)                  AMOUNT \n";
    }
    stmt->closeResultSet(rs);

    stmt = findStatement("printTaxR", statements, size);
    stmt->setString(1, cid);
    rs = stmt->executeQuery();
    while (rs->next()) {
        //outputFile
	outputFile<< rs->getString(1) <<"     "<< rs->getString(2)  << endl;
    }
    stmt->closeResultSet(rs);
    outputFile.close();
    cout<< "Receipt has been generated, have a nice day! \n";
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
    Connection *conn = env->createConnection(userName, password, connectString);
   
    STMT *statements;
    int size = initStatements(conn, statements);
    
    displayCallRecord(statements,size);
    
    // clean up environment before terminating
    terminateStatements(conn, statements, size);
    env->terminateConnection(conn);
    Environment::terminateEnvironment(env);
    
    return 0;
}
