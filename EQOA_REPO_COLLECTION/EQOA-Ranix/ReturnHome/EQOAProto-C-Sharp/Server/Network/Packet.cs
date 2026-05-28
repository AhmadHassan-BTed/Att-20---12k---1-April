using System;
using System.Collections.Concurrent;

namespace ReturnHome.Server.Network
{
    public abstract class Packet
    {
        public PacketHeader Header { get; } = new PacketHeader();
        public ConcurrentDictionary<ushort, PacketMessage> Messages { get; } = new ConcurrentDictionary<ushort, PacketMessage>();
        public ClientPacketMessage clientUpdate = null;
    }
}
