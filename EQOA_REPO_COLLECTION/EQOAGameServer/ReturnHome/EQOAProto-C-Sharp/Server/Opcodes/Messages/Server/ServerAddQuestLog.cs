using System.Text;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    class ServerAddQuestLog
    {
        public static void AddQuestLog(Session session, uint questNumber, string questText)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.AddQuestLog);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write(questNumber);
            writer.WriteString(Encoding.Unicode, questText);

            message.Size = writer.Position;
            session.sessionQueue.Add(message);
        }
    }
}
