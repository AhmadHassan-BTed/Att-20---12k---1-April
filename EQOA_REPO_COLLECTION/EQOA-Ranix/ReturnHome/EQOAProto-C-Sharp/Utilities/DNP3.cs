using System;
using System.Collections.Generic;

namespace ReturnHome.Utilities
{
    class DNP3Creation
    {
        /// <summary>
        /// Should we just return our DNP3 Byte array here?
        /// </summary>
        /// <returns></returns>
        public static ulong CreateDNP3(int val)
        { 
            ///Gets our time since epoch
            ulong Date1 = DateTime.Now.GetUnixEpoch() + (ulong)val;

            ///return time * 1000 for milliseconds
            return Date1 * 1000;
        }

        public static byte[] CreateDNP3TimeStamp()
        {
            List<byte> newList = new List<byte> { };
            newList.AddRange(BitConverter.GetBytes(CreateDNP3(0)));
            newList.AddRange(BitConverter.GetBytes(CreateDNP3(0) + 1350000));

            return newList.ToArray();
        }

        /// <summary>
        /// Create DNP3, but then only return 4 bytes for Session information
        /// </summary>
        /// <returns></returns>
        public static uint DNP3Session()
        {
            ///Gets our time since epoch
            ulong Date1 = DateTime.Now.GetUnixEpoch();

            return (uint)(((Date1 * 1000) << 32) >> 32);
        }

    }
}
