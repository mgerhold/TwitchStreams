using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StackVsHeap
{
    class MyClass
    {
        int number;
    }

    struct MyStruct
    {
        int number;
    }

    class Program
    {
        static void Func()
        {
            int c = 3;
            int[] numbers = new int[] { 1, 2, 3, 4, 5};
        }

        static void Main(string[] args)
        {
            int a = 1;
            int b = 2;
            Func();

            // Referenzdatentyp
            MyClass c = new MyClass();

            // Wertdatentyp
            MyStruct s = new MyStruct();
        }
    }
}
