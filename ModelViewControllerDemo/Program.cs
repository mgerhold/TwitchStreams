using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DatabaseTest
{
    class Program
    {
        static void Main(string[] args)
        {
            Database database = new Database("datasource=127.0.0.1;port=3306;username=root;password=;database=test2;");

            ClientController controller = new ClientController(database);
            controller.Index();
            controller.Client(9);

            Console.ReadLine();
        }
    }
}
