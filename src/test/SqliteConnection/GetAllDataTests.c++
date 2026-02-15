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

namespace StiltFox::StorageShed::Tests::Sqlite_Connection::GetAllData
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
                           "CONSTRAINT FILE_TAG_FK_CATEGORY_ID FOREIGN KEY (categoryid) REFERENCES "
                           "tagcategory(categoryid) ON DELETE RESTRICT ON UPDATE RESTRICT);";
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

    TEST(getAllData, will_return_connected_false_if_the_datbase_is_not_connected)
    {
        //given we have a database that we do not connect to
        const TemporaryFile database = ".sfdb_f3da8544995d4dbe8823e905c7707403";
        SqliteConnection connection = setupDatabase(database);

        //when we try to get all data from the database
        const auto actual = connection.getAllData();

        //then we get back that we are not connected to the database
        const Result<MultiTableData> expected =
        {
            false,
            0,
            "",
            {},
            {}
        };
        EXPECT_EQ(expected, actual);
    }

    TEST(getAllData, will_return_all_data_from_the_database_when_we_are_connected_to_the_database)
    {
        //given we have a database that we are connected to
        const TemporaryFile database = ".sfdb_d7f3e072ce834de286495307ab141118";
        SqliteConnection connection = setupDatabase(database);
        connection.connect();

        //when we try to get all the data in the database
        const auto actual = connection.getAllData();

        //then we get back all the data in the database
        const Result<MultiTableData> expected =
        {
            true,
            0,
            "",
            {
                {"select tbl_name from sqlite_schema where type = 'table';"},
                {"select * from FILEDATA;"},
                {"select * from FILETAG;"}
            },
            {
                {"FILEDATA",
                    {
                        {{"hashcode", "abc"},{"title","scp-009"},{"trash","0"}},
                        {{"hashcode", "asd"},{"title","scp-000"},{"trash","1"}}
                    }
                },
                {"FILETAG", {}}
            }
        };
        EXPECT_EQ(expected, actual);
    }
}