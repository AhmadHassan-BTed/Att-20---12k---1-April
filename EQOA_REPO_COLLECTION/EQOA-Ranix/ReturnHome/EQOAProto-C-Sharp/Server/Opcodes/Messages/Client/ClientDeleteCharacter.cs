using ReturnHome.Database.SQL;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Client
{
    class ClientDeleteCharacter
    {
        public static void DeleteCharacter(Session session, PacketMessage ClientPacket)
        {
            CharacterSQL deletedCharacter = new CharacterSQL();

            BufferReader reader = new(ClientPacket.Data.Span);
            //Passes in packet with ServerID on it, will grab, transform and return ServerID while also removing packet bytes
            int clientServID = (int)reader.Read7BitEncodedInt64();

            //Call SQL delete method to actually process the delete.
            deletedCharacter.DeleteCharacter(clientServID, session);

            //Close SQL connection
            deletedCharacter.CloseConnection();
        }
    }
}
