using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DatabaseTest
{

    /// <summary>
    /// This is just an example for how you can generalize
    /// the views.
    /// </summary>
    interface IView
    {
        void Show(params object[] args);
    }
}
