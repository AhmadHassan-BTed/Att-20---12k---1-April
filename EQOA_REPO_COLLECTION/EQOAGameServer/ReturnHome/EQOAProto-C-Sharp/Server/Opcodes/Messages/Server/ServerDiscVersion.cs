using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    class ServerDiscVersion
    {
        public static void DiscVersion(Session session, int GameVersion)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.DiscVersion);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write(GameVersion);

            message.Size = writer.Position;
            ///Handles packing message into outgoing packet
            session.sessionQueue.Add(message);
        }
    }
}
