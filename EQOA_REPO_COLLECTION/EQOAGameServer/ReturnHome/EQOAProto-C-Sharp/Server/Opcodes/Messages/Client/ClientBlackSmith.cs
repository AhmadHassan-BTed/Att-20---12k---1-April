using System;

using ReturnHome.Server.EntityObject.Items;
using ReturnHome.Server.Network;
using ReturnHome.Server.Opcodes.Chat;
using ReturnHome.Server.Opcodes.Messages.Server;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Client
{
    public class ClientBlackSmith
    {
        //TODO: We would utilize this to "free" character from a busy status of repairing gear
        public static void CloseBlackSmithMenu(Session session, Message ClientPacket)
        {

        }

        public static void BlackSmithRepairGear(Session session, Message ClientPacket)
        {
            BufferReader reader = new(ClientPacket.Span);

            //Item slot to repair? Verify
            byte itemSlot = (byte)reader.Read<uint>();

            //unknown? From limited testing doesn't seem to change
            uint unk = reader.Read<uint>();

            //Check and verify, perform repair
            if(session.MyCharacter.Inventory.TryRetrieveItem(itemSlot, out ClientItemWrapper i, out byte t))
            {
                Item item = i.item;
                uint repairCost = (uint)(i.item.Pattern.ItemCost - Math.Ceiling((item.RemainingHP * 1.0f) / item.Pattern.Maxhp * item.Pattern.ItemCost));

                if (repairCost <= session.MyCharacter.Inventory.Tunar)
                {
                    //remove repair costs from tunar
                    session.MyCharacter.Inventory.RemoveTunar((int)repairCost);

                    //repair the item
                    //TODO: Create Server method to repair gear, adjust items currenr gp
                    // tie this into item classes and be sure to pass the session in?
                    item.RemainingHP = item.Pattern.Maxhp;

                    ServerAdjustItemHP.AdjustItemHP(session, item, i.key);
                    return;
                }

                //Not enough tunar, let client know
                ChatMessage.DistributeSpecificMessageAndColor(session, "You can't afford that.", new byte[] { 0xFF, 0x00, 0x00, 0x00 });
            }
        }
    }
}
