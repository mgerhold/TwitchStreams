using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DelegatesAndEvents
{
    class JackCimberly : IAlertable
    {
        public void OnElapsedSeconds(int numberOfSeconds)
        {
            if (numberOfSeconds == 7)
            {
                Console.WriteLine("Mmmmmh....lecker Frühstück!");
            }
        }
    }
}
