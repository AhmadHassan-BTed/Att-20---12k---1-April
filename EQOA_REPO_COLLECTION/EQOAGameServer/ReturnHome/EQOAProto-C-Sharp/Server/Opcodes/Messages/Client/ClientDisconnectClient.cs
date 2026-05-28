using ReturnHome.Database.SQL;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Client
{
    class ClientDisconnectClient
    {
        public static void DisconnectClient(Session session, Message ClientPacket)
        {
            //By setting termination here, next tick should drop the player currently
            session.PendingTermination = true;

            //Create new handle for mysql connection
            CharacterSQL savePlayerData = new();

            //Call the mysql update query to save player data
            savePlayerData.SavePlayerData(session.MyCharacter);
            savePlayerData.CloseConnection();
        }
    }
}
