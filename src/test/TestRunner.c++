/*******************************************************
* Created by Cryos on 4/13/25.
* Copyright 2025 Stilt Fox® LLC
*
* See LICENSE on root project directory for terms
* of use.
********************************************************/
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <Stilt_Fox/Scribe/File.h++>

using namespace std;
using namespace nlohmann;
using namespace StiltFox::Scribe;

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}