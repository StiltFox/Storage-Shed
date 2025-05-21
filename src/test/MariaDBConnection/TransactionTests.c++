/*******************************************************
* Created by Cryos on 5/16/25.
* Copyright 2025 Stilt Fox® LLC
*
* See LICENSE on root project directory for terms
* of use.
********************************************************/
#include <gtest/gtest.h>
#include "PrintHelper.h++"
#include "TestHelpFunctions.h++"

namespace StiltFox::StorageShed::Tests::MariaDB_Connection::Transactions
{
    TEST(transaction, starting_a_transaction_will_register_the_changes_to_a_database_if_committed)
    {
        //given we connect to the database
        generateTablesWithSomeData();
        const MariaDBConnection::ConnectionInformation connectionInformation = getConnectionInformationFromEnvironment();
        MariaDBConnection connection = connectionInformation;
        connection.connect();

        //when we start a transaction, make the change, then commit
        connection.startTransaction();
        connection.performUpdate("create table test2.new_table (id int primary key);");
        connection.commitTransaction();

        //then the results will be saved
        Data::Result<Data::TableDefinitions> expected =
        {
            true,
            "",
            {
                {"select concat(TABLE_SCHEMA, '.', TABLE_NAME) as TABLE_NAME,COLUMN_NAME,COLUMN_TYPE "
                    "from information_schema.COLUMNS where TABLE_SCHEMA "
                    "not in ('information_schema', 'mysql', 'performance_schema');"}
            },
            {
                {"test.table1",{{"id", "int(11)"}, {"name", "varchar(255)"},
                    {"dead", "tinyint(1)"}}},
                {"test.table2",{{"id_1", "int(11)"}, {"id_2", "int(11)"}}},
                {"test2.information",{{"id","uuid"}}},
                {"test2.new_table",{{"id", "int(11)"}}}
            }
        };
        EXPECT_EQ(expected, connection.getMetaData());
    }
}