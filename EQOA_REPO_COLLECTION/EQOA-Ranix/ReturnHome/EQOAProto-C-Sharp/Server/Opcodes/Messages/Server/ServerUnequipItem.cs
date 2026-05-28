using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    public static class ServerUnequipItem
    {
        public static void ProcessServerUnequipItem(Session session, byte EquipSlot)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.UnequipItem);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);

            writer.Write(EquipSlot);
            message.Size = writer.Position;
            session.sessionQueue.Add(message);
        }
    }
}
