using ReturnHome.Server.EntityObject.Items;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Client
{
    public static class ClientInteractItem
    {
        public static void ProcessItemInteraction(Session session, Message ClientPacket)
        {
            BufferReader reader = new(ClientPacket.message.Span);

            uint itemIndex = reader.Read<uint>();
            uint target = reader.Read<uint>();

            //byte test = reader.Read<byte>();
            byte equip = (byte)reader.Read7BitEncodedInt64();

            ItemInteraction.InteractItem(session.MyCharacter, itemIndex, equip);
        }

        public static void ArrangeItem(Session session, Message ClientPacket)
        {
            BufferReader reader = new(ClientPacket.Span);

            byte itemSlot1 = (byte)reader.Read<uint>();
            byte itemSlot2 = (byte)reader.Read<uint>();
            session.MyCharacter.ArrangeItem(itemSlot1, itemSlot2);
        }

        public static void MerchantSellItem(Session session, Message ClientPacket)
        {
            BufferReader reader = new(ClientPacket.Span);

            byte itemSlot = (byte)reader.Read<int>();
            int itemQty = (int)reader.Read7BitEncodedInt64();
            uint targetNPC = reader.Read<uint>();
            //We just need to verify the player is talking to a merchant and within range here, just let it work for now
            session.MyCharacter.SellItem(itemSlot, itemQty, targetNPC);
        }

        public static void BuyMerchantItem(Session session, Message ClientPacket)
        {
            BufferReader reader = new(ClientPacket.Span);

            byte itemSlot = (byte)reader.Read7BitEncodedInt64();
            int itemQty = (int)reader.Read7BitEncodedInt64();
            uint targetNPC = reader.Read<uint>();
            session.MyCharacter.MerchantBuy(itemSlot, itemQty, targetNPC);
        }
    }
}
