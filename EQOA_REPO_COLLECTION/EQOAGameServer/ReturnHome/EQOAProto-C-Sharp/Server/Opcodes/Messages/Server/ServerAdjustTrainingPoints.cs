using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    class ServerAdjustTrainingPoints
    {
        public static void AdjustTrainingPoints(Session session, int AddRemoveTrainingPoints)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.UpdateTrainingPoints);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write7BitEncodedInt64(AddRemoveTrainingPoints);

            message.Size = writer.Position;
            ///Handles packing message into outgoing packet
            session.sessionQueue.Add(message);
        }
    }
}
