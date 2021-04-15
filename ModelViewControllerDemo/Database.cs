using System;
using MySql.Data.MySqlClient;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DatabaseTest
{
    /// <summary>
    /// Class to establish a connection to a MySql database.
    /// </summary>
    class Database
    {
        private MySqlConnection connection = null;

        public Database(string connectionString)
        {
            try
            {
                connection = new MySqlConnection(connectionString);
                connection.Open();
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error establishing database connection (${ex.Message}).");
            }
        }

        /// <summary>
        /// Executes an Sql command that returns no data (e.g. INSERT).
        /// </summary>
        /// <param name="sql">The Sql command to execute.</param>
        /// <returns>The number of affected rows after execution or 0 on failure.</returns>
        public int Execute(string sql)
        {
            var command = new MySqlCommand(sql, connection);
            try
            {
                return command.ExecuteNonQuery();
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Failed to execute command {sql} (${ex.Message})");
                return 0;
            }
        }

        /// <summary>
        /// Executes a Sql query.
        /// </summary>
        /// <param name="sql">The Sql query to execute.</param>
        /// <returns>A MySqlDataReader to read the results of the query.</returns>
        public MySqlDataReader Query(string sql)
        {
            var command = new MySqlCommand(sql, connection);
            try
            {
                return command.ExecuteReader();
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Failed to execute query {sql} (${ex.Message})");
                return null;
            }
        }

    }
}
