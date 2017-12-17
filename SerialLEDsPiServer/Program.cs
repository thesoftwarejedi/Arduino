using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Runtime.InteropServices;
using System.Text;

namespace SerialLEDsPiServer
{
    class Program
    {
        static SerialPort sp;
        static RGB[] leds;
        static byte[] go = new byte[] { 0xFF };

        static void Main(string[] args)
        {
            leds = new RGB[300];
            RGB black = Color(0, 0, 0);
            RGB white = Color(254, 254, 254);
            sp = new SerialPort("/dev/ttyAMA0", 38400, Parity.None, 8, StopBits.One);
            sp.Open();

            for (int i = 0; i < leds.Length; i++)
                leds[i] = black;

            Console.WriteLine(1);

            for (int i = 0; i < leds.Length; i++)
            {
                Console.WriteLine(i);
                leds[i] = white;
                Write();
            }

            Console.WriteLine(2);

            for (int i = 0; i < leds.Length; i++)
            {
                leds[i] = black;
                Write();
            }
        }

        private static void Go()
        {
            sp.Write(go, 0, 1);
        }

        private static void Write()
        {
            Write(leds);
            Go();
        }

        private static void Write(RGB[] b)
        {
            foreach (RGB bs in b)
                Write(bs);
        }

        private static void Write(RGB b)
        {
            byte[] buffer = new byte[] { (byte)b.r, (byte)b.g, (byte)b.b };
            sp.Write(buffer, 0, 3);
        }

        static RGB Color(byte r, byte g, byte b)
        {
            return new RGB((byte)r, (byte)g, (byte)b);
        }
    }

    struct RGB
    {
        public RGB(byte r, byte g, byte b)
        {
            this.r = r;
            this.g = g;
            this.b = b;
        }
        public byte r;
        public byte g;
        public byte b;
    }
}
