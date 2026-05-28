using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    class ServerUpdatePlayerXPandLevel
    {
        public static void UpdatePlayerXPandLevel(Session session, int Level, int totalXPEarned)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.GrantXP);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write7BitEncodedInt64(Level);
            writer.Write7BitEncodedInt64(totalXPEarned);

            message.Size = writer.Position;
            session.sessionQueue.Add(message);
        }
    }
}
