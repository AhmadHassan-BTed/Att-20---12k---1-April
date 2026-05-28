using ReturnHome.Server.Network;
using ReturnHome.Server.Opcodes.Messages.Server;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Client
{
    public static class ClientFaction
    {
        public static void ProcessClientFaction(Session session, Message ClientPacket)
        {
            BufferReader reader = new(ClientPacket.message.Span);
            ServerFaction.ServerSendFaction(session);
        }
    }
}
