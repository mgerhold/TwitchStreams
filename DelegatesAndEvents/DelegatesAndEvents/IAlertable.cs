using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DelegatesAndEvents
{
    interface IAlertable
    {
        void OnElapsedSeconds(int numberOfSeconds);
    }
}
