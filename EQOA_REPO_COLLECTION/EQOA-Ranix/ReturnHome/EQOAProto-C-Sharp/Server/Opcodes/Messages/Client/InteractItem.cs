using System;
using ReturnHome.Server.EntityObject.Items;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Client
{
    public static class InteractItem
    {
        public static void ProcessItemInteraction(Session session, PacketMessage ClientPacket)
        {
            BufferReader reader = new(ClientPacket.Data.Span);

            uint itemIndex = reader.Read<uint>();
            uint target = reader.Read<uint>();

            //byte test = reader.Read<byte>();
            byte equip = (byte)reader.Read7BitEncodedInt64();

            ItemInteraction.InteractItem(session.MyCharacter, itemIndex, equip);
        }
    }
}
