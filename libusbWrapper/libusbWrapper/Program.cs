using System;
using System.Runtime.InteropServices;

namespace libusbWrapper
{
    class Program
    {
        [StructLayout(LayoutKind.Sequential)]
		public struct libusb_device_descriptor
		{
			public byte bLength;
			public byte bDescriptorType;
			public ushort bcdUSB;
			public byte bDeviceClass;
			public byte bDeviceSubClass;
			public byte bDeviceProtocol;
			public byte bMaxPacketSize0;
			public ushort idVendor;
			public ushort idProduct;
			public ushort bcdDevice;
			public byte iManufacturer;
			public byte iProduct;
			public byte iSerialNumber;
			public byte bNumConfigurations;
		}
		
		[DllImport("libusb-1.0.dll")]
        static public extern int libusb_init(IntPtr context);

        [DllImport("libusb-1.0.dll")]
        static public extern int libusb_get_device_list(IntPtr ctx, IntPtr list);

        [DllImport("libusb-1.0.dll")]
        static public extern void libusb_free_device_list(IntPtr list, int unref_devices);

        [DllImport("libusb-1.0.dll")]
        static public extern int libusb_get_device_descriptor(IntPtr dev, ref libusb_device_descriptor desc);

        [DllImport("libusb-1.0.dll")]
        static public extern int libusb_open(IntPtr dev, IntPtr dev_handle);

        [DllImport("libusb-1.0.dll")]
        static public extern int libusb_interrupt_transfer(IntPtr dev_handle,
                                                            ushort endpoint,
                                                            byte[] data,
                                                            int length,
                                                            ref int transferred,
                                                            int timeout
                                                            );

        [DllImport("libusb-1.0.dll")]
        static public extern int libusb_bulk_transfer(IntPtr dev_handle,
                                                            ushort endpoint,
                                                            byte[] data,
                                                            int length,
                                                            ref int transferred,
                                                            int timeout
                                                            );
        [DllImport("libusb-1.0.dll")]
        static public extern IntPtr libusb_open_device_with_vid_pid(IntPtr ctx,
        ushort vendor_id,
        ushort product_id
        );

        static void Main(string[] args)
        {
            if (libusb_init(IntPtr.Zero) == 0) 
            {
                libusb_device_descriptor desc = new libusb_device_descriptor();
                
                IntPtr devs = Marshal.AllocHGlobal(8);

                int cnt = libusb_get_device_list(IntPtr.Zero, devs);

                long addr = Marshal.ReadInt64(devs);

                for (int i = 0; i < cnt; i++)
                {
                    IntPtr nPtr = new IntPtr(addr);
                    IntPtr dev = IntPtr.Add(nPtr, i * 8);
                    IntPtr ndev = new IntPtr(Marshal.ReadInt64(dev));

                    if (libusb_get_device_descriptor(ndev, ref desc) == 0)
                    {
                        if (desc.idVendor == 0x048D && desc.idProduct == 0x003F)
                        {
                            Console.WriteLine("Device found");
                            IntPtr devHandle = Marshal.AllocHGlobal(8);

                            IntPtr handleValue = libusb_open_device_with_vid_pid(IntPtr.Zero, 0x048D, 0x003F);

                            byte[] data = new byte[66];

                            data[1] = 9;
                            data[64] = 7;

                            int written = 0;

                            int tt = libusb_interrupt_transfer(handleValue, 0x81, data, 65, ref written, 5000);

                            //if (libusb_open(ndev, devHandle) == 0) 
                            //{
                            //    byte[] data = new byte[66];

                            //    data[1] = 9;
                            //    data[64] = 7;

                            //    int written = 0;

                            //    //IntPtr handleValue = new IntPtr(Marshal.ReadInt64(devHandle));
                            //    int tt = libusb_interrupt_transfer(handleValue, 0x01, data, 65, ref written, 0);
                            //    if (libusb_interrupt_transfer(handleValue, 0x01, data, 65, ref written, 0) == 0) 
                            //    {
                            //        Console.WriteLine("Data transferred");
                            //    }
                            //}
                            break;
                        }
                    }
                    else
                        throw new Exception("Couldn't get the device descriptor");
                }

            }
        }
    }
}
