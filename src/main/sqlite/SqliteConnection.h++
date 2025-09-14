#ifndef StiltFox_UniversalLibrary_SqliteConnection
#define StiltFox_UniversalLibrary_SqliteConnection
#include <string>
#include <functional>
#include <sqlite3.h>

#include "DatabaseConnection.h++"

namespace StiltFox::StorageShed
{
    /*******************************************************************************************************************
     * This class is an SQLite implementation of DatabaseConnection.
     *
     * This class will disconnect and delete its connection pointer whenever it leaves scope. If you wish to use this
     * connection in multiple places, consider a few options:
     * 1) Pass around a shared pointer.
     *      - this will allow functions to use the same connection
     * 2) Copy the class to a new object
     *       - the copy constructor copies the connection string, but not the connection itself, allowing the copy to
     *         manage its own connection.
     ******************************************************************************************************************/
    class SqliteConnection : public DatabaseConnection
    {
        sqlite3* connection = nullptr;
        std::string connectionString;

        bool checkIfValidSqlDatabase() const;
        void forEachTable(const std::function<void(std::string)>&, std::vector<Data::StructuredQuery>& queryTracker)
                                                                                                                  const;

        public:
        SqliteConnection(const std::string& connection);
        SqliteConnection(const char* connection);
        SqliteConnection(const SqliteConnection& toCopy);

        // For information on this block of functions, see DatabaseConnection.h++
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
        bool isConnected() override;
        std::string getConnectionString() override;
        std::string getSqlType() override;
        // end override section

        /***************************************************************************************************************
         * This operator makes it possible to assign a connection string directly to an SqliteConnection. This is like
         * calling the constructor that accepts a std::string.
         *
         * @param connection - the path to the Sqlite file that you wish to connect to.
         *
         * @return this will return a reference to the newly copied SqliteConnection
         **************************************************************************************************************/
        SqliteConnection& operator=(const std::string& connection);
        ~SqliteConnection();
    };
}

#endif