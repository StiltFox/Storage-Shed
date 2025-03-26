if(NOT SFSkipTesting)
    find_package(GTest REQUIRED)
    find_package(nlohmann_json REQUIRED)

    add_subdirectory(print_helper)

    add_executable(Tests
            SqliteConnection/TransactionTests.c++
            SqliteConnection/ConnectionTests.c++
            SqliteConnection/GetMetaDataTests.c++
            SqliteConnection/PerformUpdateTests.c++
            SqliteConnection/validateTests.c++
            SqliteConnection/performQueryTests.c++
            SqliteConnection/GetAllDataTests.c++
    )

    target_link_libraries(Tests
            PrintHelper
            GTest::gtest_main
            StiltFox::Scribe::TempFile
            SqliteConnection
    )
endif ()