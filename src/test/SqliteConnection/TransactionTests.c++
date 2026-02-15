/*******************************************************
* Created by Cryos on 1/25/25.
* Copyright 2025 Stilt Fox® LLC
*
* See LICENSE on root project directory for terms
* of use.
********************************************************/
#include <Stilt_Fox/Scribe/TempFile.h++>
#include <gtest/gtest.h>
#include "PrintHelper.h++"
#include "SqliteConnection.h++"

using namespace StiltFox::Scribe;
using namespace StiltFox::StorageShed;

namespace StiltFox::StorageShed::Tests::Sqlite_Connection::Transaction
{
    TEST(transaction, starting_a_transaction_will_register_the_changes_to_a_database_if_commited)
    {
        //Given we have a database
        TemporaryFile databaseFile = ".sfdb_fc43d2287b214793a0d3358bd9015bcb";
        SqliteConnection connection = databaseFile.getPath();

        //when we start a transaction, then make a change then commit
        connection.connect();
        connection.startTransaction();
        connection.performUpdate("create table test (id int primary key);");
        connection.commitTransaction();

        //then the data will be saved.
        Data::Result<Data::TableDefinitions> expected =
        {
            true,
            0,
            "",
            {
                {"select tbl_name from sqlite_schema where type = 'table';"},
                {"select * from pragma_table_info(?);", {"test"}}
            },
            {
                    {"test", {{"id", "INT"}}}
                }
        };
        EXPECT_EQ(expected, connection.getMetaData());
    }

    TEST(transaction, starting_a_transaction_then_rolling_it_back_will_remove_change_and_close_the_transaction)
    {
        //Given we have a database
        TemporaryFile databaseFile = ".sfdb_00c33ce439914f179ee10046be760165";
        SqliteConnection connection = databaseFile.getPath();

        //when we start a transaction, then make a change then rollback
        connection.connect();
        connection.startTransaction();
        connection.performUpdate("create table test (id int, constraint primary key pk_test(id));");
        connection.rollbackTransaction();
        auto temp = connection.performUpdate("create table pickle (id int primary key);");

        //then only data saved after the transaction will be detected
        Data::Result<Data::TableDefinitions> expected =
            {
            true,
            0,
            "",
            {
                {"select tbl_name from sqlite_schema where type = 'table';"},
                {"select * from pragma_table_info(?);", {"pickle"}}
            },
            {
                        {"pickle", {{"id", "INT"}}}
            }
            };
        Data::Result<Data::TableDefinitions> actual = connection.getMetaData();
        EXPECT_EQ(expected, actual);
    }

    TEST(transaction, starting_a_transaction_while_not_connected_to_the_database_will_return_a_connected_of_false)
    {
        //given we have a database and we are not connected
        TemporaryFile databaseFile = ".sfdb_251e7bcab2564f07b6e7a29104538d9b";
        SqliteConnection connection = databaseFile.getPath();

        //when we start a transaction
        auto actual = connection.startTransaction();

        //then we get back a connected value of false
        Data::Result<void*> expected = {false, 0, "", {{"begin transaction;"}}, nullptr};
        EXPECT_EQ(expected, actual);
    }

    TEST(transaction, rolling_back_a_transaction_while_not_connected_to_the_database_will_return_a_connected_of_false)
    {
        //given we have a database and we are not connected
        TemporaryFile databaseFile = ".sfdb_2ddf01329da542e5bfed38681bb192c1";
        SqliteConnection connection = databaseFile.getPath();

        //when we roll back a transaction
        auto actual = connection.rollbackTransaction();

        //then we get back a connected value of false
        Data::Result<void*> expected = {false, 0, "", {{"rollback transaction;"}}, nullptr};
        EXPECT_EQ(expected, actual);
    }

    TEST(transaction, committing_a_transaction_while_not_connected_to_the_database_will_return_a_connected_of_false)
    {
        //given we have a database and we are not connected
        TemporaryFile databaseFile = ".sfdb_e0c88b738cb74c0fa3142e6dbe9e96f8";
        SqliteConnection connection = databaseFile.getPath();

        //when we commit a transaction
        auto actual = connection.commitTransaction();

        //then we get back a connected value of false
        Data::Result<void*> expected = {false, 0, "", {{"commit transaction;"}}, nullptr};
        EXPECT_EQ(expected, actual);
    }

    TEST(transaction, committing_a_transaction_while_a_transaction_is_not_in_place_will_return_an_error)
    {
        //given we have a database and we are connected
        TemporaryFile databaseFile = ".sfdb_36c76c223536484d95ad556e3f98309d";
        SqliteConnection connection = databaseFile.getPath();
        connection.connect();

        //when we commit a transaction
        auto actual = connection.commitTransaction();

        //then we get back a connected value of true and an error
        Data::Result<void*> expected = {true, 0, "cannot commit - no transaction is active",
            {{"commit transaction;"}}, nullptr};
        EXPECT_EQ(expected, actual);
    }

    TEST(transaction, rolling_back_a_transaction_while_a_transaction_is_not_in_place_will_return_an_error)
    {
        //given we have a database and we are connected
        TemporaryFile databaseFile = ".sfdb_8942569fcb044098b46de70e7d004152";
        SqliteConnection connection = databaseFile.getPath();
        connection.connect();

        //when we roll back a transaction
        auto actual = connection.rollbackTransaction();

        //then we get back a connected value of true and an error
        Data::Result<void*> expected = {true, 0, "cannot rollback - no transaction is active",
            {{"rollback transaction;"}}, nullptr};
        EXPECT_EQ(expected, actual);
    }
}