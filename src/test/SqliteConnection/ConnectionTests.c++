/*******************************************************
* Created by Cryos on 2/16/25.
* Copyright 2025 Stilt Fox® LLC
*
* See LICENSE on root project directory for terms
* of use.
********************************************************/
#include <gtest/gtest.h>
#include <Stilt_Fox/Scribe/TempFile.h++>
#include "SqliteConnection.h++"

using namespace std;
using namespace StiltFox::Scribe;
using namespace StiltFox::StorageShed;

namespace StiltFox::StorageShed::Tests::Sqlite_Connection::Connection
{
    TEST(connect, will_return_false_and_not_connect_if_the_file_does_not_exist)
    {
        //given we try to connect to a non-existing database
        SqliteConnection connection = ".non-existing.db";

        //when we try to connect
        const bool actual = connection.connect();

        //then we did not connect
        EXPECT_FALSE(actual);
        EXPECT_FALSE(connection.isConnected());
    }

    TEST(connect, will_not_create_a_file)
    {
        //given we have a non-existing database
        SqliteConnection connection = ".non-existing.db";

        //when we try to connect
        connection.connect();

        //then we did not create the database
        EXPECT_FALSE(filesystem::exists(".non-existing.db"));
    }

    TEST(connect, will_return_false_and_not_connect_if_the_file_is_not_a_database)
    {
        //given we have a file that is not a database
        const TemporaryFile badFile(".sqliteConnection_regularfile.txt", "scp-001 proposal draft.");
        SqliteConnection connection = badFile.getPath();

        //when we try to connect to the file
        const bool actual = connection.connect();

        //then we dont connect
        EXPECT_FALSE(actual);
        EXPECT_FALSE(connection.isConnected());
    }

    TEST(connect, will_return_true_and_connect_to_an_empty_file)
    {
        //given we have an empty file
        const TemporaryFile emptyFile = ".sqliteConnection_emptyfile.txt";
        SqliteConnection connection = emptyFile.getPath();

        //when we try to connect to the file
        const bool actual = connection.connect();

        //then we connect
        EXPECT_TRUE(actual);
        EXPECT_TRUE(connection.isConnected());
    }

    TEST(connect, will_return_true_if_the_connection_is_already_connected)
    {
        //given we have an already connected connection
        const TemporaryFile dbFile = ".sqliteConnection_test_pre_connected_database.db";
        SqliteConnection connection = dbFile.getPath();
        connection.connect();

        //when we try to connect again
        const bool actual = connection.connect();

        //then we show as being connected
        EXPECT_TRUE(actual);
        EXPECT_TRUE(connection.isConnected());
    }
}