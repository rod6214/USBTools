namespace USBTool
{
    using System;
    using System.Text;
    using System.Collections.Generic;
    using System.Threading;
    using System.Threading.Tasks;

    class Program
    {

        
        static async Task Main(string[] args)
        {
            var client = new USBClient(0x048d, 0x003f);

            while(true) 
            {
                Console.Write("DevTool\\>");
                var commmand = System.Console.ReadLine();
                var result = await client.GetAsync(commmand);
                Console.WriteLine(result);
            }
        }
    }
}
