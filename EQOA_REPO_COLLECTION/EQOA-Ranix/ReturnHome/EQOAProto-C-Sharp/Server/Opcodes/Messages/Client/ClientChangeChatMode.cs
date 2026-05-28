using ReturnHome.Server.Network;

namespace ReturnHome.Server.Opcodes.Messages.Client
{
    class ClientChangeChatMode
    {
        public static void ChangeChatMode(Session session, PacketMessage ClientPacket)
        {
            //Just accept and change chat mode
            session.MyCharacter.chatMode = ClientPacket.Data.Span[0];
        }
    }
}
