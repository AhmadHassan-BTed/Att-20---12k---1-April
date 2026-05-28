using System.Collections.Generic;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.Managers;
using ReturnHome.Server.Network;
using ReturnHome.Server.Opcodes.Messages.Server;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Client
{
    public static class ClientWhoListRequest
    {
        public static void ProcessWhoList(Session session, Message ClientPacket)
        {
            BufferReader reader = new(ClientPacket.message.Span);
            byte temp1 = reader.Read<byte>();
            byte temp2 = reader.Read<byte>();

            //For now send a global response
            List<Character> temp = PlayerManager.QueryForAllPlayers();

            ServerWhoListResponse.WhoListResponse(session, temp, temp2);
        }
    }
}
