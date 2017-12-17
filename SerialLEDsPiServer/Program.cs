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
            sp = new SerialPort("/dev/ttyAMA0", 614400, Parity.None, 8, StopBits.One);
            sp.Open();

            for (int i = 0; i < leds.Length; i++)
                leds[i] = black;
            Write();

            Console.WriteLine(1);

            for (int i = 0; i < leds.Length; i++)
            {
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

        private static void Flush()
        {
            sp.Write(go, 0, 1);
            Console.WriteLine("flush");
        }

        private static void Write()
        {
            Write(leds);
            Flush();
        }

        private static void Write(RGB[] b)
        {
            byte[] buffer = new byte[b.Length * 3];
            for (int i = 0; i < b.Length; i++)
            {
                buffer[i * 3 + 0] = b[i].r;
                buffer[i * 3 + 1] = b[i].g;
                buffer[i * 3 + 2] = b[i].b;
            }
            sp.Write(buffer, 0, buffer.Length);
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
