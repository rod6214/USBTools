﻿namespace USBTool
{
    using System;
    using System.Text;

    class Program
    {

        
        static void Main(string[] args)
        {
            //readExample();
            writeExample();
        }

        static void readExample()
        {
            IntPtr writeHandler = IntPtr.Zero;
            IntPtr readHandler = IntPtr.Zero;

            byte[] data = new byte[16];
            int read = 0;

            if (USB.Find_This_Device(0x048d, 0x003f, 0, ref readHandler, ref writeHandler))
            {
                while (true) 
                {
                    if (USB.ReadFile(readHandler, data, 8, ref read, 0)) 
                    {
                        Console.WriteLine("");
                    }
                }
            }
        }

        static void writeExample()
        {
            IntPtr writeHandler = IntPtr.Zero;
            IntPtr readHandler = IntPtr.Zero;

            if (USB.Find_This_Device(0x048d, 0x003f, 0, ref readHandler, ref writeHandler))
            {
                byte[] data = new byte[66];

                data[1] = 3;
                data[2] = 4;
                data[3] = 8;
                data[32] = 9;

                int written = 0;

                if (USB.WriteFile(writeHandler, data, 32, ref written, 0))
                {
                    Console.WriteLine("Data written");
                }
            }
        }
    }
}
