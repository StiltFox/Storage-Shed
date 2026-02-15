/*******************************************************
* Created by Cryos on 3/26/25.
* Copyright 2025 Stilt Fox® LLC
*
* See LICENSE on root project directory for terms
* of use.
********************************************************/
#include <iostream>
#include "MariaDBConnection.h++"

using namespace std;
using namespace sql;
using namespace StiltFox::StorageShed::Data;

namespace StiltFox::StorageShed
{
    MariaDBConnection::MariaDBConnection(const ConnectionInformation& connectionInformation)
    {
        this->connectionInformation = connectionInformation;
        this->connection = nullptr;
    }

    MariaDBConnection::MariaDBConnection(const MariaDBConnection& toCopy) : MariaDBConnection(toCopy.connectionInformation)
    {}

    bool MariaDBConnection::connect()
    {
        bool output = false;

        try
        {
            connection = DriverManager::getConnection(connectionInformation.getJDBCStringWithoutParameters(),
                connectionInformation.parameters);
            output = true;
        }
        catch (SQLException& e)
        {
            cerr << e.what() << endl;
            disconnect();
        }

        return output;
    }

    void MariaDBConnection::disconnect()
    {
        if (connection != nullptr)
        {
            connection->close();
            delete connection;
            connection = nullptr;
        }
    }

    Result<void*> MariaDBConnection::startTransaction()
    {
        Result<void*> output = {false, 0, "", {{"start transaction"}}, nullptr};

        if (isConnected())
        {
            output.connected = true;
            try
            {
                if (connection->getAutoCommit()) connection->setAutoCommit(false);
            }
            catch (SQLException& e)
            {
                output.errorText = e.what();
            }
        }

        return output;
    }

    Result<void*> MariaDBConnection::rollbackTransaction()
    {
        Result<void*> output = {false, 0, "", {{"rollback"}}, nullptr};

        if (isConnected())
        {
            output.connected = true;
            try
            {
                if (!connection->getAutoCommit())
                {
                    connection->rollback();
                    connection->setAutoCommit(true);
                }
            }
            catch (SQLException& e)
            {
                output.errorText = e.what();
            }
        }

        return output;
    }

    Result<void*> MariaDBConnection::commitTransaction()
    {
        Result<void*> output = {false, 0, "", {{"commit"}}, nullptr};

        if (isConnected())
        {
            output.connected = true;
            try
            {
                if (!connection->getAutoCommit())
                {
                    connection->commit();
                    connection->setAutoCommit(true);
                }
            }
            catch (SQLException& e)
            {
                output.errorText = e.what();
            }
        }

        return output;
    }

    Result<void*> MariaDBConnection::performUpdate(string statement)
    {
        return performUpdate(StructuredQuery{statement, {}});
    }

    Result<void*> MariaDBConnection::performUpdate(const StructuredQuery& statement)
    {
        const auto output = performQuery(statement);
        return {output.connected, output.rowsEffected, output.errorText, output.performedQueries, nullptr};
    }

    unordered_set<string> MariaDBConnection::validate(TableDefinitions tableDefinitions, bool strict)
    {
        return {};
    }

    Result<TableDefinitions> MariaDBConnection::getMetaData()
    {
        const string query =
            "select"
                " concat(TABLE_SCHEMA, '.', TABLE_NAME) as TABLE_NAME,"
                "COLUMN_NAME,"
                "COLUMN_TYPE"
            " from"
                " information_schema.COLUMNS"
            " where"
                " TABLE_SCHEMA not in ('information_schema', 'mysql', 'performance_schema');";
        TableDefinitions definitions = {};
        auto rawData = performQuery(query);

        for (const auto& row : rawData.data)
            definitions[row.at("TABLE_NAME")][row.at("COLUMN_NAME")] = row.at("COLUMN_TYPE");

        return {rawData.connected, rawData.rowsEffected, rawData.errorText, rawData.performedQueries,
            definitions};
    }

    Result<QueryReturnData> MariaDBConnection::performQuery(string query)
    {
        return performQuery(StructuredQuery {query, {}});
    }

    Result<QueryReturnData> MariaDBConnection::performQuery(StructuredQuery query)
    {
        Result<QueryReturnData> output = {false, 0, "", {query}, {}};

        if (isConnected())
        {
            output.connected = true;
            try
            {
                const unique_ptr<PreparedStatement> statement(connection->prepareStatement(query.query));
                int numParameters = statement->getParameterMetaData()->getParameterCount();

                for (int x=0; x<numParameters; x++)
                {
                    if (x<query.parameters.size())
                    {
                        statement->setString(x+1, query.parameters[x]);
                    }
                    else
                    {
                        statement->setNull(x+1, _NULL);
                    }
                }

                const unique_ptr<ResultSet> results(statement->executeQuery());

                while (results->next())
                {
                    const int columns = results->getMetaData()->getColumnCount();
                    output.data.emplace_back();
                    for (int z=0; z<columns; z++)
                    {
                        const string columnValue = results->getString(z+1).c_str();
                        output.data[output.data.size() - 1][results->getMetaData()->getColumnName(z+1).c_str()] =
                            columnValue;
                    }
                }
                output.rowsEffected = statement->getUpdateCount();
            }
            catch (SQLException& e)
            {
                output.errorText = e.what();
            }
        }

        return output;
    }

    Result<MultiTableData> MariaDBConnection::getAllData()
    {
        const string tableQuery = "select concat(TABLE_SCHEMA, '.', TABLE_NAME) as TABLE_NAME "
                                  "from information_schema.TABLES "
                                  "where TABLE_SCHEMA not in "
                                  "('information_schema', 'mysql', 'performance_schema', 'sys');";
        Result<MultiTableData> output = {false, 0, "", {}, {}};

        auto tables = performQuery(tableQuery);
        output.connected = tables.connected;
        if (tables.errorText.empty())
        {
            for (const auto& row : tables.data)
            {
                string selectQuery = "select * from " + row.at("TABLE_NAME") + ";";
                auto tableData = performQuery(selectQuery);

                output.data[row.at("TABLE_NAME")] = tableData.data;
                output.performedQueries.emplace_back(selectQuery);
                if (!output.errorText.empty()) break;
            }
        }

        return output;
    }


    bool MariaDBConnection::isConnected()
    {
        bool output = false;

        if (connection != nullptr) output = !connection->isClosed();

        return output;
    }

    string MariaDBConnection::getConnectionString()
    {
        return connectionInformation.toJDBCString();
    }

    MariaDBConnection::~MariaDBConnection()
    {
        MariaDBConnection::disconnect();
    }

    MariaDBConnection& MariaDBConnection::operator=(const ConnectionInformation& connectionInformation)
    {
        this->connectionInformation = connectionInformation;
        this->disconnect();
        return *this;
    }

    std::string MariaDBConnection::ConnectionInformation::toJDBCString() const
    {
        string connectionString = getJDBCStringWithoutParameters();

        if (!parameters.empty())
        {
            connectionString += "?";
            for (auto const& [name, value] : parameters)
            {
                if (!connectionString.ends_with('?')) connectionString += "&";
                connectionString += name + "=" + value;
            }
        }

        return connectionString;
    }

    std::string MariaDBConnection::ConnectionInformation::getJDBCStringWithoutParameters() const
    {
        return "jdbc:mariadb://" + hostName + ":" + to_string(portNumber) + "/" + databaseName;
    }
}