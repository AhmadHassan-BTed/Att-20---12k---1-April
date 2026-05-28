using System.Text;

using ReturnHome.Server.EntityObject;
using ReturnHome.Server.Managers;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    class ServerTriggerMerchantMenu
    {
        public static void TriggerMerchantMenu(Session session, Entity npc)
        {
            int unknownInt = 100;

            Message message = new Message(MessageType.SegmentReliableMessage, GameOpcode.MerchantBox);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write(npc.ObjectID);
            writer.Write7BitEncodedInt64(unknownInt);
            writer.Write7BitEncodedInt64(unknownInt); //item cost multiplier ((float)unknownInt /100) * itemcost = total it will end up

            writer.Write7BitEncodedInt64(npc.Inventory == null ? 0 : npc.Inventory.Count);
            writer.Write(npc.Inventory == null ? 0 : npc.Inventory.Count);

            if (npc.Inventory != null)
            {
                for (int i = 0; i < npc.Inventory.Count; ++i)
                    npc.Inventory.itemContainer[i].item.DumpItem(ref writer, npc.Inventory.itemContainer[i].key);
            }

            writer.WriteString(Encoding.Unicode, EventManager.GetMerchantDialogue(session, npc)); ;
            message.Size = writer.Position;
            session.sessionQueue.Add(message);
        }
    }
}
