using System;
using ReturnHome.Server.EntityObject;
using ReturnHome.Server.EntityObject.Actors;
using ReturnHome.Server.EntityObject.Items;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.Managers;
using ReturnHome.Server.Network;
using ReturnHome.Server.Opcodes.Messages.Server;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Client
{
    class ClientInteractActor
    {
        /*Some thoughts
 * In Dustin's Database under NPCType, certain npc's have a specific value
If it has 0x80+, it is/should be unattackable
Bankers are 0x02. So most likely 0x82 for unattackable and banker
Coachmen are 0x0100, so 0x0180 for coachmen and unattackable
        Coachmen:   0x100
        Blacksmith: 0x8
        Banker:     0x2
        Merchant:   0x1
*/
        //TODO:Verify and check that the interaction type matches npc type
        //TODO: Create a Flag enum for npc types, so we can easily read and set/change npc types
        public static void InteractActor(Session session, Message clientPacket)
        {
            BufferReader reader = new(clientPacket.Span);
            //Get NPC ID
            uint targetNPC = reader.Read<uint>();

            //Get NPC and Verify it is within range to interact
            if (!EntityManager.QueryForEntity(targetNPC, out Entity e))
                return;

            if (!session.MyCharacter.IsWithinRange())
                return;

            switch (clientPacket.Opcode)
            {
                //Merchant popup window, should be trigger some kind of flag if this goes through? Allowing buying/selling?
                case GameOpcode.MerchantDiag:
                    if ((e.EntityType & EntityType.Merchant) == EntityType.Merchant)
                        ServerTriggerMerchantMenu.TriggerMerchantMenu(session, e);
                    break;

                //Bank popup window
                case GameOpcode.BankUI:
                    if ((e.EntityType & EntityType.Banker) == EntityType.Banker)
                        ServerBankInteract.OpenBankMenu(session);
                    break;

                case GameOpcode.BlackSmithMenu:
                    if ((e.EntityType & EntityType.Blacksmith) == EntityType.Blacksmith)
                        ServerBlackSmith.ActivateBlackSmithMenu(session);
                    break;

                //Dialogue and Quest Interaction
                case GameOpcode.Interact:
                case GameOpcode.DialogueBoxOption:
                    session.MyCharacter.ProcessDialogue(session, reader, clientPacket, targetNPC);
                    break;
            }
        }
    }
}
