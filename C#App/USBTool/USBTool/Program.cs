namespace USBTool
{
    using System;
    using System.Text;

    class Program
    {

        
        static void Main(string[] args)
        {
            IntPtr writeHandler = IntPtr.Zero;
            IntPtr readHandler = IntPtr.Zero;

            if (USB.Find_This_Device(0x048d, 0x003f, 0, ref readHandler, ref writeHandler))
            {
                //byte[] data = Encoding.UTF8.GetBytes("\02\0  ");
                byte[] data = new byte[6] { 0, 9, 5, 3, 0, 0 };
                byte[] buffer = new byte[2048];

                int written = 0;
                int read = 0;

                //if (USB.ReadFile(readHandler, buffer, 2, ref read, 0))
                //{

                //    Console.WriteLine("Data read");
                //}

                if (USB.WriteFile(writeHandler, data, 3, ref written, 0)) {

                    Console.WriteLine("Data written");
                }
            }
        }
    }
}
