/*******************************************************
* Created by cryos on 4/23/25.
* Copyright 2024 Stilt Fox® LLC
*
* See LICENSE on root project directory for terms
* of use.
********************************************************/
#ifndef Stilt_Fox_be37c7f7a63d472ca7dbd8ca57d1edac
#define Stilt_Fox_be37c7f7a63d472ca7dbd8ca57d1edac
#include "MariaDBConnection.h++"

namespace StiltFox::StorageShed::Tests::MariaDB_Connection
{
    inline MariaDBConnection::ConnectionInformation getConnectionInformationFromEnvironment()
    {
        MariaDBConnection::ConnectionInformation output;
        const char* hostName = getenv("STILT_FOX_MARIADB_TESTS_HOSTNAME");
        const char* databaseName = getenv("STILT_FOX_MARIADB_TESTS_DATABASE");
        const char* portNumber = getenv("STILT_FOX_MARIADB_TESTS_PORT");
        const char* user = getenv("STILT_FOX_MARIADB_TESTS_USER");
        const char* password = getenv("STILT_FOX_MARIADB_TESTS_PASSWORD");

        if (hostName != nullptr) output.hostName = hostName;
        if (databaseName != nullptr) output.databaseName = databaseName;
        if (portNumber != nullptr) output.portNumber = std::stoi(portNumber);
        output.parameters["user"] = user == nullptr ? "root" : user;
        if (password != nullptr) output.parameters["password"] = password;

        return output;
    }

    inline void clearDatabase()
    {
        try
        {
            MariaDBConnection::ConnectionInformation connectionInformation = getConnectionInformationFromEnvironment();
            const std::unique_ptr<sql::Connection> connection(sql::DriverManager::getConnection(connectionInformation.getJDBCStringWithoutParameters(),
                connectionInformation.parameters));
            const std::unique_ptr<sql::PreparedStatement> getDatabases(connection->prepareStatement("show databases;"));
            const std::unique_ptr<sql::ResultSet> databases(getDatabases->executeQuery());

            std::vector<std::string> databaseNames;
            while (databases->next()) databaseNames.emplace_back(databases->getString(1).c_str());
            for (const auto& dbName : databaseNames)
            {
                if (dbName != "information_schema")
                {
                    const std::unique_ptr<sql::PreparedStatement> deleteStatement(connection->prepareStatement("drop database " + dbName + ";"));
                    deleteStatement->execute();
                }
            }
        }
        catch (sql::SQLException& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    inline void generateTablesWithSomeData()
    {
        MariaDBConnection::ConnectionInformation connectionInformation = getConnectionInformationFromEnvironment();

        try
        {
            const std::unique_ptr<sql::Connection> connection(sql::DriverManager::getConnection(connectionInformation.getJDBCStringWithoutParameters(),
                connectionInformation.parameters));
            const std::unique_ptr<sql::PreparedStatement> schemaCreate(connection->prepareStatement("create schema test;"));
            const std::unique_ptr<sql::PreparedStatement> schemaCreate1(connection->prepareStatement("create schema test2;"));
            const std::unique_ptr<sql::PreparedStatement> tableCreate(connection->prepareStatement("create table test.table1 (id int, name varchar(255), dead boolean);"));
            const std::unique_ptr<sql::PreparedStatement> tableCreate1(connection->prepareStatement("create table test.table2 (id_1 int, id_2 int);"));
            const std::unique_ptr<sql::PreparedStatement> tableCreate2(connection->prepareStatement("create table test2.information (id uuid);"));
            const std::unique_ptr<sql::PreparedStatement> insert(connection->prepareStatement("insert into test.table1 (id, name, dead) values (1, 'bagel', false);"));
            const std::unique_ptr<sql::PreparedStatement> insert2(connection->prepareStatement("insert into test.table1 (id, name, dead) values (2, 'fork', true);"));
            const std::unique_ptr<sql::PreparedStatement> insert3(connection->prepareStatement("insert into test.table1 (id, name, dead) values (3, 'pickle', false);"));
            const std::unique_ptr<sql::PreparedStatement> insert4(connection->prepareStatement("insert into test2.information (id) values ('c5ab7ae9521c406ba0e253f8b45b3ea6');"));

            schemaCreate->executeQuery();
            schemaCreate1->executeQuery();
            tableCreate->executeQuery();
            tableCreate1->executeQuery();
            tableCreate2->executeQuery();
            insert->executeQuery();
            insert2->executeQuery();
            insert3->executeQuery();
            insert4->executeQuery();

            connection->close();
        }
        catch (sql::SQLException& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
}
#endif