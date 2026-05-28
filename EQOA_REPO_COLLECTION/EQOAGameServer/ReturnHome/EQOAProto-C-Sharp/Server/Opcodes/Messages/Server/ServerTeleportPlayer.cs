using ReturnHome.Server.EntityObject;
using ReturnHome.Server.Managers;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    class ServerTeleportPlayer
    {
        //TODO: Account for group teleporting somehow
        public static void TeleportPlayer(Session session, World world, float x, float y, float z, float facing)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.Teleport);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write(world); //world
            writer.Write(x); // x
            writer.Write(y); // y
            writer.Write(z); // z
            writer.Write(facing); //Facing
            writer.Position += 8;
            writer.Write(session.MyCharacter.Teleportcounter++); //counter

            message.Size = writer.Position;
            session.sessionQueue.Add(message);

            //Reset player channels
            session.rdpCommIn.connectionData.DisableChannels();

            //Check if player is changing maps, if so remove them from the current map
            if (session.MyCharacter.World != world)
            {
                session.MyCharacter.ExpectedWorld = world;
                MapManager.RemoveObject(session.MyCharacter);
            }
        }
    }
}
