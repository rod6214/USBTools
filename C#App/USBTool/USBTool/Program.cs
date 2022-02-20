﻿namespace USBTool
{
    using System;
    using System.Text;
    using System.Collections.Generic;

    class Program
    {

        
        static void Main(string[] args)
        {
            writeExample();
            readExample();
        }

        static void readExample()
        {
            IntPtr writeHandler = IntPtr.Zero;
            IntPtr readHandler = IntPtr.Zero;

            byte[] data = new byte[66];
            List<byte> realList = new List<byte>();
            int read = 0;

            if (USB.Find_This_Device(0x048d, 0x003f, 0, ref readHandler, ref writeHandler))
            {
                if (USB.ReadFile(readHandler, data, 65, ref read, 0))
                {
                    for (int i = 0; i < data.Length; i++)
                    {
                        byte c = data[i + 1];
                        
                        if (c == 0) 
                        {
                            break;
                        }

                        realList.Add(c);
                    }

                    var message = Encoding.ASCII.GetString(realList.ToArray());
                    Console.WriteLine("Data read: {0}", message);
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

                data[1] = 15;
                data[2] = 4;
                data[3] = 8;
                data[32] = 5;

                int written = 0;

                if (USB.WriteFile(writeHandler, data, 65, ref written, 0))
                {
                    Console.WriteLine("Data written");
                }
            }
        }
    }
}
