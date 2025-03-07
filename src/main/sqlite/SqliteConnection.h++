#ifndef StiltFox_UniversalLibrary_SqliteConnection
#define StiltFox_UniversalLibrary_SqliteConnection
#include <string>
#include <functional>
#include "DatabaseConnection.h++"

namespace StiltFox::StorageShed
{
    /*******************************************************************************************************************
     * This class is an SQLite implementation of DatabaseConnection.
     ******************************************************************************************************************/
    class SqliteConnection : public DatabaseConnection
    {
        bool checkIfValidSqlDatabase();
        void forEachTable(const std::function<void(std::string)>&, std::string* = nullptr) const;

        public:
        SqliteConnection(const std::string& connection);
        SqliteConnection(const char* connection);

        bool connect() override;
        void disconnect() override;
        Data::Result<void*> startTransaction() override;
        Data::Result<void*> rollbackTransaction() override;
        Data::Result<void*> commitTransaction() override;
        Data::Result<void*> performUpdate(std::string statement) override;
        Data::Result<void*> performUpdate(const Data::StructuredQuery& statement) override;
        std::unordered_set<std::string> validate(Data::TableDefinitions tableDefinitions, bool strict) override;
        Data::Result<Data::TableDefinitions> getMetaData() override;
        Data::Result<Data::QueryReturnData> performQuery(std::string query) override;
        Data::Result<Data::QueryReturnData> performQuery(Data::StructuredQuery query) override;
        Data::Result<Data::MultiTableData> getAllData() override;

        SqliteConnection& operator=(const std::string& connection);
        ~SqliteConnection();
    };
}

#endif