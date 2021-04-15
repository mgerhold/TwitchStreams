using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DatabaseTest
{
    /// <summary>
    /// This view displays a client.
    /// </summary>
    class ClientView : IView
    {
        /// <summary>
        /// Show the view.
        /// </summary>
        /// <param name="args">The first (and only) parameter must be the name of the client.</param>
        public void Show(params object[] args)
        {
            Console.WriteLine( "============================================");
            Console.WriteLine($"| {args[0]}");
            Console.WriteLine( "============================================");
        }
    }
}
