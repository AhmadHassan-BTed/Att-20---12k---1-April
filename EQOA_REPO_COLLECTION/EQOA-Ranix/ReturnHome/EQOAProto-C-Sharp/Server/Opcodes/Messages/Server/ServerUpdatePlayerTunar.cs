using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    public static class ServerUpdatePlayerTunar
    {
		public static void UpdatePlayerTunar(Session session, int tunar)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.PlayerTunar);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write7BitEncodedInt64(tunar);

            message.Size = writer.Position;
            session.sessionQueue.Add(message);
        }
    }
}
