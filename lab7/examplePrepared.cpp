#include <iostream>
#include <occi.h>
#include <termios.h>
#include <unistd.h>
using namespace std;
using namespace oracle::occi;

// read database password from user input
// without showing the password on the screen
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

int main()
{
    string userName;
    string password;
    // address of the Oracle server
    const string connectString = "sunfire.csci.viu.ca";
    
    cout << "Your user name: ";
    getline(cin, userName);
    
    cout << "Your password: ";
    password = readPassword();
    cout << endl;
    try {
        // establish database connection
        Environment *env = Environment::createEnvironment();
        Connection *conn = env->createConnection
        (userName, password, connectString);
        
        // create a prepared statement
        string queryStr = "";
        queryStr = queryStr + "select first_name || ' ' || last_name "
        + "from hr.employees where last_name like :1";
        Statement *stmt = conn->createStatement(queryStr);
        
        // read query parameter
        string userinput;
        cout << "Last name: ";
        cin >> userinput;
        userinput = userinput + "%";
        
        // set the query parameter
        stmt->setString(1, userinput);
        
        // execute the prepared query statement
        ResultSet *rs = stmt->executeQuery();
        
        // process result
        while (rs->next()) {
            string name = rs->getString(1);
            cout << name << endl;
        }
        
        stmt->closeResultSet(rs);
        
        Statement *ustmt =
        conn->createStatement("insert into T values (:1, sysdate)");
        
        ustmt->setInt(1, 20);
        int rows = ustmt->executeUpdate();
        cout << "Inserted " << rows << " rows" << endl;
        
        conn->terminateStatement(stmt);
        env->terminateConnection(conn);
        Environment::terminateEnvironment(env);
    } catch (SQLException & e) {
        cout << e.what();
        return 0;
    }
    
    return 0;
}
