using System.Collections.Generic;
using ReturnHome.Server.EntityObject;
using ReturnHome.Server.EntityObject.Actors;
using ReturnHome.Server.EntityObject.Items;
using ReturnHome.Server.Managers;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    public static class ServerLoot
    {
        public static void ServerLootBox(Session session, List<ClientItemWrapper> Loot)
        {
            Message message = new Message(MessageType.SegmentReliableMessage, GameOpcode.Loot);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write7BitEncodedInt64(Loot == null ? 0 : Loot.Count);
            writer.Write(Loot == null ? 0 : Loot.Count);
            if (Loot != null)
                for (int i = 0; i < Loot.Count; ++i)
                    Loot[i].item.DumpItem(ref writer, Loot[i].key);

            message.Size = writer.Position;
            session.sessionQueue.Add(message);
        }

        public static void ServerLootItem(Session session, byte index, int itemQty)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.ClientLoot);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write(index);
            writer.Write7BitEncodedInt64((byte)1);
            message.Size = writer.Position;
            session.sessionQueue.Add(message);
        }

        public static void ServerLootOptions(Session session, LootOptions Option)
        {
            Message message = new Message(MessageType.ReliableMessage, GameOpcode.LootOptions);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write(Option);

            message.Size = writer.Position;
            session.sessionQueue.Add(message);
        }
    }
}
