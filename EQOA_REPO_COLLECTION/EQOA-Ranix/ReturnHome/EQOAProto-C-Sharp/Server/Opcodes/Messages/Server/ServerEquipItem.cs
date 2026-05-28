using ReturnHome.Server.EntityObject.Items;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    public static class ServerEquipItem
    {
        public static void ProcessServerEquipItem(Session session, Item item)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.EquipItem);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);

            writer.Write(item.ClientIndex);
            writer.Write(item.EquipLocation);

            message.Size = writer.Position;
            session.sessionQueue.Add(message);
        }
    }
}
