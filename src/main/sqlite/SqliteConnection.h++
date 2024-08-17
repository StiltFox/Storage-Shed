#ifndef StiltFox_UniversalLibrary_SqliteConnection
#define StiltFox_UniversalLibrary_SqliteConnection
#include <functional>
#include "DatabaseConnection.h++"

namespace StiltFox::StorageShed
{
    class SqliteConnection : public DatabaseConnection
    {
        bool checkIfValidSqlDatabase();
        void forEachTable(const std::function<void(std::string)>&);

        public:
        SqliteConnection(const char* connection);
        SqliteConnection(const std::string& connection);

        bool connect() override;
        void disconnect() override;
        std::unordered_set<std::string> validate(std::unordered_map<std::string, std::unordered_map<std::string, std::string>>) override;
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> getMetaData() override;
        void performUpdate(std::string) override;
        void performUpdate(std::string, std::vector<std::string>) override;
        std::vector<std::unordered_map<std::string, std::string>> performQuery(std::string) override;
        std::vector<std::unordered_map<std::string, std::string>> performQuery(std::string, std::vector<std::string>) override;
        std::unordered_map<std::string,std::vector<std::unordered_map<std::string, std::string>>> getAllData() override;

        SqliteConnection& operator=(const std::string& connection);
        ~SqliteConnection();
    };
}

#endif