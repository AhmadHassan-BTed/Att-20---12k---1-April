using System.Text;
using ReturnHome.Server.EntityObject.Grouping;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    public static class ServerGroup
    {
        public static void CreateGroup(Session session, Group g)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.CreateGroup);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write(g.GroupID);
            writer.Write(g.Size);

            foreach(Character c in g.GroupList)
                writer.Write(c.ObjectID);

            foreach (Character c in g.GroupList)
                writer.WriteString(Encoding.UTF8, c.CharName);

            message.Size = writer.Position;
            session.sessionQueue.Add(message);
        }

        public static void DisbandGroup(Session session)
        { 
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.RemoveGroupMember);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write(GroupActionEnum.GroupDisbanded);

            message.Size = writer.Position;

            session.sessionQueue.Add(message);
        }

        public static void RemoveGroupMember(Session session, GroupActionEnum e)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.RemoveGroupMember);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write(e);

            message.Size = writer.Position;
            session.sessionQueue.Add(message);
        }

        public static void PlayerAcceptedInvite(Session session, Character c)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.GroupInviteAcceptedMessage);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.WriteString(Encoding.UTF8, c.CharName);

            message.Size = writer.Position;
            session.sessionQueue.Add(message);
        }

        public static void PlayerDeclinedInvite(Session session, GroupActionEnum action, string charName = "")
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.GroupStuff);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write(action);
            if(action != GroupActionEnum.RecipientNotFound || action != GroupActionEnum.GroupIsFull)
                writer.WriteString(Encoding.UTF8, charName);

            message.Size = writer.Position;
            session.sessionQueue.Add(message);
        }
    }
}
