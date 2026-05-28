using ReturnHome.Server.EntityObject.Items;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    public static class ServerRemoveBankItemQuantity
    {
		public static void RemoveBankItemQuantity(Session session, Item item, byte clientIndex)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.RemoveBankItem);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write(clientIndex);
            writer.Write7BitEncodedInt64(item.StackLeft);

            message.Size = writer.Position;
            session.sessionQueue.Add(message);
        }
    }
}
