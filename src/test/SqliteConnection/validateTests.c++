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
        const string employeeTable =
            "create table if not exists employee (name varchar(10), last varchar(10), age int);";
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

    TEST(validate, will_return_no_errors_with_strict_mode_enabled_and_the_expected_database_structure_is_correct)
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
                    {"id", "INT"},
                    {"description", "varchar(225)"}
                }
            },
            {
                "employee",
                {
                    {"name", "varchar(10)"},
                    {"last", "varchar(10)"},
                    {"age", "INT"}
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

    TEST(validate, will_return_no_errors_with_strict_mode_disabled_and_the_expected_database_structure_is_correct)
    {
        //given we have a database with a structure that matches the expected table definitions
        const TemporaryFile database = ".sfdb_99f3cb96b5c74ac1a83dd07ca7d888ae";
        SqliteConnection connection = setupDatabase(database.getPath());
        connection.connect();
        TableDefinitions expectedStructure =
        {
            {
                "test",
                {
                    {"id", "INT"},
                    {"description", "varchar(225)"}
                }
            },
            {
                "employee",
                {
                    {"name", "varchar(10)"},
                    {"last", "varchar(10)"},
                    {"age", "INT"}
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
        auto actual = connection.validate(expectedStructure, false);

        //then we get back no errors
        const unordered_set<string> expected = {};
        EXPECT_EQ(expected, actual);
    }

    TEST(validate, will_detect_excess_columns_in_tables_when_strict_mode_is_enabled)
    {
        //given we have a database with an extra column
        const TemporaryFile database = ".sfdb_2dee3f7e1f574563b0abd623ccace108";
        SqliteConnection connection = setupDatabase(database.getPath());
        connection.connect();
        TableDefinitions expectedStructure =
        {
            {
                "test",
                {
                    {"id", "INT"},
                    {"description", "varchar(225)"}
                }
            },
            {
                "employee",
                {
                    {"name", "varchar(10)"},
                    {"last", "varchar(10)"},
                    {"age", "INT"}
                }
            },
            {
                "company",
                {
                    {"name", "varchar(20)"}
                }
            }
        };

        //when we check the structure of the database
        auto actual = connection.validate(expectedStructure, true);

        //then we get back that there is an unwanted column
        const unordered_set<string> expected = {"Unwanted column city in table company"};
        EXPECT_EQ(expected, actual);
    }

    TEST(validate, will_not_detect_excess_columns_in_tables_when_strict_mode_is_disabled)
    {
        //given we have a database with an extra column
        const TemporaryFile database = ".sfdb_63be1d23b530462e9b012f3cd5f8c402";
        SqliteConnection connection = setupDatabase(database.getPath());
        connection.connect();
        TableDefinitions expectedStructure =
        {
            {
                "test",
                {
                    {"id", "INT"},
                    {"description", "varchar(225)"}
                }
            },
            {
                "employee",
                {
                    {"name", "varchar(10)"},
                    {"last", "varchar(10)"},
                    {"age", "INT"}
                }
            },
            {
                "company",
                {
                    {"name", "varchar(20)"}
                }
            }
        };

        //when we check the structure of the database
        auto actual = connection.validate(expectedStructure, false);

        //then we get back no errors
        const unordered_set<string> expected = {};
        EXPECT_EQ(expected, actual);
    }

    TEST(validate, will_detect_an_extra_table_when_strict_mode_is_enabled)
    {
        //given we have a database with an extra table
        const TemporaryFile database = ".sfdb_217414a8b77348a0bc19f7c2952a8ca2";
        SqliteConnection connection = setupDatabase(database.getPath());
        connection.connect();
        TableDefinitions expectedStructure =
        {
            {
                "test",
                {
                    {"id", "INT"},
                    {"description", "varchar(225)"}
                }
            },
            {
                "employee",
                {
                    {"name", "varchar(10)"},
                    {"last", "varchar(10)"},
                    {"age", "INT"}
                }
            }
        };

        //when we check the structure of the database
        auto actual = connection.validate(expectedStructure, true);

        //then we get back that an unwanted table is present
        const unordered_set<string> expected = {"Unwanted table company"};
        EXPECT_EQ(expected, actual);
    }

    TEST(validate, will_not_detect_an_extra_table_when_strict_mode_is_disabled)
    {
        //given we have a database with an extra table
        const TemporaryFile database = ".sfdb_24de43c5198146c99ad5ba962cfd8f45";
        SqliteConnection connection = setupDatabase(database.getPath());
        connection.connect();
        TableDefinitions expectedStructure =
        {
            {
                "test",
                {
                    {"id", "INT"},
                    {"description", "varchar(225)"}
                }
            },
            {
                "employee",
                {
                    {"name", "varchar(10)"},
                    {"last", "varchar(10)"},
                    {"age", "INT"}
                }
            }
        };

        //when we check the structure of the database
        auto actual = connection.validate(expectedStructure, false);

        //then we get back no errors
        const unordered_set<string> expected = {};
        EXPECT_EQ(expected, actual);
    }

    TEST(validate, will_detect_a_missing_table_that_was_expected_regardless_of_strict_mode)
    {
        //given we have a database that's missing an expected table
        const TemporaryFile database = ".sfdb_24de43c5198146c99ad5ba962cfd8f45";
        SqliteConnection connection = setupDatabase(database.getPath());
        connection.connect();
        TableDefinitions expectedStructure =
        {
            {
                "test",
                {
                        {"id", "INT"},
                        {"description", "varchar(225)"}
                }
            },
            {
                "employee",
                {
                        {"name", "varchar(10)"},
                        {"last", "varchar(10)"},
                        {"age", "INT"}
                }
            },
            {
                "company",
                {
                        {"name", "varchar(20)"},
                        {"city", "varchar(10)"}
                }
            },
            {
                "extra_table",
                {}
            }
        };

        //when we check the structure of the database in both strict and non-strict mode
        const auto strict = connection.validate(expectedStructure, true);
        const auto nonStrict = connection.validate(expectedStructure, false);

        //then we get back that the extra table is missing
        const unordered_set<string> expected = {"Missing table extra_table"};
        EXPECT_EQ(expected, strict);
        EXPECT_EQ(expected, nonStrict);
    }

    TEST(validate, will_return_an_error_if_the_database_is_disconnected)
    {
        //given we have a database that is not connected
        const TemporaryFile database = ".sfdb_4f92050777cd4f13acde2c3f2e1007eb";
        SqliteConnection connection = setupDatabase(database.getPath());

        //when we check the structure of the database
        const auto actual = connection.validate({}, true);

        //then we get back that the database is not connected
        const unordered_set<string> expected = {"Database not connected .sfdb_4f92050777cd4f13acde2c3f2e1007eb"};
        EXPECT_EQ(expected, actual);
    }
}