using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    class ServerDeleteQuest
    {
        public static void DeleteQuest(Session session, byte questNumber)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.DeleteQuest);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write(0);
            writer.Write(questNumber);

            message.Size = writer.Position;
            session.sessionQueue.Add(message);
        }
    }
}
