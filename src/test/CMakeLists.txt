if(NOT SFSkipTesting)
    find_package(GTest REQUIRED)

    add_executable(Tests
            SqliteConnection/TransactionTests.c++
    )

    target_link_libraries(Tests
            GTest::gtest_main
            StiltFox::Scribe::TempFile
            SqliteConnection
    )
endif ()