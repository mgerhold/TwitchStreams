using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DatabaseTest
{
    /// <summary>
    /// This is a view that shows the total number of clients.
    /// </summary>
    class ClientsView : IView
    {
        public void Show(params object[] args)
        {
            Console.WriteLine($"The total number of clients is: {args[0]}");
        }
    }
}
