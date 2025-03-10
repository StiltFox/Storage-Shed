/*******************************************************
* Created by cryos on 3/8/25.
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
using namespace StiltFox::StorageShed::Data;

namespace StiltFox::StorageShed::Tests::Sqlite_Connection::Validate
{
    SqliteConnection setupDatabase(File databasePath)
    {
        sqlite3* connection;
        sqlite3_stmt* statement;
        const string testTable = "create table if not exists test (id int primary key, description varchar(225));";
        const string employeeTable = "create table if not exists employee (name varchar(10), last varchar(10), age int);";
        const string companyTable = "create table if not exists company (name varchar(20), city varchar(10));";

        sqlite3_open(databasePath.getPath().c_str(), &connection);

        sqlite3_prepare(connection, testTable.c_str(), testTable.length(), &statement, nullptr);
        sqlite3_step(statement);
        sqlite3_reset(statement);
        sqlite3_prepare(connection, employeeTable.c_str(), employeeTable.length(), &statement, nullptr);
        sqlite3_step(statement);
        sqlite3_reset(statement);
        sqlite3_prepare(connection, companyTable.c_str(), companyTable.length(), &statement, nullptr);
        sqlite3_step(statement);
        sqlite3_finalize(statement);

        sqlite3_close(connection);

        return databasePath.getPath();
    }

    TEST(SqliteConnection, Validate_will_return_no_errors_when_strict_is_true_and_the_expected_database_structure_is_correct)
    {
        //given we have a database with a structure that matches the expected table definitions
        const TemporaryFile database = ".sfdb_873bad6450bf412fbd3a7aaabb5e41df";
        SqliteConnection connection = setupDatabase(database.getPath());
        connection.connect();
        TableDefinitions expectedStructure =
        {
            {
                "test",
                {
                    {"id", "int"},
                    {"description", "varchar(225)"}
                }
            },
            {
                "employee",
                {
                    {"name", "varchar(10)"},
                    {"last", "varchar(10)"},
                    {"age", "int"}
                }
            },
            {
                "company",
                {
                    {"name", "varchar(20)"},
                    {"city", "varchar(10)"}
                }
            }
        };

        //when we check the structure of the database
        auto actual = connection.validate(expectedStructure, true);

        //then we get back no errors
        const unordered_set<string> expected = {};
        EXPECT_EQ(expected, actual);
    }
}