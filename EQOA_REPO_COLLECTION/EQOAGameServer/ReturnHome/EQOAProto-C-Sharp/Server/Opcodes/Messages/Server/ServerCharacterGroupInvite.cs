using System.Text;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    public static class ServerCharacterGroupInvite
    {
        public static void GroupInvite(Session session, Character c)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.GroupInviteResponse);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write(session.MyCharacter.ObjectID);
            writer.Write(c.ObjectID);
            writer.WriteString(Encoding.UTF8, session.MyCharacter.CharName);

            message.Size = writer.Position;
            c.characterSession.sessionQueue.Add(message);
        }
    }
}
