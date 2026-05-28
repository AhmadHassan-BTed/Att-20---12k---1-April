using System.Text;
using ReturnHome.Server.EntityObject.Grouping;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.Managers;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Chat
{
    public static class GroupChat
    {
        public static void ProcessGroupChat(Session session, string chatMessage)
        {

            chatMessage = $"{session.MyCharacter.CharName} tells the group: " + chatMessage;

            Group g = GroupManager.QueryForGroup(session.MyCharacter.GroupID);

            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.GroupChat);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write(session.MyCharacter.ObjectID);
            writer.WriteString(Encoding.Unicode, chatMessage);

            message.Size = writer.Position;
            //Loop over entity list, if any are players, distribute the message
            foreach (Character c in g.GroupList)
            {
                if (c != null)
                {
                    c.characterSession.sessionQueue.Add(message);
                }
            }
        }
    }
}
