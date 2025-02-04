/*******************************************************
* Created by Cryos on 1/25/25.
* Copyright 2025 Stilt Fox® LLC
*
* See LICENSE on root project directory for terms
* of use.
********************************************************/
#include <Stilt_Fox/Scribe/TempFile.h++>
#include <gtest/gtest.h>
#include "SqliteConnection.h++"

using namespace StiltFox::Scribe;
using namespace StiltFox::StorageShed;

TEST(SqliteConnection, starting_a_transaction_will_not_register_changes_to_a_database_if_not_commited)
{
    //Given we have a database
    TemporaryFile databaseFile = ".sfdb_3a67a82564ae4e79bfef2b4fd13f4dd6";
    SqliteConnection connection = databaseFile.getPath();

    //when we start a transaction, then make a change
    connection.startTransaction();
    connection.performUpdate("create table test (id int, constraint primary key pk_test(id))");

    //then the data won't be saved yet.
    Data::Result<Data::TableDefinitions> expected = {true, true, "select tbl_name from sqlite_schema where type = 'table';", {}};
    EXPECT_EQ(expected, connection.getMetaData());
}