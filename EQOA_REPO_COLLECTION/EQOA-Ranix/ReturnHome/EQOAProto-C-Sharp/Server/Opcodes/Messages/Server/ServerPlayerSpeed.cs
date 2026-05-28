using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    class ServerPlayerSpeed
    {
        public static void PlayerSpeed(Session session)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.PlayerSpeed);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write(25.0f);

            message.Size = writer.Position;
            //For now send a standard speed
            session.sessionQueue.Add(message);
        }

        public static void PlayerSpeed(Session session, float speed)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.PlayerSpeed);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write(speed);

            message.Size = writer.Position;
            //For now send a standard speed
            session.sessionQueue.Add(message);
        }
    }
}
