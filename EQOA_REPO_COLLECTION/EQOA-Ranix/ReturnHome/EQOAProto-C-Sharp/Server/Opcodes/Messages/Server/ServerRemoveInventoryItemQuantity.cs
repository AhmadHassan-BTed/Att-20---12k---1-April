using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    public static class ServerRemoveInventoryItemQuantity
    {
        /*<summary>
         * This method will send the opcode to update item quantity in inventory, including deleting it if entire quantity is removed.
         * </summary>
         */
		public static void RemoveInventoryItemQuantity(Session session, int StackLeft, byte clientIndex)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.RemoveInvItem);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.Write(clientIndex);
            writer.Write((byte)1);
            writer.Write7BitEncodedInt64(StackLeft);

            message.Size = writer.Position;
            session.sessionQueue.Add(message);
        }
    }
}
