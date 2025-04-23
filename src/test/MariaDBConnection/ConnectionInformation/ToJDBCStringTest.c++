/*******************************************************
* Created by Cryos on 3/30/25.
* Copyright 2025 Stilt Fox® LLC
*
* See LICENSE on root project directory for terms
* of use.
********************************************************/
#include <gtest/gtest.h>
#include "MariaDBConnection.h++"

using namespace std;
using namespace StiltFox::StorageShed;

namespace StiltFox::StorageShed::Tests::MariaDB_Connection::Connection_Information::toJDBCString
{
    TEST(MariaDBConnection_ConnectionInformation, toJDBCString_will_return_a_connection_string)
    {
        //given we have a connection information object with all fields filled in
        MariaDBConnection::ConnectionInformation connectionInformation =
            {
            "stiltfox.net",
            "testDB",
            {
                    {"username", "test"},
                    {"password", "chickens"}
                },
            8080
            };

        //when we convert the object to a JDBC String
        auto actual = connectionInformation.toJDBCString();

        //then we get back a valid JDBC String
        EXPECT_EQ("jdbc:mariadb://stiltfox.net:8080/testDB?password=chickens&username=test", actual);
    }

    TEST(MariaDBConnection_ConnectionInformation, toJDBCString_will_default_to_port_3306_if_no_port_is_provided)
    {
        //given we have a connection information object with no port number
        MariaDBConnection::ConnectionInformation connectionInformation =
        {
            "stiltfox.net",
            "testDB",
            {
                        {"username", "test"},
                        {"password", "chickens"}
            }
        };

        //when we convert the object to a JDBC string
        auto actual = connectionInformation.toJDBCString();

        //then we get back a valid JDBC string
        EXPECT_EQ("jdbc:mariadb://stiltfox.net:3306/testDB?password=chickens&username=test", actual);
    }

    TEST(MariaDBConnection_ConnectionInformation, toJDBCString_will_default_to_localhost_if_a_hostname_is_not_provided)
    {
        //given we have a new connection information object with no port number
        MariaDBConnection::ConnectionInformation connectionInformation;
        connectionInformation.databaseName = "testDB";
        connectionInformation.portNumber = 50;

        //when we convert the object to a JDBC string
        auto actual = connectionInformation.toJDBCString();

        //then we get back a valid JDBC string
        EXPECT_EQ("jdbc:mariadb://localhost:50/testDB", actual);
    }
}