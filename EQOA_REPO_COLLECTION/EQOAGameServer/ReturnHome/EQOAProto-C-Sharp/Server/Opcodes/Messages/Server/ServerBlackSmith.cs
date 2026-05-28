using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    public class ServerBlackSmith
    {
        public static void ActivateBlackSmithMenu(Session session)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.BlackSmithMenu);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);

            message.Size = writer.Position;
            ///Handles packing message into outgoing packet
            session.sessionQueue.Add(message);
        }
    }
}
