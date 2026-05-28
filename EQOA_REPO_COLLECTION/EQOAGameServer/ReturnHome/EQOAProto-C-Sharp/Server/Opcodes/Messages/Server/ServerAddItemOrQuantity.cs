using System;
using ReturnHome.Server.EntityObject.Items;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    public static class ServerAddItemOrQuantity
    {
		public static void AddItemOrQuantity(Session session, GameOpcode opcode, Item item, int quantity)
        {
            Message message = Message.Create(MessageType.ReliableMessage, opcode);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            int qty = item.StackLeft;
            item.StackLeft = quantity;
            item.DumpItem(ref writer, item.ClientIndex);
            item.StackLeft = qty;
            message.Size = writer.Position;
            session.sessionQueue.Add(message);
        }
    }
}
