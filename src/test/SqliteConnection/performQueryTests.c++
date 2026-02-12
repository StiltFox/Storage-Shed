/*******************************************************
* Created by Cryos on 3/23/25.
* Copyright 2025 Stilt Fox® LLC
*
* See LICENSE on root project directory for terms
* of use.
********************************************************/
#include <sqlite3.h>
#include <gtest/gtest.h>
#include <Stilt_Fox/Scribe/TempFile.h++>
#include "PrintHelper.h++"
#include "SqliteConnection.h++"

using namespace std;
using namespace StiltFox::Scribe;
using namespace StiltFox::StorageShed;
using namespace StiltFox::StorageShed::Data;

namespace StiltFox::StorageShed::Test::Sqlite_Connection::PerformQuery
{
    SqliteConnection setupDatabase(const File& databasePath)
    {
        sqlite3* connection;
        sqlite3_stmt* statement;
        //the value field is never used, but it exists to make sure that the system can handle null strings
        const string tableInfo = "create table if not exists test (id int primary key, value varchar(255));";
        const string row = "insert into test (id) values (3)";

        sqlite3_open(databasePath.getPath().c_str(), &connection);

        sqlite3_prepare(connection, tableInfo.c_str(), tableInfo.length(), &statement, nullptr);
        sqlite3_step(statement);
        sqlite3_reset(statement);
        sqlite3_prepare(connection, row.c_str(), row.length(), &statement, nullptr);
        sqlite3_step(statement);
        sqlite3_finalize(statement);

        sqlite3_close(connection);

        return databasePath.getPath();
    }

    QueryReturnData testProcedure(File DatabasePath)
    {
        sqlite3* connection;
        sqlite3_stmt* statement;
        const string retrieveData = "select * from test;";
        QueryReturnData returnData;

        sqlite3_open(DatabasePath.getPath().c_str(), &connection);
        sqlite3_prepare(connection, retrieveData.c_str(), retrieveData.length(), &statement, nullptr);

        while (sqlite3_step(statement) == SQLITE_ROW)
        {
            Row currentRow;
            int columnCount = sqlite3_data_count(statement);
            for (int i = 0; i < columnCount; i++)
            {
                const char* value = (char*)sqlite3_column_text(statement, i);
                const char* column = (char*)sqlite3_column_name(statement, i);
                currentRow[column] = value == nullptr ? "" : value;
            }

            returnData.emplace_back(currentRow);
        }

        return returnData;
    }

    TEST(performQuery, will_return_connected_false_if_the_database_is_not_connected)
    {
        //given we have a database that we don't connect to
        const TemporaryFile database = ".sfdb_e5192e7333344a69855aa4ca7fb59b8a";
        SqliteConnection connection = database.getPath();

        //when we try to perform a query
        const auto actual = connection.performQuery("create table test(id int primary key);");

        //then we get back a connected value of false
        const Result<QueryReturnData> expected = {
            false,
            "",
            {{"create table test(id int primary key);"}},
            {}
        };
        EXPECT_EQ(expected, actual);
    }

    TEST(performQuery, will_return_connected_true_and_an_error_if_the_sql_cannot_be_executed)
    {
        //given we have a database that we connect to
        const TemporaryFile database = ".sfdb_efeb47c5b9104479ae9c979bb3be322f";
        SqliteConnection connection = database.getPath();
        connection.connect();

        //when we try to perform a bad query
        const auto actual = connection.performQuery("bad sql");

        //then we get back a connected value of true and an error
        const Result<QueryReturnData> expected = {
            true,
            "near \"bad\": syntax error",
            {{"bad sql"}},
            {}
        };
        EXPECT_EQ(expected, actual);
    }

    TEST(performQuery, will_return_any_data_that_the_query_does)
    {
        //given we have a database with some data and we connect to it
        const TemporaryFile database = ".sfdb_c14e6742cb3048e998d401415d49352d";
        SqliteConnection connection = setupDatabase(database.getPath());
        connection.connect();

        //when we perform a query that should return data
        const auto actual = connection.performQuery("select * from test;");

        //then we get back the appropriate data
        const Result<QueryReturnData> expected =
        {
            true,
            "",
            {{"select * from test;"}},
            {
                {
                    {"id", "3"},
                    {"value", ""}
                }
            }
        };
        EXPECT_EQ(expected, actual);
    }

    TEST(performQuery, will_perform_the_passed_in_sql_statement)
    {
        //given we have a database and connect to it
        const TemporaryFile database = ".sfdb_bd32604208864952a0a8e8b725623a2f";
        SqliteConnection connection = setupDatabase(database.getPath());
        connection.connect();

        //when we perform a query
        const auto actual = connection.performQuery("insert into test(id) values (1);");

        //then the query is performed and the data is saved
         const Result<QueryReturnData> expected = {
             true,
             "",
             {{"insert into test(id) values (1);"}},
             {}
         };
         const QueryReturnData expectedData = {
             {
                 {"id", "3"},
                 {"value", ""}
             },
             {
                     {"id", "1"},
                 {"value", ""}
             }
         };
         EXPECT_EQ(expected, actual);
         EXPECT_EQ(expectedData, testProcedure(database.getPath()));
    }

    TEST(performQuery, will_return_the_expanded_performedQuery_statement_when_a_StructuredQuery_is_passed_in)
    {
        //given we have a database and a structured query
        const TemporaryFile database = ".sfdb_c4b20ed52442434fbf28b424677eb4de";
        const StructuredQuery structuredQuery = {"insert into test(id) values (?)", {"5"}};
        SqliteConnection connection = setupDatabase(database.getPath());
        connection.connect();

        //when we pass the structured query to performQuery
        const auto actual = connection.performQuery(structuredQuery);

        //then the expanded sql statement is returned
        const Result<QueryReturnData> expected =
        {
            true,
            "",
            {structuredQuery},
            {}
        };
        EXPECT_EQ(expected, actual);
    }

    TEST(performQuery, will_fill_in_null_values_if_no_parameters_are_provided_in_StructuredQuery)
    {
        //given we have a database and a structured query with no parameters
        const TemporaryFile database = ".sfdb_28f9f88195f64086b902cbf122c904ce";
        const StructuredQuery structuredQuery = {"insert into test(id) values (?)", {}};
        SqliteConnection connection = setupDatabase(database.getPath());
        connection.connect();

        //when we perform the query
        const auto actual = connection.performQuery(structuredQuery);

        //then we get back that the passed in value is null
        const Result<QueryReturnData> expected =
        {
            true,
            "",
            {structuredQuery},
            {}
        };
        EXPECT_EQ(expected, actual);
    }

    TEST(performQuery, will_ignore_extra_parameters_in_StructuredQuery)
    {
        //given we have a database and a structured query with too many parrameters
        const TemporaryFile database = ".sfdb_8541163aa8c242d9b66af559e7b2d8f3";
        const StructuredQuery structuredQuery = {"insert into test(id) values (?)", {"5", "10"}};
        SqliteConnection connection = setupDatabase(database.getPath());
        connection.connect();

        //when we perform the query
        const auto actual = connection.performQuery(structuredQuery);

        //then we get back that the query was a success and only the needed values are used
        const Result<QueryReturnData> expected =
        {
            true,
            "",
            {structuredQuery},
            {}
        };
        EXPECT_EQ(expected, actual);
    }

    TEST(performQuery, will_return_connected_false_if_the_database_is_not_connected_and_a_StructuredQuery_is_passed_in)
    {
        //given we have a disconnected database and a structured query
        const TemporaryFile database = ".sfdb_61078cfab7804a2c859d04d1428f1930";
        const StructuredQuery structuredQuery = {"insert into test(id) values (?)", {"5"}};
        SqliteConnection connection = setupDatabase(database.getPath());

        //when we perform the query
        const auto actual = connection.performQuery(structuredQuery);

        //then we get back a connected value of false
        const Result<QueryReturnData> expected =
        {
            false,
            "",
            {structuredQuery},
            {}
        };
        EXPECT_EQ(expected, actual);
    }

    TEST(performQuery, will_return_connected_true_and_an_error_when_a_bad_sql_statement_is_passed_via_StructruedQuery)
    {
        //given we have a connected database and the sql is bad
        const TemporaryFile database = ".sfdb_32b0ae5d48474da19de5f263b2b58c01";
        const StructuredQuery structuredQuery = {"bad sql", {"5"}};
        SqliteConnection connection = setupDatabase(database.getPath());
        connection.connect();

        //when we perform the query
        const auto actual = connection.performQuery(structuredQuery);

        //then we get back a connected of true and an error
        const Result<QueryReturnData> expected =
        {
            true,
            "near \"bad\": syntax error",
            {structuredQuery},
            {}
        };
        EXPECT_EQ(expected, actual);
    }

    TEST(performQuery, will_perform_the_passed_in_StructuredQuery)
    {
        //given we have a database and connect to it
        const TemporaryFile database = ".sfdb_2708c8f7ad614ccc8513eca4f7721101";
        const StructuredQuery structuredQuery = {"insert into test(id) values (?);", {"1"}};
        SqliteConnection connection = setupDatabase(database.getPath());
        connection.connect();

        //when we perform a query
        const auto actual = connection.performQuery(structuredQuery);

        //then the update is performed and the data is saved
        const Result<QueryReturnData> expected =
        {
            true,
            "",
            {structuredQuery},
            {}
        };
        const QueryReturnData expectedData =
        {
            {
                {"id", "3"},
                {"value", ""}
            },
            {
                    {"id", "1"},
                {"value", ""}
            }
        };
        EXPECT_EQ(expected, actual);
        EXPECT_EQ(expectedData, testProcedure(database.getPath()));
    }

    TEST(performQuery, will_return_any_data_that_the_StructuredQuery_does)
    {
        //given we have a database and connect to it
        const TemporaryFile database = ".sfdb_7becc00d02ba4657843ede23c28384fb";
        const StructuredQuery structuredQuery = {"select * from test where id = ?", {"3"}};
        SqliteConnection connection = setupDatabase(database.getPath());
        connection.connect();

        //when we perform the update
        const auto actual = connection.performQuery(structuredQuery);

        //then the update is performed but no data is returned
        const Result<QueryReturnData> expected =
        {
            true,
            "",
            {structuredQuery},
            {
                {
                    {"id", "3"},
                    {"value", ""}
                }
            }
        };
        EXPECT_EQ(expected, actual);
    }

    TEST(performQuery, will_be_able_to_return_all_data_in_a_column_even_if_a_null_character_is_included_in_the_middle)
    {
        //given we have a database and we insert a value that has a null character in the middle of it
        string hexValue = "df00453a";
        vector<unsigned char> rawBinary  = {0xdf, 0x00, 0x45, 0x3a};
        string stringBinary(rawBinary.begin(), rawBinary.end());
        const TemporaryFile database = ".sfdb_3e1d79bea80c4c0d98c626cb23fa0f8a";
        SqliteConnection connection = setupDatabase(database.getPath());
        connection.connect();
        connection.performQuery("insert into test (id, value) values (1, x'" + hexValue + "');");

        //when we try to get the data back from the database
        const auto& actual = connection.performQuery("select * from test;");

        //then the value is read the full way through
        const Result<QueryReturnData> expected =
        {
            true,
            "",
            {{"select * from test;",{}}},
            {
                {
                    {"id", "3"},
                    {"value", ""}
                },
                {
                    {"id","1"},
                    {"value",stringBinary}
                }
            }
        };

        EXPECT_EQ(actual, expected);
    }
}