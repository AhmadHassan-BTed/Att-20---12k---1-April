using ReturnHome.Server.EntityObject.Items;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    public static class ServerAdjustItemHP
    {
        public static void AdjustItemHP( Session session, Item item, int index)
        {

            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.AdjustItemHP);

            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);

            writer.Write(index);
            writer.Write7BitEncodedInt64(item.RemainingHP);

            message.Size = writer.Position;
            session.sessionQueue.Add(message);
        }
    }
}
