// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

using System;
using System.Runtime.InteropServices;

namespace ReturnHome.PacketProcessing
{
    [StructLayout(LayoutKind.Sequential, Size = 6)]
    public readonly struct MessageHeaderReliableLong
    {
        public readonly ushort Type;
        public readonly ushort Length;
        public readonly ushort Number;

        public MessageHeaderReliableLong(ushort type, ushort length, ushort number)
        {
            Type = type;
            Length = length;
            Number = number;
        }

        public byte[] getBytes()
        {
            int size = Marshal.SizeOf(this);
            byte[] arr = new byte[size];

            IntPtr ptr = Marshal.AllocHGlobal(size);
            Marshal.StructureToPtr(this, ptr, true);
            Marshal.Copy(ptr, arr, 0, size);
            Marshal.FreeHGlobal(ptr);
            return arr;
        }
    }

    [StructLayout(LayoutKind.Sequential, Size = 4)]
    public readonly struct MessageHeaderReliableShort
    {
        public readonly byte Type;
        public readonly byte Length;
        public readonly ushort Number;

        public MessageHeaderReliableShort(byte type, byte length, ushort number)
        {
            Type = type;
            Length = length;
            Number = number;
        }

        public byte[] getBytes()
        {
            int size = Marshal.SizeOf(this);
            byte[] arr = new byte[size];

            IntPtr ptr = Marshal.AllocHGlobal(size);
            Marshal.StructureToPtr(this, ptr, true);
            Marshal.Copy(ptr, arr, 0, size);
            Marshal.FreeHGlobal(ptr);
            return arr;
        }
    }

    [StructLayout(LayoutKind.Sequential, Size = 4)]
    public readonly struct MessageHeaderUnreliableLong
    {
        public readonly ushort Type;
        public readonly ushort Length;
        public readonly ushort Number;

        public MessageHeaderUnreliableLong(ushort type, ushort length, ushort number)
        {
            Type = type;
            Length = length;
            Number = number;
        }

        public byte[] getBytes()
        {
            int size = Marshal.SizeOf(this);
            byte[] arr = new byte[size];

            IntPtr ptr = Marshal.AllocHGlobal(size);
            Marshal.StructureToPtr(this, ptr, true);
            Marshal.Copy(ptr, arr, 0, size);
            Marshal.FreeHGlobal(ptr);
            return arr;
        }
    }

    [StructLayout(LayoutKind.Sequential, Size = 2)]
    public readonly struct MessageHeaderUnreliableShort
    {
        public readonly byte Type;
        public readonly byte Length;
        public readonly ushort Number;

        public MessageHeaderUnreliableShort(byte type, byte length, ushort number)
        {
            Type = type;
            Length = length;
            Number = number;
        }

        public byte[] getBytes()
        {
            int size = Marshal.SizeOf(this);
            byte[] arr = new byte[size];

            IntPtr ptr = Marshal.AllocHGlobal(size);
            Marshal.StructureToPtr(this, ptr, true);
            Marshal.Copy(ptr, arr, 0, size);
            Marshal.FreeHGlobal(ptr);
            return arr;
        }
    }
}
