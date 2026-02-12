#include <Stilt_Fox/Scribe/File.h++>
#include "SqliteConnection.h++"

using namespace StiltFox::Scribe;
using namespace StiltFox::StorageShed;
using namespace StiltFox::StorageShed::Data;
using namespace std;

SqliteConnection::SqliteConnection(const string& connection) : SqliteConnection(connection.c_str()) {}

SqliteConnection::SqliteConnection(const char* connection)
{
    connectionString = connection;
    this->connection = nullptr;
}

SqliteConnection::SqliteConnection(const SqliteConnection& toCopy) : SqliteConnection(toCopy.connectionString){}

SqliteConnection::~SqliteConnection()
{
    if (connection != nullptr) SqliteConnection::disconnect();
}

SqliteConnection& SqliteConnection::operator=(const string& connection)
{
    connectionString = connection;
    return *this;
}

bool SqliteConnection::checkIfValidSqlDatabase() const
{
    File dbFile = connectionString.c_str();
    return dbFile.readFirstNCharacters(16) == "SQLite format 3\000" || dbFile.getSize() == 0;
}

void SqliteConnection::forEachTable(const function<void(string)>& perform, vector<StructuredQuery>& queryTracker) const
{
    auto dbConnection = connection;
    sqlite3_stmt* statement = nullptr;
    StructuredQuery getTablesQuery =
    {
        "select tbl_name from sqlite_schema where type = 'table';",
        {}
    };

    queryTracker.emplace_back(getTablesQuery);

    if (sqlite3_prepare(dbConnection, getTablesQuery.query.c_str(), -1, &statement, nullptr) ==
        SQLITE_OK)
    {
        while(sqlite3_step(statement) == SQLITE_ROW) perform((char*)sqlite3_column_text(statement, 0));
    }

    sqlite3_finalize(statement);
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
    sqlite3_close(connection);
    connection = nullptr;
}

Result<void*> SqliteConnection::startTransaction()
{
    return performUpdate("begin transaction;");
}

Result<void*> SqliteConnection::commitTransaction()
{
    return performUpdate("commit transaction;");
}

Result<void*> SqliteConnection::rollbackTransaction()
{
    return performUpdate("rollback transaction;");
}

Result<void*> SqliteConnection::performUpdate(const string statement)
{
    const StructuredQuery query = {statement, {}};
    return performUpdate(query);
}

Result<void*> SqliteConnection::performUpdate(const StructuredQuery& statement)
{
    const Result<QueryReturnData> values = performQuery(statement);
    return {values.connected, values.errorText, values.performedQueries, nullptr};
}

Result<TableDefinitions> SqliteConnection::getMetaData()
{
    Result<TableDefinitions> output;
    const string getAllTableInfo = "select * from pragma_table_info(?);";
    output.connected = isConnected();

    if (output.connected)
    {
        const auto dbConnection = connection;
        sqlite3_stmt* statement = nullptr;

        if (sqlite3_prepare(dbConnection, getAllTableInfo.c_str(), -1, &statement, nullptr) == SQLITE_OK)
        {
            forEachTable([&output, statement, getAllTableInfo](const string& table)
            {
                StructuredQuery query = {getAllTableInfo, {table}};
                sqlite3_bind_text(statement, 1, table.c_str(), table.size(), SQLITE_STATIC);

                while (sqlite3_step(statement) == SQLITE_ROW)
                {
                    string columnText =
                        string((char*)sqlite3_column_text(statement,2), sqlite3_column_bytes(statement,2));
                    output.data[table][(char*)sqlite3_column_text(statement,1)] = columnText;
                }
                output.performedQueries.emplace_back(query);
                sqlite3_reset(statement);
            }, output.performedQueries);
        }
        else
        {
            output.errorText = sqlite3_errmsg(dbConnection);
        }

        sqlite3_finalize(statement);
    }

    return output;
}

unordered_set<string> SqliteConnection::validate(TableDefinitions tableDefinitions, bool strict)
{
    unordered_set<string> output;
    Result<TableDefinitions> metaData = getMetaData();

    if (!metaData.connected) output.emplace("Database not connected " + connectionString);
    if (!metaData.errorText.empty()) output.emplace(metaData.errorText);

    if (output.empty())
    {
        if (strict)
        {
            for (auto const&[tableName, columnData] : metaData.data)
            {
                if (tableDefinitions.contains(tableName))
                {
                    for (auto const&[columnName, columnType] : columnData)
                    {
                        if (!tableDefinitions[tableName].contains(columnName))
                            output.emplace("Unwanted column " + columnName + " in table " + tableName);
                    }
                }
                else
                {
                    output.emplace("Unwanted table " + tableName);
                }
            }
        }

        for (auto const&[tableName, columnData] : tableDefinitions)
        {
            if (metaData.data.contains(tableName))
            {
                for (auto const&[columnName, columnType] : columnData)
                {
                    if (metaData.data[tableName].contains(columnName))
                    {
                        if(metaData.data[tableName][columnName] != columnType)
                            output.emplace("Column " + columnName + " in table " + tableName +
                                " is the wrong type; expected: " + columnType + " actual: " +
                                metaData.data[tableName][columnName]);
                    }
                    else
                    {
                        output.emplace("Missing column in " + tableName + ": " + columnName);
                    }
                }
            }
            else
            {
                output.emplace("Missing table " + tableName);
            }
        }
    }

    return output;
}

Result<QueryReturnData> SqliteConnection::performQuery(string query)
{
    const StructuredQuery structuredQuery = {query, {}};
    return performQuery(structuredQuery);
}

Result<QueryReturnData> SqliteConnection::performQuery(StructuredQuery structuredQuery)
{
    Result<QueryReturnData> output = {false, "", {structuredQuery}, {}};

    if (isConnected())
    {
        output.connected = true;
        auto dbConnection = connection;
        sqlite3_stmt* statement = nullptr;

        if (
            sqlite3_prepare(dbConnection, structuredQuery.query.c_str(), -1, &statement, nullptr) ==
            SQLITE_OK
           )
        {
            for (int x=0; x<structuredQuery.parameters.size(); x++)
                sqlite3_bind_text(statement, x+1, structuredQuery.parameters[x].c_str(),
                    structuredQuery.parameters[x].size(), SQLITE_STATIC);

            while (sqlite3_step(statement) == SQLITE_ROW)
            {
                int columns = sqlite3_data_count(statement);
                output.data.emplace_back();
                for (int z=0; z<columns; z++)
                {
                    string columnValue =
                        string((char*)sqlite3_column_text(statement, z), sqlite3_column_bytes(statement, z));
                    output.data[output.data.size() - 1][(char*)sqlite3_column_name(statement, z)] = columnValue;
                }
            }

            if (sqlite3_finalize(statement) != SQLITE_OK) output.errorText = sqlite3_errmsg(dbConnection);
        }
        else
        {
            output.errorText = sqlite3_errmsg(dbConnection);
        }
    }

    return output;
}

Result<MultiTableData> SqliteConnection::getAllData()
{
    Result<MultiTableData> output = {false, "", {}, {}};

    if (isConnected())
    {
        output.connected = true;
        forEachTable([&output, this](const string& table)
        {
            auto tableData = this->performQuery("select * from " + table + ";");
            output.data[table] = tableData.data;
            if (!output.errorText.empty()) output.errorText += " ";
            output.errorText += tableData.errorText;
            if(!tableData.performedQueries.empty())
                output.performedQueries.emplace_back(tableData.performedQueries.front());
        }, output.performedQueries);
    }

    return output;
}

bool SqliteConnection::isConnected()
{
    return connection != nullptr;
}

std::string SqliteConnection::getConnectionString()
{
    return connectionString;
}