using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DelegatesAndEvents
{
    class Program
    {
        static void OnElapsedSeconds(int numberOfSeconds)
        {
            Console.WriteLine($"Es ist {numberOfSeconds} Uhr");
        }

        static void Main(string[] args)
        {
            Clock clock = new Clock();
            The_Think3r the_Think3r = new The_Think3r();
            JackCimberly jackCimberly = new JackCimberly();

            clock.onSecondsElapsed += OnElapsedSeconds;
            clock.onSecondsElapsed += the_Think3r.OnElapsedSeconds;
            clock.onSecondsElapsed += jackCimberly.OnElapsedSeconds;
            // Auch Lamb(a)das funktionieren:
            clock.onSecondsElapsed += seconds => Console.WriteLine($"Das doppelte der aktuellen Uhrzeit wäre {2 * seconds}");
            // clock.onSecondsElapsed += Console.WriteLine;

            // Wir lassen die Uhr für 14 Sekunden laufen
            clock.Run(14.0);
            Console.WriteLine("Programmende");
            Console.ReadLine();
        }
    }
}
