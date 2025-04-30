/*******************************************************
* Created by Cryos on 4/23/25.
* Copyright 2025 Stilt Fox® LLC
*
* See LICENSE on root project directory for terms
* of use.
********************************************************/
#include <gtest/gtest.h>
#include "PrintHelper.h++"
#include "TestHelpFunctions.h++"

using namespace std;
using namespace sql;
using namespace StiltFox::StorageShed::Data;

namespace StiltFox::StorageShed::Tests::MariaDB_Connection::GetAllData
{
    TEST(getAllData, will_return_success_of_false_and_connected_of_false_if_the_database_is_not_connected)
    {
        //given we have a database that we do not connect to
        MariaDBConnection connection = getConnectionInformationFromEnvironment();

        //when we try to get all the data
        const auto actual = connection.getAllData();

        //then we get back that we are not connected and the operation was not a success
        const Result<MultiTableData> expected =
        {
            false,
            false,
            "",
            {}
        };
        EXPECT_EQ(expected, actual);
    }

    TEST(getAllData, will_return_all_data_from_all_tables_and_schemas_when_we_are_connected_to_a_database)
    {
        //given we have a database that we are connected to
        generateTablesWithSomeData();
        MariaDBConnection connection = getConnectionInformationFromEnvironment();
        connection.connect();

        //when we try to get all the data
        const auto actual = connection.getAllData();

        //then we get back all the data from all the schemas in the database
        const Result<MultiTableData> expected =
        {
            true,
            true,
            "select * from test.table1; select * from test.table2; select * from test2.information;",
            {
                {"test.table1",
                    {
                        {{"id", "1"},{"name","bagel"},{"dead","0"}},
                        {{"id", "2"},{"name","fork"},{"dead","1"}},
                        {{"id", "3"},{"name","pickle"},{"dead","0"}}
                    }
                },
                {"test.table2", {}},
                {"test2.information",
                    {
                        {{"id", "c5ab7ae9-521c-406b-a0e2-53f8b45b3ea6"}}
                    }
                }
            }
        };
        EXPECT_EQ(expected, actual);
        clearDatabase();
    }
}