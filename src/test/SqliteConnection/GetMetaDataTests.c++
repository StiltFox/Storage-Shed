/*******************************************************
* Created by Cryos on 2/16/25.
* Copyright 2025 Stilt Fox® LLC
*
* See LICENSE on root project directory for terms
* of use.
********************************************************/
#include <gtest/gtest.h>
#include <sqlite3.h>
#include <Stilt_Fox/Scribe/TempFile.h++>
#include "SqliteConnection.h++"

using namespace std;
using namespace StiltFox::Scribe;
using namespace StiltFox::StorageShed;
using namespace StiltFox::StorageShed::Data;

namespace StiltFox::StorageShed::Tests::Sqlite_Connection::getMetaData
{
    string createFileData = "CREATE TABLE IF NOT EXISTS FILEDATA ("
                            "hashcode VARCHAR(255) NOT NULL,"
                            "title VARCHAR(255),"
                            "trash BOOLEAN NOT NULL,"
                            "CONSTRAINT FILE_DATA_PRIMARY_KEY PRIMARY KEY (hashcode));";
    string createFileTag = "CREATE TABLE IF NOT EXISTS FILETAG ("
                           "filetagid VARBINARY NOT NULL,"
                           "name VARCHAR(255) NOT NULL,"
                           "categoryid VARBINARY NOT NULL,"
                           "CONSTRAINT FILE_TAG_PRIMARY_KEY PRIMARY KEY (filetagid),"
                           "CONSTRAINT FILE_TAG_FK_CATEGORY_ID FOREIGN KEY (categoryid) "
                           "REFERENCES tagcategory(categoryid) ON DELETE RESTRICT ON UPDATE RESTRICT);";
    string insertFileA = "INSERT INTO FILEDATA (hashcode, title, trash) values ('abc','scp-009',false);";
    string insertFileB = "INSERT INTO FILEDATA (hashcode, title, trash) values ('asd','scp-000',true);";

    SqliteConnection setupDatabase(File databasePath)
    {
        sqlite3* connection;
        sqlite3_stmt* createFileStmt;
        sqlite3_stmt* createTagStmt;
        sqlite3_stmt* stmtFileA;
        sqlite3_stmt* stmtFileB;

        sqlite3_open(databasePath.getPath().c_str(), &connection);

        sqlite3_prepare(connection, createFileData.c_str(), createFileData.length(), &createFileStmt, nullptr);
        sqlite3_step(createFileStmt);
        sqlite3_finalize(createFileStmt);

        sqlite3_prepare(connection, createFileTag.c_str(), createFileTag.length(), &createTagStmt, nullptr);
        sqlite3_step(createTagStmt);
        sqlite3_finalize(createTagStmt);

        sqlite3_prepare(connection, insertFileA.c_str(), insertFileA.length(), &stmtFileA, nullptr);
        sqlite3_step(stmtFileA);
        sqlite3_finalize(stmtFileA);

        sqlite3_prepare(connection, insertFileB.c_str(), insertFileB.length(), &stmtFileB, nullptr);
        sqlite3_step(stmtFileB);
        sqlite3_finalize(stmtFileB);

        sqlite3_close(connection);
        return databasePath.getPath();
    }

    TEST(getMetaData, gets_the_metadata_from_a_database)
    {
        //given we have a database to query
        const TemporaryFile database = ".sfdb_50063d12f27c4fd08bee7dcf7ca04910";
        SqliteConnection connection = setupDatabase(database);

        //when we get the metadata for the database
        connection.connect();
        const auto actual = connection.getMetaData();

        //then we get back a map of all the tables and columns
        const Result<TableDefinitions> expected =
            {
                true,
                "",
            {
                {"select tbl_name from sqlite_schema where type = 'table';"},
                {"select * from pragma_table_info(?);", {"FILEDATA"}},
                {"select * from pragma_table_info(?);", {"FILETAG"}}
            },
        {
                {"FILEDATA",{{"hashcode", "VARCHAR(255)"}, {"title", "VARCHAR(255)"}, {"trash", "BOOLEAN"}}},
                {"FILETAG",{{"filetagid", "VARBINARY"},{"name", "VARCHAR(255)"},{"categoryid", "VARBINARY"}}}
               }
            };
        EXPECT_EQ(actual, expected);
    }

    TEST(getMetaData, will_return_connected_false_if_the_database_is_not_connected)
    {
        //given we have a database that we do not connect to
        const TemporaryFile database = ".sfdb_c54c5dcd21fb4da69745d841e809945d";
        SqliteConnection connection = setupDatabase(database);

        //when we get the metadata for the database
        const auto actual = connection.getMetaData();

        //then we get back that we are not connected
        const Result<TableDefinitions> expected = {
            false,
            "",
            {},
            {}
        };
        EXPECT_EQ(expected, actual);
    }
}