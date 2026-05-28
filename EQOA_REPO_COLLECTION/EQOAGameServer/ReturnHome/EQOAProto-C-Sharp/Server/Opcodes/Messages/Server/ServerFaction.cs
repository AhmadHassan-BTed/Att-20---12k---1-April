using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    public static class ServerFaction
    {
        public static void ServerSendFaction(Session session)
        {
            /*Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.Something2);//0 removes from group, no message //1 Group leader ejected you //2 Group disbanded //3 not invited to this group //4 Group is full //5 no message, not in group
            BufferWriter writer = new BufferWriter(message.Span); //7 Group disbanded

            writer.Write(message.Opcode);
            writer.Write<byte>(7);

            message.Size = writer.Position;
            session.sessionQueue.Add(message);*/
        }
    }
}
