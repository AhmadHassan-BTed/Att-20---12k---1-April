using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    class ServerOpcode0x07F5
    {
        public static void Opcode0x07F5(Session session)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.Camera2);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write(0x1B);
            message.Size = writer.Position;
            ///Handles packing message into outgoing packet
            session.sessionQueue.Add(message);
        }
    }
}
