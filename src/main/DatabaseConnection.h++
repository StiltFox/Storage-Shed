#ifndef StiltFox_UniversalLibrary_DatabaseConnection
#define StiltFox_UniversalLibrary_DatabaseConnection
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>

/***********************************************************************************************************************
 * This file contains the main interfaces that will be implemented for each supported database. These are designed to
 * represent relational databases.
 **********************************************************************************************************************/
namespace StiltFox::StorageShed
{
    /*******************************************************************************************************************
     * Because table structures can be complex, here we will make some type declarations to make our lives a little
     * easier.
     ******************************************************************************************************************/
    namespace Data
    {
        // maps of maps of maps can get a little long. So here's some type defs.
        typedef std::unordered_map<std::string, std::string> Row, RowDefinition;
        typedef std::vector<Row> QueryReturnData;
        typedef std::unordered_map<std::string, Data::QueryReturnData> MultiTableData;
        typedef std::unordered_map<std::string, RowDefinition> TableDefinitions;

        /***************************************************************************************************************
         * In general, throwing exceptions in C++ is a bad idea. Because of this we need a way to communicate to the
         * caller that something went wrong. IO operations are prone to going wrong.
         *
         * This class wraps the return value in a success check. This allows the caller to see if there was an error
         * and what the offending query was.
         **************************************************************************************************************/
        template <typename T>
        struct Result
        {
            bool success = false, connected = false;
            std::string performedQuery;
            T data;
        };

        /***************************************************************************************************************
         * This function overrides the comparison operator for result.
         **************************************************************************************************************/
        template <typename T>
        bool operator==(Result<T> lhs, Result<T> rhs)
        {
            return lhs.success == rhs.success && lhs.connected == rhs.connected && lhs.data == rhs.data && lhs.performedQuery == rhs.performedQuery;
        }

        /***************************************************************************************************************
         * This class represents a structured query. Question marks in the query will be safely replaced with parameters
         * in the order they are given. All data will be represented as a string to make things easier.
         **************************************************************************************************************/
        struct StructuredQuery
        {
            // A query with ? marks where data is supposed to go
            std::string query;
            // a string representation of the data to be inserted
            std::vector<std::string> parameters;
        };
    }

    /*******************************************************************************************************************
     * This class represents a relational database style connection. This class must be implemented by each supported
     * database.
     ******************************************************************************************************************/
    struct DatabaseConnection
    {
        /***************************************************************************************************************
         * Open this connection to the database. No operations will occur without this happening.
         *
         * @return returns weather the database could be connected to or not.
         **************************************************************************************************************/
        virtual bool connect() = 0;
        /***************************************************************************************************************
         * Disconnect this connection from the database. This does not delete anything or change anything about the
         * internal structure of this object. It can be reopened at any time.
         **************************************************************************************************************/
        virtual void disconnect() = 0;
        /***************************************************************************************************************
         * This function will start a transaction.
         *
         * @return returns a result indicating weather the transaction was opened or not. The data is not used.
         **************************************************************************************************************/
        virtual Data::Result<void*> startTransaction() = 0;
        /***************************************************************************************************************
         * This function will undo all changes made to the database during the current transaction. the current
         * transaction will be closed. This function will do nothing if there is no active transaction.
         *
         * @return returns a result indicating weather or not the transaction was rolled back. If the session is closed
         *         or a transaction is not started success will be false. If his is successful the transaction will
         *         be closed.
         **************************************************************************************************************/
        virtual Data::Result<void*> rollbackTransaction() = 0;
        /***************************************************************************************************************
         * This function will apply all changes from the current transaction to the database. The current transaction
         * will be closed. This function will do nothing if there is no active transaction.
         *
         * @return returns a result indicating weather or not the transaction was commited. Commiting the transaction
         *         will save the changed data to the database and close the transaction.
         **************************************************************************************************************/
        virtual Data::Result<void*> commitTransaction() = 0;
        /***************************************************************************************************************
         * This function will execute a query but not return any results. This is great for update statements, insert
         * statements, drops, creates, basically anything that doesn't return a query result that you care about.
         *
         * @param statement - This is the sql statement you'd like to execute.
         *
         * @return this will return a Result object of the void type. The success indicator and the connected indicator
         *         will return valid information, however data will remain unused. Hence, the void typing.
         **************************************************************************************************************/
        virtual Data::Result<void*> performUpdate(std::string statement) = 0;
        /***************************************************************************************************************
         * This function will execute a query but not return any results. This is great for update statements, insert
         * statements, drops, creates, basically anything that doesn't return a query result that you care about.
         *
         * @param statement - a structured query that represents the statement you'd like to execute. See the structured
         *                    query class above for details.
         *
         * @return this will return a Result object of the void type. The success indicator and the connected indicator
         *         will return valid information, however data will remain unused. Hence, the void typing.
         **************************************************************************************************************/
        virtual Data::Result<void*> performUpdate(const Data::StructuredQuery& statement) = 0;
        /***************************************************************************************************************
         * This function will validate the structure of the database against a provided set of table definitions.
         * Depending on weather strict is set or not, it will change how it's evaluation is done. If strict is set
         * the function will expect the database to match the set of table definitions exactly. No extra tables, no
         * extra columns.
         *
         * @param tableDefinitions - This is the set of tables and columns that you expect to be in the database.
         * @param strict - If this is set to true, the function will expect the database structure to match the provided
         *                 structure exactly. If it's set to false it will only fail if tables or columns are missing.
         *
         * @return this will return a set of strings that indicate each error found. It will be a list of differences
         *         between the database and requested structure. If this list is empty, it means that validation was
         *         passed. These are designed to be human-readable for error messages or logging.
         **************************************************************************************************************/
        virtual std::unordered_set<std::string> validate(Data::TableDefinitions tableDefinitions, bool strict) = 0;
        /***************************************************************************************************************
         * This function will get the metadata of the database. That means a list of every table, each column in that
         * table and it's type.
         *
         * @return this will return a result containing the table definitions of the database. This will only fail if
         *         the database is not connected.
         **************************************************************************************************************/
        virtual Data::Result<Data::TableDefinitions> getMetaData() = 0;
        /***************************************************************************************************************
         * This function will perform a query that returns a result. This is usually used for select or show
         * statements.
         *
         * @param query - this is the query that you wish to execute.
         *
         * @return this will return a result of the QueryReturnData type. If success is set to false the data returned
         *         should be considered invalid.
         **************************************************************************************************************/
        virtual Data::Result<Data::QueryReturnData> performQuery(std::string query) = 0;
        /***************************************************************************************************************
         * This function will perform a query that returns a result. This is usually used for select or show
         * statements.
         *
         * @param query - a structured query that represents the statement you'd like to execute. See the structured
         *                query class above for details.
         *
         * @return this will return a result of the QueryReturnData type. If success is set to false the data returned
         *         should be considered invalid.
         **************************************************************************************************************/
        virtual Data::Result<Data::QueryReturnData> performQuery(Data::StructuredQuery query) = 0;
        /***************************************************************************************************************
         * This function will return all data stored in the database. This can be slow and memory consuming on larger
         * databases. It is recommended to only use this for testing purposes.
         *
         * @return a result object holding all data stored in the database, sorted by table. This will fail if the
         *         database is not connected.
         **************************************************************************************************************/
        virtual Data::Result<Data::MultiTableData> getAllData() = 0;

        virtual bool isConnected() = 0;

        virtual std::string getConnectionString() = 0;
    };
}

#endif