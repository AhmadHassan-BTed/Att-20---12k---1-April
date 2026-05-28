using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    public static class ServerUpdateTunar
    {
		public static void UpdateTunar(Session session, GameOpcode OpcodeType, int tunar)
        {
            Message message = Message.Create(MessageType.ReliableMessage, OpcodeType);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write7BitEncodedInt64(tunar);

            message.Size = writer.Position;
            session.sessionQueue.Add(message);
        }
    }
}
