using ReturnHome.Server.Network;
using ReturnHome.Server.Opcodes.Messages.Server;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Client
{
    public static class ClientGenerateRandomCharacterName
    {
        public static void GenerateRandomCharacterName(Session session, Message ClientPacket)
        {
            BufferReader reader = new(ClientPacket.message.Span);

            //This could be useful later if real names are created per race/sex
            ///Get Race Byte
            byte Race = reader.Read<byte>();

            ///Make sure Message number is expected, needs to be in order.
            byte sex = reader.Read<byte>();

            ServerGenerateRandomCharacterName.GenerateRandomCharacterName(session);
        }
    }
}
