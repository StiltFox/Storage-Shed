/*******************************************************
* Created by cryos on 3/26/25.
* Copyright 2024 Stilt Fox® LLC
*
* See LICENSE on root project directory for terms
* of use.
********************************************************/
#ifndef Stilt_Fox_7f34044c89ad492ebe326eeda332de02
#define Stilt_Fox_7f34044c89ad492ebe326eeda332de02
#include <string>
#include <mariadb/conncpp.hpp>
#include "DatabaseConnection.h++"

namespace StiltFox::StorageShed
{
    class MariaDBConnection : public DatabaseConnection
    {
        public:
        struct ConnectionInformation
        {
            std::string hostName = "localhost", databaseName;
            sql::Properties parameters;
            int portNumber = 3306;

            std::string toJDBCString() const;
            std::string getJDBCStringWithoutParameters() const;
        };

        private:
        sql::Connection* connection;
        ConnectionInformation connectionInformation;

        public:
        MariaDBConnection(const ConnectionInformation& connectionInformation);
        MariaDBConnection(const MariaDBConnection& toCopy);

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

        MariaDBConnection& operator=(const ConnectionInformation& connectionInformation);
        ~MariaDBConnection();
    };
};

#endif