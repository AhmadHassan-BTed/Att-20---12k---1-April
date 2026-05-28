using System;
using ReturnHome.Server.Network;
using ReturnHome.Server.Opcodes.Messages.Server;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Client
{
    public static class ClientClassMastery
    {
        public static void ProcessClassMastery(Session session, Message ClientPacket)
        {
            BufferReader reader = new(ClientPacket.message.Span);

            byte subOpcode = reader.Read<byte>();
            int counter = reader.Read<int>();

            switch(subOpcode)
            {
                //set cm xp%
                case 1:
                    break;

                //request cm menu
                case 4:
                    ServerCMMenu.SendCMMenu(session);
                    break;
            }
        }
    }
}
