using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    class ServerPlayerIgnoreList
    {
        public static void PlayerIgnoreList(Session session)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.IgnoreList);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write((byte)0);

            message.Size = writer.Position;
            //For now send no ignored people
            session.sessionQueue.Add(message);
        }
    }
}
