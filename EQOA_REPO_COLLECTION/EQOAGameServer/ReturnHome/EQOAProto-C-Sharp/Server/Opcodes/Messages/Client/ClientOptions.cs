using ReturnHome.Server.Network;
using ReturnHome.Server.Opcodes.Messages.Server;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Client
{
    public static class ClientOptions
    {
        public static void ClientMessageOptions(Session session, Message ClientPacket)
        {
            BufferReader reader = new(ClientPacket.message.Span);

            byte state = reader.Read<byte>();
            if (ClientPacket.Opcode == GameOpcode.LootMessages)
                session.MyCharacter.LootMessages = (state == 0);

            if (ClientPacket.Opcode == GameOpcode.FactionMessages) ;
                //session.MyCharacter.FactionMessages = state == 0;
        }
    }
}
