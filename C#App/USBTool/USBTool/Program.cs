namespace USBTool
{
    using System;
    using System.Text;

    class Program
    {

        
        static void Main(string[] args)
        {
            readExample();
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
                    if (USB.ReadFile(readHandler, data, 3, ref read, 0)) 
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
                //byte[] data = Encoding.UTF8.GetBytes("\02\0  ");
                //byte[] data = new byte[6] { 0, 9, 5, 3, 0, 0 };
                byte[] buffer = new byte[30];
                Console.WriteLine("Data read");
                //int written = 0;
                //int read = 0;

                //if (USB.HidD_SetFeature(readHandler, buffer, 28))
                //{
                //    Console.WriteLine("Data written");
                //}

                //if (USB.ReadFile(readHandler, buffer, 2, ref read, 0))
                //{

                //    Console.WriteLine("Data read");
                //}

                //if (USB.WriteFile(writeHandler, data, 3, ref written, 0)) {

                //    Console.WriteLine("Data written");
                //}
            }
        }
    }
}
