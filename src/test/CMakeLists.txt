if(NOT SFSkipTesting)
    find_package(GTest REQUIRED)
    include(GoogleTest)
    enable_testing()
    add_executable(Tests SqliteConnectionTest.c++)
    target_link_libraries(Tests GTest::gtest_main SqliteConnection sqlite3 StiltFox::Scribe::File)
    gtest_discover_tests(Tests)
endif()