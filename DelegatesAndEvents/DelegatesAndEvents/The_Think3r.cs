using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DelegatesAndEvents
{
    class The_Think3r : IAlertable
    {
        public void OnElapsedSeconds(int numberOfSeconds)
        {
            if (numberOfSeconds == 12)
            {
                Console.WriteLine("Ich mache einen Rissuntersuchung!");
            }
            else if (numberOfSeconds == 13)
            {
                Console.WriteLine("Re");
            }
        }

    }
}
