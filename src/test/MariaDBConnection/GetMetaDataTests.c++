/*******************************************************
* Created by Cryos on 4/25/25.
* Copyright 2025 Stilt Fox® LLC
*
* See LICENSE on root project directory for terms
* of use.
********************************************************/
#include <gtest/gtest.h>
#include "PrintHelper.h++"
#include "TestHelpFunctions.h++"

using namespace std;
using namespace sql;
using namespace StiltFox::StorageShed::Data;

namespace StiltFox::StorageShed::Tests::MariaDB_Connection::GetMetaData
{
    TEST(getMetaData, will_return_success_false_and_connected_false_if_the_database_is_not_connected)
    {
        //given we do not connect to the database
        const MariaDBConnection::ConnectionInformation connectionInformation = getConnectionInformationFromEnvironment();
        MariaDBConnection connection(connectionInformation);

        //when we try to get the metadata
        const auto actual = connection.getMetaData();

        //then we get back that we are not connected and the operation was not a success
        const Result<TableDefinitions> expected = {
            false,
            false,
            "select concat(TABLE_SCHEMA, '.', TABLE_NAME) as TABLE_NAME,COLUMN_NAME,COLUMN_TYPE from information_schema.COLUMNS where TABLE_SCHEMA not in ('information_schema', 'mysql', 'performance_schema');",
            {}
        };
        EXPECT_EQ(expected, actual);
    }

    TEST(getMetaData, gets_the_metadata_from_a_database)
    {
        //given we have a database to query
        generateTablesWithSomeData();
        const MariaDBConnection::ConnectionInformation connectionInformation = getConnectionInformationFromEnvironment();
        MariaDBConnection connection(connectionInformation);
        connection.connect();

        //when we try to get the metadata
        const auto actual = connection.getMetaData();

        //then we get back a map of all the tables and columns
        const Result<TableDefinitions> expected =
            {
                true,
                true,
            "select concat(TABLE_SCHEMA, '.', TABLE_NAME) as TABLE_NAME,COLUMN_NAME,COLUMN_TYPE from information_schema.COLUMNS where TABLE_SCHEMA not in ('information_schema', 'mysql', 'performance_schema');",
                {
                    {"test.table1",{{"id", "int(11)"}, {"name", "varchar(255)"}, {"dead", "tinyint(1)"}}},
                    {"test.table2",{{"id_1", "int(11)"}, {"id_2", "int(11)"}}},
                    {"test2.information",{{"id","uuid"}}}
                }
            };
        EXPECT_EQ(expected, actual);
        clearDatabase();
    }
}