using System.Collections.Generic;
using System.Text;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    public static class ServerWhoListResponse
    {
        public static void WhoListResponse(Session session, List<Character> temp, byte temp2)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.WhoListResponse);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write<byte>(1);
            writer.Write(temp2);
            //Subtrac t ourself from total
            writer.Write7BitEncodedInt64(temp.Count - 1);
            
            foreach(Character c in temp)
            {
                if (c == session.MyCharacter)
                    continue;

                writer.Write((byte)c.EntityRace);
                writer.Write((byte)c.EntityClass);
                writer.Write((byte)c.Level);
                writer.WriteString(Encoding.UTF8, c.CharName);
                writer.Write(c.ObjectID);
            }

            message.Size = writer.Position;
            ///Handles packing message into outgoing packet
            session.sessionQueue.Add(message);
        }
    }
}
