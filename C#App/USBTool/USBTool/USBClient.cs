using Microsoft.AspNetCore.Http;
using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;

namespace USBTool
{
    public class USBClient
    {
        private IntPtr writeHandler = IntPtr.Zero;
        private IntPtr readHandler = IntPtr.Zero;

        public USBClient(ushort vendorID, ushort productID) 
        {
            bool created = USB.Find_This_Device(vendorID, productID, 0, ref readHandler, ref writeHandler);

            if (!created) 
            {
                throw new Exception("Device not found.");
            }
        }

        public async Task<string> GetAsync(string command) 
        {
            command = command.Equals("") ? "NULL" : command;
            writeCommand(command);
            var response = await GetResponse();
            return response;
        }

        private async Task<string> GetResponse() 
        {
            var response = await Task.Run(() =>
            {
                string re = "";
                bool exit = false;
                while (!exit)
                {
                    if (readHandler != IntPtr.Zero)
                    {
                        var temp = readCommand();

                        if (string.IsNullOrEmpty(temp))
                        {
                            writeCommand("-");
                            exit = true;
                        }
                        else
                        {
                            re += temp;
                        }
                    }
                }

                return re;
            });

            return response;
        }

        private void writeCommand(string command)
        {
            byte[] data = new byte[66];
            byte[] cmd = Encoding.ASCII.GetBytes(command);

            for (int i = 0; i < cmd.Length; i++)
            {
                data[i + 1] = cmd[i];
            }

            int written = 0;

            if (USB.WriteFile(writeHandler, data, 65, ref written, 0))
            {
                return;
            }

            throw new Exception("Error: It couldn't sent the command, check USB connection.");
        }

        private string readCommand()
        {
            byte[] data = new byte[66];
            List<byte> realList = new List<byte>();
            int read = 0;

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
                return message;
            }

            throw new Exception("Error: It couldn't receive data from the USB pipe, check USB connection.");
        }
    }
}
