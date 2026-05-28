using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    public static class ServerInventoryItemArrange
    {
		public static void InventoryItemArrange(Session session, byte clientItem1, byte clientItem2)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.ArrangeItem);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);

            //send slot swap back to player to confirm
            writer.Write(clientItem1);
            writer.Write(clientItem2);

            message.Size = writer.Position;
            //Send arrange op code back to player
            session.sessionQueue.Add(message);
        }
    }
}
