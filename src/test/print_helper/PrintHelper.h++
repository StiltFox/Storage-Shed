/*******************************************************
* Created by cryos on 2/16/25.
* Copyright 2024 Stilt Fox® LLC
*
* See LICENSE on root project directory for terms
* of use.
********************************************************/
#ifndef Stilt_Fox_b393e494bc6e4e4eaeea8d44d7b8f239
#define Stilt_Fox_b393e494bc6e4e4eaeea8d44d7b8f239
#include "DatabaseConnection.h++"

namespace StiltFox::StorageShed::Data
{
    void PrintTo(const Result<void*>& result, std::ostream* outputStream);
    void PrintTo(const Result<TableDefinitions>& result, std::ostream* outputStream);
    void PrintTo(const Result<QueryReturnData>& result, std::ostream* outputStream);
    void PrintTo(const Result<MultiTableData>& result, std::ostream* outputStream);
}

#endif