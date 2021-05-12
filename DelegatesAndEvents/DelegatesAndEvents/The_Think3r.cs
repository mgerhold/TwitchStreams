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
            if (numberOfSeconds == 13)
            {
                Console.WriteLine("Ich unterziehe, die Innenseiten meiner Augenlieder eine Rißprüfung!");
            }
            else if (numberOfSeconds == 15)
            {
                Console.WriteLine("Rißprüfung erfolgreich abgeschlossen! Resultat: Keine Riße gefunden!");
            }
        }

    }
}
