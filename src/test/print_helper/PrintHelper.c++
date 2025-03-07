/*******************************************************
* Created by Cryos on 2/15/25.
* Copyright 2025 Stilt Fox® LLC
*
* See LICENSE on root project directory for terms
* of use.
********************************************************/
#include <nlohmann/json.hpp>
#include "PrintHelper.h++"

using namespace std;
using namespace nlohmann;

namespace StiltFox::StorageShed::Data
{
    template <typename T>
    json resultToJsonGeneric(const Result<T>& result)
    {
        json output;
        output["success"] = result.success;
        output["connected"] = result.connected;
        output["performed_query"] = result.performedQuery;
        return output;
    }

    void PrintTo(const Result<void*>& result, ostream* outputStream)
    {
        *outputStream << resultToJsonGeneric(result).dump(4) << endl;
    }

    void PrintTo(const Result<TableDefinitions>& result, ostream* outputStream)
    {
        json outputText = resultToJsonGeneric(result);
        outputText["data"] = result.data;
        *outputStream << outputText.dump(4) << endl;
    }
}