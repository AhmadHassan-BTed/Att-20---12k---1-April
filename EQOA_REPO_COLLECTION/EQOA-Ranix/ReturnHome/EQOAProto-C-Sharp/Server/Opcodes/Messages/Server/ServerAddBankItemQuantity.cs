using System;
using ReturnHome.Server.EntityObject.Items;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    public static class ServerAddBankItemQuantity
    {
		public static void AddBankItemQuantity(Session session, Item item)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.AddBankItem);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);

            item.DumpItem(ref writer);

            message.Size = writer.Position;
            session.sessionQueue.Add(message);
        }
    }
}
