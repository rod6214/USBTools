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
                byte[] data = Encoding.UTF8.GetBytes("H\0  ");

                int written = 0;

                USB.WriteFile(writeHandler, data, 1, ref written, 0);
                
                //Console.WriteLine("Hello World!");
            }
        }
    }
}
