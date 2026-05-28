using ReturnHome.Server.EntityObject.Items;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    public static class ServerAddInventoryItemQuantity
    {
		public static void AddInventoryItemQuantity(Session session, Item item)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.AddInvItem);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            item.DumpItem(ref writer);

            message.Size = writer.Position;
            session.sessionQueue.Add(message);
        }
    }
}
