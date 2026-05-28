using System;
using ReturnHome.Server.Network;
using ReturnHome.Server.Opcodes.Messages.Server;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Client
{
    public static class ClientClassMastery
    {
        public static void ProcessClassMastery(Session session, PacketMessage ClientPacket)
        {
            BufferReader reader = new(ClientPacket.Data.Span);

            byte subOpcode = reader.Read<byte>();
            int counter = reader.Read<int>();

            switch(subOpcode)
            {
                //set cm xp%
                case 1:
                    break;

                //request cm menu
                case 4:
                    Console.WriteLine("Sending CM Menu");
                    ServerCMMenu.SendCMMenu(session);
                    break;
            }
        }
    }
}
