using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Client
{
    class ClientPlayerTarget
    {
        public static void PlayerTarget(Session session, PacketMessage clientPacket)
        {
            BufferReader reader = new(clientPacket.Data.Span);

            //First 4 bytes is targeting counter, just discarding for now
            _ = reader.Read<uint>();
            uint targetID = reader.Read<uint>();

            session.MyCharacter.Target = targetID;
            session.TargetUpdate();
        }
    }
}
