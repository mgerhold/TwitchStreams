using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DatabaseTest
{

    /// <summary>
    /// This controller handles all "requests" related to clients.
    /// </summary>
    class ClientController
    {
        private ClientModel model = null;

        public ClientController(Database database)
        {
            model = new ClientModel(database);
        }

        /// <summary>
        /// Calculates the total number of clients and shows the result.
        /// </summary>
        public void Index()
        {
            // get parameter for the view
            int numClients = model.All.Length;

            ClientsView view = new ClientsView();
            view.Show(numClients);
        }

        /// <summary>
        /// Shows the name of a given client.
        /// </summary>
        /// <param name="clientID">The client to show.</param>
        public void Client(int clientID)
        {
            // some business logic...
            var client = model[clientID];
            string name = $"{client.LastName}, {client.FirstName}".ToUpper();

            // show view
            ClientView view = new ClientView();
            view.Show(name);
        }
    }
}
