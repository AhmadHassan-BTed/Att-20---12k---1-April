using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    class ServerCharacterNameTaken
    {
        public static void CharacterNameTaken(Session session)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.NameTaken);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);

            message.Size = writer.Position;
            //Log character name taken and send out RDP message to pop up that name is taken.
            //Console.WriteLine("Character Name Already Taken");                //Send Message
            session.sessionQueue.Add(message);
        }
    }
}
