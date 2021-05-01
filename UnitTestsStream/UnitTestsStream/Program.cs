using System;
using System.Linq;

namespace UnitTestsStream
{
    class Program
    {
        static void Main(string[] args)
        {
            Vector3 a = new Vector3(1.0, 2.0, 3.0);
            Vector3 b = a;
            Vector3 c = a + b;
            
            Console.ReadLine();
        }
    }
}
