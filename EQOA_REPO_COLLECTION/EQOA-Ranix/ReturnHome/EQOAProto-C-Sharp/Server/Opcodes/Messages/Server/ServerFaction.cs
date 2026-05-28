using System;
using System.Text;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    public static class ServerFaction
    {
        public static void ServerSendFaction(Session session)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.FactionStuff);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            //throw new Exception();
            //writer.Write7BitEncodedInt64(200);
            //writer.Write(673457636);
            //writer.Write((byte)0x0C);
            //writer.Write((byte)12);
            writer.WriteString(Encoding.Unicode, "Test");

            message.Size = writer.Position;
            session.sessionQueue.Add(message);
        }
    }
}
