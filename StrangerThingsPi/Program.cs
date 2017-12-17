using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StrangerThingsPi
{
    class Program
    {
        static SerialPort sp;

        static void Main(string[] args)
        {
            sp = new SerialPort("/dev/ttyAMA0", 115200, Parity.None, 8, StopBits.One);
            sp.Open();
            sp.Write("dana hanna");
            sp.Close();
        }
    }

}
