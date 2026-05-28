using System.Linq;
using System.Text;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    public static class ServerClientMenu
    {
        public static void ChangeMenu(Session session, byte type, byte code)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.AdminCode);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write(type);
            writer.Write7BitEncodedInt64(code);

            message.Size = writer.Position;
            ///Handles packing message into outgoing packet
            session.sessionQueue.Add(message);
        }
    }
}
