/*******************************************************
* Created by Cryos on 4/28/25.
* Copyright 2025 Stilt Fox® LLC
*
* See LICENSE on root project directory for terms
* of use.
********************************************************/
#include <gtest/gtest.h>
#include "PrintHelper.h++"
#include "TestHelpFunctions.h++"

using namespace StiltFox::StorageShed::Data;

namespace StiltFox::StorageShed::Tests::MariaDB_Connection::PerformQuery
{
    class performQuery : public ::testing::Test
    {
    protected:
        MariaDBConnection connectionInformation = getConnectionInformationFromEnvironment();

        void SetUp() override
        {
            generateTablesWithSomeData();
        }

        void TearDown() override
        {
            clearDatabase();
        }
    };

    TEST_F(performQuery, will_return_connected_false_and_success_false_if_the_database_is_not_connected)
    {
        //given we have a database that we are not connected to
        MariaDBConnection connection = connectionInformation;

        //when we try to perform a query
        auto actual = connection.performQuery("select * from test.table1");

        //then we get back a connected value of false and a success value of false
        const Result<QueryReturnData> expected =
        {
            false,
            false,
            "select * from test.table1",
            {}
        };
        ASSERT_EQ(expected, actual);
    }

    TEST_F(performQuery, will_return_any_data_that_the_query_does)
    {
        //given we have a database with some data and we connect to it
        MariaDBConnection connection = connectionInformation;
        connection.connect();

        //when we perform a query that should return data
        const auto actual = connection.performQuery("select * from test.table1");

        //then we get back a successful query with the appropriate data
        const Result<QueryReturnData> expected =
        {
            true,
            true,
            {"select * from test.table1"},
            {
                {{"id","1"},{"name","bagel"}, {"dead","0"}},
                {{"id","2"},{"name","fork"}, {"dead","1"}},
                {{"id","3"},{"name","pickle"}, {"dead","0"}}
            }
        };
        EXPECT_EQ(expected, actual);
    }

    TEST_F(performQuery, will_perform_the_passed_in_sql_statement)
    {
        //given we have a database and connect to it
        MariaDBConnection connection = connectionInformation;
        connection.connect();

        //when we perform a query
        const auto actual = connection.performQuery("insert into test.table1 (id, name, dead) values (4, 'apple', 0)");
        const auto tableData = connection.performQuery("select * from test.table1");

        //then the query is performed and the data is inserted
        const Result<QueryReturnData> expected = {
            true,
            true,
            {"insert into test.table1 (id, name, dead) values (4, 'apple', 0)"},
            {}
        };
        const Result<QueryReturnData> expectedData =
{
            true,
            true,
            {"select * from test.table1", {}},
            {
                {{"id","1"},{"name","bagel"}, {"dead","0"}},
                {{"id","2"},{"name","fork"}, {"dead","1"}},
                {{"id","3"},{"name","pickle"}, {"dead","0"}},
                {{"id", "4"},{"name","apple"},{"dead","0"}}
            }
};
        EXPECT_EQ(expected, actual);
        EXPECT_EQ(expectedData, tableData);
    }

    TEST_F(performQuery, will_return_the_expanded_performedQuery_statement_when_a_StructuredQuery_is_passed_in)
    {
        //given we have a database and a structured query
        const StructuredQuery structuredQuery = {"insert into test.table1 (id, name, dead) values (?, ?, ?)", {"5", "cucumber", "true"}};
        MariaDBConnection connection = connectionInformation;
        connection.connect();

        //when we pass the structured query in
        const auto actual = connection.performQuery(structuredQuery);

        //then we get back the expanded sql statement
        const Result<QueryReturnData> expected =
        {
            true,
            true,
            {"insert into test.table1 (id, name, dead) values ('5', 'cucumber', '1')"},
            {}
        };
        EXPECT_EQ(expected, actual);
    }

    TEST_F(performQuery, will_fill_in_null_values_if_no_parameters_are_provided_in_StructuredQuery)
    {
        //given we have a database and a structured query with no parameters
        const StructuredQuery structuredQuery = {"insert into test.table1 (id, name, dead) values (?, ?, ?)", {}};
        MariaDBConnection connection = connectionInformation;
        connection.connect();

        //when we try to perform the query
        const auto actual = connection.performQuery(structuredQuery);

        //then we get back that the passed in values are null
        const Result<QueryReturnData> expected =
        {
            false,
            true,
            {"insert into test.table1 (id, name, dead) values (NULL, NULL, NULL)"},
            {}
        };
        EXPECT_EQ(expected, actual);
    }
}