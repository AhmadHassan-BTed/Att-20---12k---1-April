using System.Text;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    class ServerGenerateRandomCharacterName
    {
        public static void GenerateRandomCharacterName(Session session)
        {
            string Name = RandomName.GenerateName();

            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.RandomName);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write(Name.Length);
            writer.WriteString(Encoding.UTF8, Name);

            message.Size = writer.Position;
            //Send Message
            session.sessionQueue.Add(message);
        }
    }
}
