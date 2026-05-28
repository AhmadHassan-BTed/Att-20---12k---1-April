using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Client
{
    class ClientDeleteItem
    {
        public static void DeleteItem(Session session, Message ClientPacket)
        {
            BufferReader reader = new(ClientPacket.message.Span);

            byte itemToDestroy = (byte)reader.Read<uint>();
            int quantityToDestroy = (int)reader.Read<uint>();

            session.MyCharacter.DestroyItem(itemToDestroy, quantityToDestroy);
        }
    }
}
