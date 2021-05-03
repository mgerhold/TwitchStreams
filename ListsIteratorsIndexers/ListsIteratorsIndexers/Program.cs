using System;

namespace ListsIteratorsIndexers
{
    class Program
    {
        static void Main(string[] args)
        {
            LinkedList<int> numbers = new LinkedList<int>();
            numbers.Append(1);
            numbers.Append(2);
            numbers.Append(3);
            numbers.Append(4);
            numbers.Append(5);
            numbers.Prepend(0);
            numbers.Prepend(99);

            var iterator = numbers.GetEnumerator();
            while (iterator.MoveNext())
            {
                Console.WriteLine(iterator.Current);
            }
            Console.ReadLine();
        }
    }
}
