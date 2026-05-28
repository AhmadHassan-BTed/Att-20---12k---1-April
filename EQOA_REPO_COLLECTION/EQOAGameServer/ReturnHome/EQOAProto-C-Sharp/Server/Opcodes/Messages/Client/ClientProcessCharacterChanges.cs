using ReturnHome.Database.SQL;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.Network;
using ReturnHome.Server.Network.Managers;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Client
{
    class ClientProcessCharacterChanges
    {
        public static void ProcessCharacterChanges(Session session, Message ClientPacket)
        {
            BufferReader reader = new(ClientPacket.message.Span);

            //Retrieve CharacterID from client
            //TODO: Save character changes to database
            int ServerID = reader.Read<int>();
            int FaceOption = reader.Read<int>();
            int HairStyle = reader.Read<int>();
            int HairLength = reader.Read<int>();
            int HairColor = reader.Read<int>();

            CharacterSQL cSQL = new();
            //Query Character
            Character MyCharacter = cSQL.AcquireCharacter(session, ServerID);
            cSQL.CloseConnection();
            try
            {
                SessionManager.CreateMemoryDumpSession(session, MyCharacter);
            }
            catch
            {
                Logger.Err($"Unable to create Memory Dump Session for {session.SessionID} : {MyCharacter.CharName}");
            }
        }
    }
}
