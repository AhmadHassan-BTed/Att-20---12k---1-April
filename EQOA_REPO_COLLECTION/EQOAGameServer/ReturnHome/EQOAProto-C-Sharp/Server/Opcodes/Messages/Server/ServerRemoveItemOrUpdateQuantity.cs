using ReturnHome.Server.EntityObject.Items;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    public static class ServerRemoveItemOrUpdateQuantity
    {
		public static void RemoveItemOrUpdateQuantity(Session session, GameOpcode opcode, int change, byte clientIndex)
        {
            Message message = Message.Create(MessageType.ReliableMessage, opcode);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write(clientIndex);

            //What should dictate this? Pretty sure it's a noise client side. 1 is noise, 0 is nothing. Did we get a noise when buying or selling?
            if(GameOpcode.RemoveInvItem == opcode)
                writer.Write((byte)1);

            writer.Write7BitEncodedInt64(change);

            message.Size = writer.Position;
            session.sessionQueue.Add(message);
        }
    }
}
