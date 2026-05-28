using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Client
{
    class ClientChangeChatMode
    {
        public static void ChangeChatMode(Session session, Message ClientPacket)
        {
            //Just accept and change chat mode
            session.MyCharacter.chatMode = ClientPacket.message.Span[0];
        }
    }
}
