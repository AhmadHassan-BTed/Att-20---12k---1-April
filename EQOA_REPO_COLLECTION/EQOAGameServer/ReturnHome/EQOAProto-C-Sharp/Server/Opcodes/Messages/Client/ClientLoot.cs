using System;
using ReturnHome.Server.EntityObject;
using ReturnHome.Server.EntityObject.Actors;
using ReturnHome.Server.Managers;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Client
{
    public static class ClientLoot
    {
        public static void ClientOpenLootMenu(Session session, Message clientPacket)
        {
            BufferReader reader = new(clientPacket.message.Span);

            uint target = reader.Read<uint>();

            if (session.MyCharacter.Target == target)
                if(EntityManager.QueryForEntity(target, out Entity temp))
                    ((Actor)temp).corpse.LootCorpse(session);
        }

        public static void ClientLootItem(Session session, Message clientPacket)
        {
            BufferReader reader = new(clientPacket.message.Span);

            byte key = (byte)reader.Read<int>();
            int itemQty = reader.Read<int>();

            if (EntityManager.QueryForEntity(session.MyCharacter.Target, out Entity temp))
                ((Actor)temp).corpse.LootItems(session, key, itemQty);
        }

        public static void ClientLootClose(Session session, Message clientPacket)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.ClientCloseLoot);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            message.Size = writer.Position;
            session.sessionQueue.Add(message);
        }
    }
}
