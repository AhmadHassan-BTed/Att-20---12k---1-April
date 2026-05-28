using System;

namespace ReturnHome.Server.Network
{
    public abstract class PacketMessage
    {
        public static int MaxMessageSize { get; } = 1024;
        public PacketMessageHeader Header { get; } = new PacketMessageHeader();
        public ReadOnlyMemory<byte> Data { get; protected set; }
    }
}
