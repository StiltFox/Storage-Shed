if(NOT SFSkipTesting)
    find_package(GTest REQUIRED)
    find_package(nlohmann_json REQUIRED)

    add_subdirectory(print_helper)

    add_executable(MariaDBTests
            MariaDBConnection/TestRunner.c++
            MariaDBConnection/ConnectionInformation/ToJDBCStringTest.c++
            MariaDBConnection/ConnectionTests.c++
            MariaDBConnection/GetAllDataTests.c++
            MariaDBConnection/GetMetaDataTests.c++
            MariaDBConnection/PerformQueryTests.c++
            MariaDBConnection/TransactionTests.c++
    )

    add_executable(SqliteTests
            SqliteConnection/TransactionTests.c++
            SqliteConnection/ConnectionTests.c++
            SqliteConnection/GetMetaDataTests.c++
            SqliteConnection/PerformUpdateTests.c++
            SqliteConnection/validateTests.c++
            SqliteConnection/performQueryTests.c++
            SqliteConnection/GetAllDataTests.c++
    )

    target_link_libraries(SqliteTests
            PrintHelper
            gtest
            gtest_main
            StiltFox::Scribe::TempFile
            nlohmann_json
            SqliteConnection
    )

    target_link_libraries(MariaDBTests
            PrintHelper
            gtest
            gtest_main
            StiltFox::Scribe::TempFile
            nlohmann_json
            MariaDBConnection
    )
endif ()