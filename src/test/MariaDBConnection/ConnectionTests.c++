/*******************************************************
* Created by Cryos on 3/31/25.
* Copyright 2025 Stilt Fox® LLC
*
* See LICENSE on root project directory for terms
* of use.
********************************************************/
#include <cstdlib>
#include <gtest/gtest.h>
#include "MariaDBConnection.h++"

using namespace std;

namespace StiltFox::StorageShed::Tests::MariaDB_Connection::Connection
{
    MariaDBConnection::ConnectionInformation getConnectionInformationFromEnvironment()
    {
        MariaDBConnection::ConnectionInformation output;
        const char* hostName = getenv("STILT_FOX_MARIADB_TESTS_HOSTNAME");
        const char* databaseName = getenv("STILT_FOX_MARIADB_TESTS_DATABASE");
        const char* portNumber = getenv("STILT_FOX_MARIADB_TESTS_PORT");
        const char* user = getenv("STILT_FOX_MARIADB_TESTS_USER");
        const char* password = getenv("STILT_FOX_MARIADB_TESTS_PASSWORD");

        if (hostName != nullptr) output.hostName = hostName;
        if (databaseName != nullptr) output.databaseName = databaseName;
        if (portNumber != nullptr) output.portNumber = stoi(portNumber);
        output.parameters["user"] = user == nullptr ? "root" : user;
        if (password != nullptr) output.parameters["password"] = password;

        return output;
    }

    TEST(Connection, will_return_false_and_not_connect_if_the_database_does_not_exist)
    {
        //given we have invalid connection information
        MariaDBConnection::ConnectionInformation connectionInformation {"bad host", "not a db", {}, 4444};
        MariaDBConnection connection = connectionInformation;

        //when we try to connect
        const bool actual = connection.connect();

        //then we did not connect
        EXPECT_FALSE(actual);
        EXPECT_FALSE(connection.isConnected());
    }

    TEST(Connection, will_return_true_if_all_information_is_correct)
    {
        //given we have accurate connection information
        MariaDBConnection::ConnectionInformation connectionInformation = getConnectionInformationFromEnvironment();
        MariaDBConnection connection = connectionInformation;

        //when we try to connect
        const bool actual = connection.connect();

        //then we connected to the database
        EXPECT_TRUE(actual);
        EXPECT_TRUE(connection.isConnected());
    }
}