using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace StrangerThingsPi
{
    class Program
    {
        static SerialPort sp;

        static void Main(string[] args)
        {
            while (true) {
                try
                {
                    sp = new SerialPort("/dev/ttyAMA0", 115200, Parity.None, 8, StopBits.One);
                    sp.Open();

                    HttpListener listen = new HttpListener();
                    listen.Prefixes.Add("http://*:8080/");
                    listen.Start();
                    while (true)
                    {
                        var c = listen.GetContext();
                        var msg = new StreamReader(c.Request.InputStream).ReadToEnd();
                        Console.WriteLine(msg);
                        var body = msg.Split(new string[] { "Body=" }, StringSplitOptions.None)[1].Split('&')[0].Replace('+', ' ').ToLower();
                        Console.WriteLine(body);
                        sp.Write(body);
                    }
                } catch
                {
                    try
                    {
                        sp.Close();
                        Thread.Sleep(2000);
                    } catch { }
                }
            }
        }
    }
}
