using ReturnHome.Database.SQL;
using ReturnHome.Server.Network;

namespace ReturnHome.Server.Opcodes.Messages.Client
{
    class ClientDisconnectClient
    {
        public static void DisconnectClient(Session session, PacketMessage ClientPacket)
        {
            session.PendingTermination = true;
            //Create new handle for mysql connection
            CharacterSQL savePlayerData = new();

            //Call the mysql update query to save player data
            savePlayerData.SavePlayerData(session.MyCharacter, (string)Newtonsoft.Json.JsonConvert.SerializeObject(session.MyCharacter.playerFlags));
            //Actually drop the player's session
            session.DropSession();
        }
    }
}
