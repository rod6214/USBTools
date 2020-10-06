using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace USBTool
{
    public class USB
    {
        public static string UnitID = "";

        private const uint GENERIC_READ = 0x80000000;
        private const uint GENERIC_WRITE = 0x40000000;
        private const uint FILE_SHARE_READ = 0x00000001;
        private const uint FILE_SHARE_WRITE = 0x00000002;
        private const uint FILE_FLAG_OVERLAPPED = 0x40000000;
        private const int INVALID_HANDLE_VALUE = -1;
        private const short OPEN_EXISTING = 3;
        // from setupapi.h
        private const short DIGCF_PRESENT = 0x00000002;
        private const short DIGCF_DEVICEINTERFACE = 0x00000010;
        //
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct SP_DEVICE_INTERFACE_DATA
        {
            public int cbSize;
            public System.Guid InterfaceClassGuid;
            public int Flags;
            public IntPtr Reserved;
            //public int Reserved;
            //public int Reserved2;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct SP_DEVICE_INTERFACE_DETAIL_DATA
        {
            public int cbSize;
            public string DevicePath;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct SP_DEVINFO_DATA
        {
            public int cbSize;
            public System.Guid ClassGuid;
            public int DevInst;
            public int Reserved;
        }

        //
        public struct HIDD_ATTRIBUTES
        {
            public int Size;
            public ushort VendorID;
            public ushort ProductID;
            public ushort VersionNumber;
        }
        //
        public struct SECURITY_ATTRIBUTES
        {
            public int nLength;
            public int lpSecurityDescriptor;
            public int bInheritHandle;
        }
        //
        [StructLayout(LayoutKind.Sequential)]
        public struct HIDP_CAPS
        {
            public short Usage;
            public short UsagePage;
            public short InputReportByteLength;
            public short OutputReportByteLength;
            public short FeatureReportByteLength;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 17)]
            public short[] Reserved;
            public short NumberLinkCollectionNodes;
            public short NumberInputButtonCaps;
            public short NumberInputValueCaps;
            public short NumberInputDataIndices;
            public short NumberOutputButtonCaps;
            public short NumberOutputValueCaps;
            public short NumberOutputDataIndices;
            public short NumberFeatureButtonCaps;
            public short NumberFeatureValueCaps;
            public short NumberFeatureDataIndices;

        }
        //
        // DLL imnports
        //
        [DllImport("hid.dll")]
        static public extern void HidD_GetHidGuid(ref System.Guid HidGuid);

        [DllImport("setupapi.dll", CharSet = CharSet.Auto)]
        public static extern IntPtr SetupDiGetClassDevs(ref System.Guid ClassGuid, string Enumerator, int hwndParent, int Flags);

        [DllImport("setupapi.dll", SetLastError = true, CharSet = CharSet.Auto)]
        static public extern int SetupDiEnumDeviceInterfaces(IntPtr DeviceInfoSet, int DeviceInfoData, ref System.Guid InterfaceClassGuid, int MemberIndex, ref SP_DEVICE_INTERFACE_DATA DeviceInterfaceData);

        [DllImport("setupapi.dll", CharSet = CharSet.Auto)]
        static public extern bool SetupDiGetDeviceInterfaceDetail(IntPtr DeviceInfoSet, ref SP_DEVICE_INTERFACE_DATA DeviceInterfaceData, IntPtr DeviceInterfaceDetailData, int DeviceInterfaceDetailDataSize, ref int RequiredSize, IntPtr DeviceInfoData);

        [DllImport("kernel32.dll", CharSet = CharSet.Auto)]
        static public extern IntPtr CreateFile(string lpFileName, uint dwDesiredAccess, uint dwShareMode, ref SECURITY_ATTRIBUTES lpSecurityAttributes, int dwCreationDisposition, uint dwFlagsAndAttributes, int hTemplateFile);

        [DllImport("hid.dll")]
        static public extern int HidD_GetAttributes(IntPtr HidDeviceObject, ref HIDD_ATTRIBUTES Attributes);

        [DllImport("hid.dll")]
        static public extern bool HidD_GetPreparsedData(IntPtr HidDeviceObject, ref IntPtr PreparsedData);

        [DllImport("hid.dll", SetLastError = true, CallingConvention = CallingConvention.StdCall)]
        static public extern bool HidD_GetSerialNumberString(IntPtr HidDeviceObject, IntPtr Buffer, uint BufferLength);

        [DllImport("hid.dll")]
        static public extern int HidP_GetCaps(IntPtr PreparsedData, ref HIDP_CAPS Capabilities);

        [DllImport("setupapi.dll")]
        public static extern int SetupDiDestroyDeviceInfoList(IntPtr DeviceInfoSet);

        [DllImport("hid.dll")]
        static public extern bool HidD_FreePreparsedData(ref IntPtr PreparsedData);

        [DllImport("kernel32.dll")]
        public static extern Int32 CloseHandle(IntPtr hObject);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern unsafe bool WriteFile(
            IntPtr hFile,                       // handle to file
            byte[] Buffer,                      // data buffer
            int numBytesToWrite,                // num of bytes to write
            ref int numBytesWritten,            // number of bytes actually written
            int Overlapped                      // overlapped buffer - not used
            );

        [DllImport("kernel32", SetLastError = true)]
        public static extern unsafe bool ReadFile(
              IntPtr hFile,                       // handle to file
              byte[] Buffer,                      // data buffer
              int NumberOfBytesToRead,            // number of bytes to read
              ref int pNumberOfBytesRead,         // number of bytes read
              int Overlapped                      // overlapped buffer - not used
              );


        public static bool Find_This_Device(ushort p_VendorID,
                                           ushort p_PoductID,
                                           ushort p_index,
                                           ref IntPtr p_ReadHandle,
                                           ref IntPtr p_WriteHandle)
        {
            // Zero based p_index is used to identify which PICkit 2 we wish to talk to
            IntPtr DeviceInfoSet = IntPtr.Zero;
            IntPtr PreparsedDataPointer = IntPtr.Zero;
            HIDP_CAPS Capabilities = new HIDP_CAPS();
            System.Guid HidGuid;
            int Result;
            bool l_found_device;
            ushort l_num_found_devices = 0;
            IntPtr l_temp_handle = IntPtr.Zero;
            int BufferSize = 0;
            SP_DEVICE_INTERFACE_DATA MyDeviceInterfaceData;
            SP_DEVICE_INTERFACE_DETAIL_DATA MyDeviceInterfaceDetailData;
            string SingledevicePathName;
            SECURITY_ATTRIBUTES Security = new SECURITY_ATTRIBUTES();
            HIDD_ATTRIBUTES DeviceAttributes;
            IntPtr InvalidHandle = new IntPtr(-1);
            string unitIDSerial;
            //
            // initialize all
            //
            Security.lpSecurityDescriptor = 0;
            Security.bInheritHandle = System.Convert.ToInt32(true);
            Security.nLength = Marshal.SizeOf(Security);
            //
            HidGuid = Guid.Empty;
            //
            MyDeviceInterfaceData.cbSize = 0;
            MyDeviceInterfaceData.Flags = 0;
            MyDeviceInterfaceData.InterfaceClassGuid = Guid.Empty;
            MyDeviceInterfaceData.Reserved = IntPtr.Zero;
            //MyDeviceInterfaceData.Reserved2 = 0;
            //
            MyDeviceInterfaceDetailData.cbSize = 0;
            MyDeviceInterfaceDetailData.DevicePath = "";
            //
            DeviceAttributes.ProductID = 0;
            DeviceAttributes.Size = 0;
            DeviceAttributes.VendorID = 0;
            DeviceAttributes.VersionNumber = 0;
            //
            l_found_device = false;
            Security.lpSecurityDescriptor = 0;
            Security.bInheritHandle = System.Convert.ToInt32(true);
            Security.nLength = Marshal.SizeOf(Security);

            HidD_GetHidGuid(ref HidGuid);
            DeviceInfoSet = SetupDiGetClassDevs(
                    ref HidGuid,
                    null,
                    0,
                    DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

            MyDeviceInterfaceData.cbSize = Marshal.SizeOf(MyDeviceInterfaceData);
            for (int l_loop = 0; l_loop < 20; l_loop++)
            {
                Result = SetupDiEnumDeviceInterfaces(
                         DeviceInfoSet,
                         0,
                         ref HidGuid,
                         l_loop,
                         ref MyDeviceInterfaceData);
                if (Result != 0)
                {
                    SetupDiGetDeviceInterfaceDetail(DeviceInfoSet, ref MyDeviceInterfaceData, IntPtr.Zero, 0, ref BufferSize, IntPtr.Zero);
                    // Store the structure's size.
                    MyDeviceInterfaceDetailData.cbSize = Marshal.SizeOf(MyDeviceInterfaceDetailData);
                    // Allocate memory for the MyDeviceInterfaceDetailData Structure using the returned buffer size.
                    IntPtr DetailDataBuffer = Marshal.AllocHGlobal(BufferSize);
                    // Store cbSize in the first 4 bytes of the array
                    Marshal.WriteInt32(DetailDataBuffer, 4 + Marshal.SystemDefaultCharSize);
                    //Call SetupDiGetDeviceInterfaceDetail again.  
                    // This time, pass a pointer to DetailDataBuffer and the returned required buffer size.
                    SetupDiGetDeviceInterfaceDetail(DeviceInfoSet, ref MyDeviceInterfaceData, DetailDataBuffer, BufferSize, ref BufferSize, IntPtr.Zero);
                    // Skip over cbsize (4 bytes) to get the address of the devicePathName.
                    IntPtr pdevicePathName = new IntPtr(DetailDataBuffer.ToInt32() + 4);
                    // Get the String containing the devicePathName.
                    SingledevicePathName = Marshal.PtrToStringAuto(pdevicePathName);
                    l_temp_handle = CreateFile(
                                        SingledevicePathName,
                                        GENERIC_READ | GENERIC_WRITE,
                                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                                        ref Security,
                                        OPEN_EXISTING,
                                        0,
                                        0);
                    if (l_temp_handle != InvalidHandle)
                    {
                        // tried to use System.Threading.WaitHandle.InvalidHandle, but had access problems since it's protected
                        // The returned handle is valid, so find out if this is the device we're looking for.
                        // Set the Size property of DeviceAttributes to the number of bytes in the structure.
                        DeviceAttributes.Size = Marshal.SizeOf(DeviceAttributes);
                        Result = HidD_GetAttributes(l_temp_handle, ref DeviceAttributes);
                        if (Result != 0)
                        {
                            if (DeviceAttributes.VendorID == p_VendorID &&
                                DeviceAttributes.ProductID == p_PoductID)
                            {
                                if (l_num_found_devices == p_index)
                                {
                                    IntPtr ptrBuffer = Marshal.AllocHGlobal(126);

                                    // found the correct one
                                    l_found_device = true;

                                    // get serial string (UnitID)
                                    HidD_GetSerialNumberString(l_temp_handle, ptrBuffer, 126);
                                    unitIDSerial = Marshal.PtrToStringUni(ptrBuffer, 64);
                                    Marshal.FreeHGlobal(ptrBuffer);

                                    if (((byte)unitIDSerial[0] == '\t') || (unitIDSerial[0] == 0) || (unitIDSerial[0] == 0x409))
                                    {   // For some reason not clear to me, the blank PK2s return 
                                        // {0x09, 0x04, 0x00} in the first 3 bytes of the SN. The 
                                        // Unicode conversion turns this to character "CYRILLIC 
                                        // CAPITAL LETTER LJE". So, add an extra check to catch
                                        // it.
                                        UnitID = "-";    // blank
                                    }
                                    else
                                    {
                                        UnitID = unitIDSerial;
                                    }
                                    // set return value
                                    p_WriteHandle = l_temp_handle;
                                    // get the device capabilities
                                    HidD_GetPreparsedData(l_temp_handle, ref PreparsedDataPointer);
                                    HidP_GetCaps(PreparsedDataPointer, ref Capabilities);
                                    // now create read handle
                                    p_ReadHandle = CreateFile(
                                                    SingledevicePathName,
                                                    GENERIC_READ | GENERIC_WRITE,
                                                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                                                    ref Security,
                                                    OPEN_EXISTING,
                                                    0,
                                                    0);

                                    // now free up the resource, don't need anymore
                                    HidD_FreePreparsedData(ref PreparsedDataPointer);
                                    // get out of loop
                                    break;
                                }
                                CloseHandle(l_temp_handle);
                                l_num_found_devices++;
                            }
                            else
                            {
                                l_found_device = false;
                                CloseHandle(l_temp_handle);
                            }
                        }
                        else
                        {
                            // There was a problem w/ HidD_GetAttributes
                            l_found_device = false;
                            CloseHandle(l_temp_handle);
                        } // if result == true
                    } // if HIDHandle
                }  // if result == true
            }  // end for
            //Free the memory reserved for the DeviceInfoSet returned by SetupDiGetClassDevs.
            SetupDiDestroyDeviceInfoList(DeviceInfoSet);
            return l_found_device;
        }
    }
}
