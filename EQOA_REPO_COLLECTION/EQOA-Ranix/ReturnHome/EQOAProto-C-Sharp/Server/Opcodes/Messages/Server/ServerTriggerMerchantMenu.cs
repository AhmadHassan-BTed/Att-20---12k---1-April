using ReturnHome.Server.EntityObject;
using ReturnHome.Server.EntityObject.Items;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    class ServerTriggerMerchantMenu
    {
        public static void TriggerMerchantMenu(Session session, Entity npc)
        {
            int unknownInt = 200;

            Message message = new Message(MessageType.SegmentReliableMessage, GameOpcode.MerchantBox);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write(npc.ObjectID);
            writer.Write7BitEncodedInt64(unknownInt);
            writer.Write7BitEncodedInt64(unknownInt);

            writer.Write7BitEncodedInt64(npc.Inventory == null ? 0 : npc.Inventory.Count);
            writer.Write(npc.Inventory == null ? 0 : npc.Inventory.Count);
            if (npc.Inventory != null)
            {
                foreach (Item entry in npc.Inventory.itemContainer.Values)
                    entry.DumpItem(ref writer);
            }
            message.Size = writer.Position;
            session.sessionQueue.Add(message);
        }
    }
}
