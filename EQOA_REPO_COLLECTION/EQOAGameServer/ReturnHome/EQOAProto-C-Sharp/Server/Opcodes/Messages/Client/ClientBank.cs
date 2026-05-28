using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Client
{
    public static class ClientBank
    {
        public static void DepositOrTakeTunar(Session session, Message clientPacket)
        {
            BufferReader reader = new(clientPacket.Span);

            uint targetNPC = reader.Read<uint>();
            uint giveOrTake = (uint)reader.Read7BitEncodedUInt64();
            int transferAmount = (int)reader.Read7BitEncodedInt64();
            session.MyCharacter.BankTunar(targetNPC, giveOrTake, transferAmount);
        }

        public static void DepositOrTakeItem(Session session, Message clientPacket)
        {
            BufferReader reader = new(clientPacket.Span);

            uint targetNPC = reader.Read<uint>();
            byte giveOrTake = reader.Read<byte>();
            byte itemToTransfer = (byte)reader.Read<uint>();
            int qtyToTransfer = (int)reader.Read7BitEncodedInt64();
            session.MyCharacter.TransferItem(giveOrTake, itemToTransfer, qtyToTransfer);
        }
    }
}
