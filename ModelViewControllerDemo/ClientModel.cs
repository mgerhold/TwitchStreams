using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DatabaseTest
{
    /// <summary>
    /// This class models the client database.
    /// </summary>
    class ClientModel
    {
        /// <summary>
        /// This is a helper class to encapsulate the data of a single client.
        /// </summary>
        public class Client
        {
            public string FirstName { get; set; }
            public string LastName { get; set; }

            public Client(string firstName, string lastName)
            {
                FirstName = firstName;
                LastName = lastName;
            }
        }

        private Database database = null;

        public ClientModel(Database database)
        {
            this.database = database;
        }

        /// <summary>
        /// Indexer to fetch a single client from the database.
        /// </summary>
        /// <param name="id">ID of the client.</param>
        /// <returns>The client if found, null otherwise.</returns>
        public Client this[int id]
        {
            get
            {
                var result = database.Query($"SELECT FirstName, LastName FROM clients WHERE ClientID = {id};");
                if (!result.HasRows)
                    return null;
                result.Read();
                Client client = new Client(result.GetString("FirstName"), result.GetString("LastName"));
                result.Close();
                return client;
            }
        }

        /// <summary>
        /// All clients currently in the database.
        /// </summary>
        public Client[] All
        {
            get
            {
                var result = database.Query("SELECT FirstName, LastName FROM clients;");
                List<Client> clients = new List<Client>();
                while (result.Read())
                {
                    clients.Add(new Client(result.GetString("FirstName"), result.GetString("LastName")));
                }
                result.Close();
                return clients.ToArray();
            }
        }
    }
}
