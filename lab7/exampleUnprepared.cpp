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

string sanitize(string s)
{
    string ns = "";
    for(int i = 0; i < s.length(); i++) {
        if (s[i] == '\'' || s[i] == '\\')
        ns = ns + '\\';
        ns = ns + s[i];
    }
    return ns;
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
        
        // form a query string using literal string
        // and user input
        string queryStr = "";
        queryStr = queryStr + "select first_name || ' ' || last_name "
        + "from hr.employees where last_name like '";
        string userinput;
        cout << "Last name: ";
        cin >> userinput;
        userinput = sanitize(userinput);
        queryStr = queryStr + userinput + "%'";
        
        // create a statment object
        Statement *stmt = conn->createStatement();
        
        // using the statement object to execute a query
        ResultSet *rs = stmt->executeQuery(queryStr);
        
        // process result
        while (rs->next()) {
            string name = rs->getString(1);
            cout << name << endl;
        }
        
        stmt->closeResultSet(rs);
        conn->terminateStatement(stmt);
        env->terminateConnection(conn);
        Environment::terminateEnvironment(env);
    } catch (SQLException & e) {
        cout << e.what();
        return 0;
    }
    
    return 0;
}
