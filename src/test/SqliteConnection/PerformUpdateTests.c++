/*******************************************************
* Created by Cryos on 2/24/25.
* Copyright 2025 Stilt Fox® LLC
*
* See LICENSE on root project directory for terms
* of use.
********************************************************/
#include <sqlite3.h>
#include <gtest/gtest.h>
#include <Stilt_Fox/Scribe/TempFile.h++>
#include "SqliteConnection.h++"

using namespace std;
using namespace StiltFox::Scribe;
using namespace StiltFox::StorageShed;
using namespace StiltFox::StorageShed::Data;

namespace StiltFox::StorageShed::Tests::Sqlite_Connection::PerformUpdate
{
    SqliteConnection setupDatabase(File databasePath)
    {
        sqlite3* connection;
        sqlite3_stmt* statement;
        const string tableInfo = "create table if not exists test (id int primary key);";
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
                currentRow[column] = value;
            }

            returnData.emplace_back(currentRow);
        }

        return returnData;
    }

    TEST(SqliteConnection, performUpdate_will_return_connected_false_and_success_false_if_the_database_is_not_connected)
    {
        //given we have a database that we don't connect to
        const TemporaryFile database = ".sfdb_a0ffc6bfd57942588a549142668b36d8";
        SqliteConnection connection = database.getPath();

        //when we try to perform an update
        const auto actual = connection.performUpdate("create table test(id int primary key);");

        //then we get back a connected value of false and success false
        const Result<void*> expected = {
            false,
            false,
            "create table test(id int primary key);",
            nullptr
        };
        EXPECT_EQ(expected, actual);
    }

    TEST(SqliteConnection, performUpdate_will_return_connected_true_and_success_false_if_the_sql_cannot_be_executed)
    {
        //given we have a database that we connect to
        const TemporaryFile database = ".sfdb_e4d5962190dc4038a5e5ea6a4e3ddb9d";
        SqliteConnection connection = database.getPath();
        connection.connect();

        //when we try to perform a bad query
        const auto actual = connection.performUpdate("bad sql");

        //then we get back a connected value of true and a success of false
        const Result<void*> expected = {
            false,
            true,
            "bad sql",
            nullptr
        };
        EXPECT_EQ(expected, actual);
    }

    TEST(SqliteConnection, performUpdate_will_never_return_any_data_even_if_the_query_does)
    {
        //given we have a database with some data and we connect to it
        const TemporaryFile database = ".sfdb_69d08a8f2adc43efbf610bb2a762bd48";
        SqliteConnection connection = setupDatabase(database.getPath());
        connection.connect();

        //when we perform an update that should return data
        const auto actual = connection.performUpdate("select * from test;");

        //then we get back a successful query with no data
        const Result<void*> expected =
        {
            true,
            true,
            "select * from test;",
            nullptr
        };
        EXPECT_EQ(expected, actual);
    }

    TEST(SqliteConnection, performUpdate_will_perform_the_passed_in_sql_statement)
    {
        //given we have a database and connect to it
        const TemporaryFile database = ".sfdb_7a3e4273dbb44ac48915833b13a26cad";
        SqliteConnection connection = setupDatabase(database.getPath());
        connection.connect();

        //when we perform an update
        const auto actual = connection.performUpdate("insert into test(id) values (1);");

        //then the update is performed and the data is saved
        const Result<void*> expected = {
            true,
            true,
            "insert into test(id) values (1);",
            nullptr
        };
        const QueryReturnData expectedData = {
            {
                {"id", "3"}
            },
            {
                {"id", "1"}
            }
        };
        EXPECT_EQ(expected, actual);
        EXPECT_EQ(expectedData, testProcedure(database.getPath()));
    }

    TEST(SqliteConnection, performUpdate_will_return_the_expanded_performedQuery_statement_when_a_StructuredQuery_is_passed_in)
    {
        //given we have a database and a structured query
        const TemporaryFile database = ".sfdb_81e8653c16b24d1b91392883a9430ba3";
        const StructuredQuery structuredQuery = {"insert into test(id) values (?)", {"5"}};
        SqliteConnection connection = setupDatabase(database.getPath());
        connection.connect();

        //when we pass the structured query to perform update
        const auto actual = connection.performUpdate(structuredQuery);

        //then the expanded sql statement is returned
        const Result<void*> expected =
        {
            true,
            true,
            "insert into test(id) values ('5')",
            nullptr
        };
        EXPECT_EQ(expected, actual);
    }

    TEST(SqliteConnection, performUpdate_will_fill_in_blank_values_if_no_parameters_are_provided_in_StructuredQuery)
    {

    }
}