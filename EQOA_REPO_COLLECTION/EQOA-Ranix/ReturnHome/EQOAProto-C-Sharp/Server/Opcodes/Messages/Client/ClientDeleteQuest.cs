using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Client
{
    class ClientDeleteQuest
    {
        public static void DeleteQuest(Session session, PacketMessage clientPacket)
        {
            BufferReader reader = new(clientPacket.Data.Span);

            byte unknownSection = reader.Read<byte>();
            byte questNumber = reader.Read<byte>();

            Character.DeleteQuest(session, questNumber);
        }
    }
}
