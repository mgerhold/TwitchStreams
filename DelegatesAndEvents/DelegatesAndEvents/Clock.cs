using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace DelegatesAndEvents
{
    class Clock
    {
        private DateTime startingTime;

        private double elapsedTime = 0.0;

        // Definition des Delegaten-Datentyps
        public delegate void OnSecondsElapsedDelegate(int numElapsedSeconds);
        // in der folgenden Variable kann man nun Referenzen (= Verweise) auf Methoden
        // speichern, die der oben festgelegten "Signatur" entsprechen, also
        // void Funktionsname(int)
        public event OnSecondsElapsedDelegate onSecondsElapsed = null;
        
        // Oder in kürzer und schöner:
        // public event Action<int> onSecondsElapsed;

        public double ElapsedSeconds { get => (DateTime.Now - startingTime).TotalSeconds; }

        public Clock()
        {
            startingTime = DateTime.Now;
        }

        public void Run(double durationInSeconds)
        {
            double newElapsedTime;
            while ((newElapsedTime = (DateTime.Now - startingTime).TotalSeconds) < durationInSeconds)
            {
                if ((int)newElapsedTime != (int)elapsedTime)
                {
                    onSecondsElapsed?.Invoke((int)newElapsedTime);
                    // Oder in länger:
                    /*if (onSecondsElapsed != null)
                    {
                        onSecondsElapsed((int)newElapsedTime);
                    }*/
                }                
                elapsedTime = newElapsedTime;
                Thread.Sleep(16);                
            }
        }
    }
}
