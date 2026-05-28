using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    public class ServerTime
    {
        public static void Time(Session session)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.Time);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            //Get our timestamp opcode in queue
            writer.Write(DNP3Creation.CreateDNP3TimeStamp());

            message.Size = writer.Position;
            session.sessionQueue.Add(message);
        }
    }
}
