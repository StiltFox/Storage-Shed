#include <sqlite3.h>
#include <Stilt_Fox/Scribe/File.h++>
#include "SqliteConnection.h++"

using namespace StiltFox::Scribe;
using namespace StiltFox::StorageShed;
using namespace std;

SqliteConnection::SqliteConnection(const string& connection) : SqliteConnection(connection.c_str()) {}

SqliteConnection::SqliteConnection(const char* connection)
{
    connectionString = connection;
}

SqliteConnection::~SqliteConnection()
{
    if (connection != nullptr) SqliteConnection::disconnect();
}

SqliteConnection& SqliteConnection::operator=(const string& connection)
{
    connectionString = connection;
    return *this;
}

void SqliteConnection::forEachTable(const function<void(string)>& perform)
{
    if (connection != nullptr)
    {
        auto dbConnection = (sqlite3*)connection;
        sqlite3_stmt* statement = nullptr;

        if (sqlite3_prepare(dbConnection, "select tbl_name from sqlite_schema where type = 'table';", -1, &statement, nullptr) == SQLITE_OK)
        {
            while(sqlite3_step(statement) == SQLITE_ROW) perform((char*)sqlite3_column_text(statement, 0));
        }

        sqlite3_finalize(statement);
    }
}

bool SqliteConnection::connect()
{
    if(connection == nullptr && checkIfValidSqlDatabase())
    {
        sqlite3* newConnection;
        if (sqlite3_open(connectionString.c_str(), &newConnection) == SQLITE_OK) connection = newConnection;
    }

    return isConnected();
}

void SqliteConnection::disconnect()
{
    sqlite3_close((sqlite3*)connection);
    connection = nullptr;
}

unordered_map<string, unordered_map<string, string>> SqliteConnection::getMetaData()
{
    unordered_map<string, unordered_map<string, string>> output;

    if (connection != nullptr)
    {
        auto dbConnection = (sqlite3*)connection;
        sqlite3_stmt* statement = nullptr;

        if (sqlite3_prepare(dbConnection, "select * from pragma_table_info(?);", -1, &statement, nullptr) == SQLITE_OK)
            forEachTable([&output, statement](const string& table)
            {
                sqlite3_bind_text(statement, 1, table.c_str(), table.size(), SQLITE_STATIC);
                while (sqlite3_step(statement) == SQLITE_ROW) 
                {
                    char* columnText = (char*)sqlite3_column_text(statement, 2);
                    output[table][(char*)sqlite3_column_text(statement,1)] = columnText == nullptr ? "" : columnText;
                }
                sqlite3_reset(statement);
            });

        sqlite3_finalize(statement);
    }

    return output;
}

unordered_set<string> SqliteConnection::validate(unordered_map<string, unordered_map<string, string>> validStructure)
{
    unordered_set<string> output;
    unordered_map<string, unordered_map<string, string>> metadata = getMetaData();

    for (auto const&[tableName, columnData] : metadata)
    {
        if (validStructure.contains(tableName))
        {
            for (auto const&[columnName, columnType] : columnData)
            {
                if(validStructure[tableName].contains(columnName))
                {
                    if (validStructure[tableName][columnName] != columnType)
                        output.insert("Column " + columnName + " in table " + tableName + " is the wrong type; expected: " + validStructure[tableName][columnName] + " actual: " + columnType);
                }
                else
                {
                    output.insert("Unwanted column in " + tableName + ": " + columnName);
                }
            }
        }
        else
        {
            output.insert("Unwanted table " + tableName);
        }
    }

    for (auto const&[tableName, columnData] : validStructure)
    {
        if (metadata.contains(tableName))
        {
            for (auto const&[columnName, columnType] : columnData)
                if (!metadata[tableName].contains(columnName)) output.insert("Misssing column in " + tableName + " type " + columnType);
        }
        else
        {
            output.insert("Missing table " + tableName);
        }
    }

    return output;
}

bool SqliteConnection::checkIfValidSqlDatabase()
{
    File dbFile = connectionString.c_str();
    return dbFile.readFirstNCharacters(16) == "SQLite format 3\000" || dbFile.getSize() == 0;
}

vector<unordered_map<string,string>> SqliteConnection::performQuery(string query)
{
    return performQuery(query, vector<string>{});
}

vector<unordered_map<string,string>> SqliteConnection::performQuery(string query, vector<string> inputs)
{
    vector<unordered_map<string,string>> output;

    if (connection != nullptr)
    {
        auto dbConnection = (sqlite3*)connection;
        sqlite3_stmt* statement = nullptr;

        if (sqlite3_prepare(dbConnection, query.c_str(), -1, &statement, nullptr) == SQLITE_OK)
        {
            for(int x=0; x<inputs.size(); x++) sqlite3_bind_text(statement, x+1, inputs[x].c_str(), inputs[x].size(), SQLITE_STATIC);
            
            while (sqlite3_step(statement) == SQLITE_ROW)
            {
                int columns = sqlite3_data_count(statement);
                output.emplace_back();
                for (int z=0; z < columns; z++)
                {
                    char* columnValue = (char*)sqlite3_column_text(statement, z);
                    output[output.size()-1][(char*)sqlite3_column_name(statement, z)] = columnValue == nullptr ? "" : columnValue;
                }
            }
        }

        sqlite3_finalize(statement);
    }

    return output;
}

void SqliteConnection::performUpdate(string query)
{
    performUpdate(query, {});
}

void SqliteConnection::performUpdate(string query, vector<string> inputs)
{
    if (connection != nullptr)
    {
        auto dbConnection = (sqlite3*)connection;
        sqlite3_stmt* statement = nullptr;

        if (sqlite3_prepare(dbConnection, query.c_str(), -1, &statement, nullptr) == SQLITE_OK)
        {
            for(int x=0; x<inputs.size(); x++) sqlite3_bind_text(statement, x+1, inputs[x].c_str(), inputs[x].size(), SQLITE_STATIC);
            sqlite3_step(statement);
        }

        sqlite3_finalize(statement);
    }
}

unordered_map<string,vector<unordered_map<string,string>>> SqliteConnection::getAllData()
{
    unordered_map<string,vector<unordered_map<string,string>>> output;

    forEachTable([&output, this](const string& table)
    {
        output[table] = this->performQuery("select * from " + table + ";");
    });

    return output;
}