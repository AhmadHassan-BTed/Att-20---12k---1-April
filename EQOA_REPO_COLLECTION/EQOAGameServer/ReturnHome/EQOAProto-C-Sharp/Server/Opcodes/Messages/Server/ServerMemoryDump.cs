using ReturnHome.Database.SQL;
using ReturnHome.Server.EntityObject.Items;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    class ServerMemoryDump
    {
        public static void MemoryDump(Session session)
        {
            Message message = new Message(MessageType.SegmentReliableMessage, GameOpcode.MemoryDump);
            BufferWriter writer = new BufferWriter(message.Span);

            //Perform SQl stuff
            CharacterSQL charDump = new CharacterSQL();

            //Probably change this to only pass in character ServerID
            charDump.GetPlayerHotkeys(session);
            charDump.GetPlayerWeaponHotbar(session);
            charDump.GetPlayerSpells(session);

            //Toss opcode in
            writer.Write(message.Opcode);

            session.MyCharacter.DumpCharacter(ref writer);
            writer.Write7BitEncodedInt64(session.MyCharacter.MyHotkeys.Count);

            //cycle over all our hotkeys and append them
            foreach (Hotkey h in session.MyCharacter.MyHotkeys)
                h.PullHotkey(ref writer);

            //Unknown at this time 4 byte null
            writer.Write(0);

            //Unknown at this time 4 byte null
            writer.Write(0);

            //Quest Count
            writer.Write(session.MyCharacter.activeQuests.Count);

            //Iterate over quest data and append (Should be 0 for now...)
            foreach (Quest q in session.MyCharacter.activeQuests)
                q.DumpQuest(ref writer);

            //Get Inventory Item count
            writer.Write7BitEncodedInt64(session.MyCharacter.Inventory.Count);
            writer.Write(session.MyCharacter.Inventory.Count);

            for(int i = 0; i < session.MyCharacter.Inventory.Count; i++)
                session.MyCharacter.Inventory.itemContainer[i].item.DumpItem(ref writer, session.MyCharacter.Inventory.itemContainer[i].key);

            foreach (WeaponHotbar wb in session.MyCharacter.WeaponHotbars)
                wb.DumpWeaponHotbar(ref writer);

            //Get Bank Item count
            writer.Write7BitEncodedInt64(session.MyCharacter.Bank.Count);
            writer.Write(session.MyCharacter.Bank.Count);
            for (int i = 0; i < session.MyCharacter.Bank.Count; i++)
                session.MyCharacter.Bank.itemContainer[i].item.DumpItem(ref writer, session.MyCharacter.Bank.itemContainer[i].key);

            // end of bank? or could be something else for memory dump
            writer.Write((byte)0);

            //Buying auctions
            writer.Write((byte)session.MyCharacter.MyBuyingAuctions.Count);
            foreach (Auction ba in session.MyCharacter.MyBuyingAuctions)
                ba.DumpAuction(ref writer);

            //Selling auctions
            writer.Write((byte)session.MyCharacter.MySellingAuctions.Count);
            foreach (Auction sa in session.MyCharacter.MySellingAuctions)
                sa.DumpAuction(ref writer);

            //Spell count and Spells
            writer.Write7BitEncodedInt64(session.MyCharacter.MySpells.Count);
            foreach (Spell s in session.MyCharacter.MySpells)
                s.DumpSpell(ref writer);

            //Collect Character Stats
            //DefaultCharacter.DefaultCharacterDict.TryGetValue((session.MyCharacter.EntityRace, session.MyCharacter.EntityClass, session.MyCharacter.EntityHumanType, session.MyCharacter.EntitySex), out Character defaultCharacter);
            for (int i = 0; i < session.MyCharacter.Inventory.Count; i++)
            {
                if ((sbyte)session.MyCharacter.Inventory.itemContainer[i].item.EquipLocation != -1)
                    ItemInteraction.EquipItem(session.MyCharacter, session.MyCharacter.Inventory.itemContainer[i].item, (byte)i);
            }
            //Not entirely known what this is at this time
            //Related to stats and CM's possibly. Needs testing, just using data from a pcap of live.
            writer.Write(session.MyCharacter.Speed);
            writer.Write(new byte[] { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                      0x09, 0x0a, 0x0b, 0x0c, //Weapon Level
                                      0x0d, 0x0e, 0x0f, 0x10, //Weapon Rank
                                      0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                                      0x19, 0x1a, 0x1b, 0x1c, //Armor Level
                                      0x1d, 0x1e, 0x1f, 0x20, //Armor Rank
                                      0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
                                      0x29, 0x00, 0x00, 0x00, //Tailor Level
                                      0x30, 0x00, 0x00, 0x00, //Tailor Rank
                                      0x00, 0x80, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00,
                                      0x50, 0x00, 0x00, 0x00, //Jewler Level
                                      0x51, 0x00, 0x00, 0x00, //Jewlet Rank
                                      0x25, 0x26, 0x27, 0x28,
                                      0x11, 0x12, 0x13, 0x14,
                                      0x15, 0x16, 0x17, 0x18,
                                      0x0d, 0x0e, 0x0f, 0x10,
                                      0x21, 0x22, 0x23, 0x24,
                                      0x05, 0x06, 0x07, 0x08,
                                      0x01, 0x02, 0x03, 0x04,
                                      0x19, 0x1a, 0x1b, 0x1c,
                                      0x1d, 0x1e, 0x1f, 0x20,
                                      0x25, 0x26, 0x27, 0x28 });

            writer.Write7BitEncodedInt64(session.MyCharacter.UnspentCMs);
            writer.Write7BitEncodedInt64(session.MyCharacter.SpentCMs);

            writer.Write(new byte[] { 0xae, 0x98, 0x4c });
            writer.Write7BitEncodedInt64(session.MyCharacter.CMPercentage);
            writer.Write(session.MyCharacter.Speed);

            /*Addition to Strength Base*/
            writer.Write7BitEncodedInt64(0);
            /*Addition to Stamina  Base*/
            writer.Write7BitEncodedInt64(0);
            /*Addition to Agility Base*/
            writer.Write7BitEncodedInt64(0);
            /*Addition to Dexterity Base*/
            writer.Write7BitEncodedInt64(0);
            /*Addition to Wisdom Base*/
            writer.Write7BitEncodedInt64(0);
            /*Addition to Intelligence Base*/
            writer.Write7BitEncodedInt64(0);
            /*Addition to Charisma Base*/
            writer.Write7BitEncodedInt64(0);

            writer.Write(new byte[] { 0xde, 0x02, //CM Strength Max
                                      0xde, 0x02, //CM Stamina Max
                                      0x00, //CM Agility Max
                                      0xfa, 0x01,  // CM Dexterity Max
                                      0x00, // CM Wisdom Max
                                      0x00, // CM Intelligence Max
                                      0x00 }); // CM Charisma Max


            /*Addition to HP Base*/
            writer.Write7BitEncodedInt64(0);
            /*Addition to Power Base*/
            writer.Write7BitEncodedInt64(0);

            writer.Write(new byte[] { 0x5a, //CM HoT
                                      0x00 }); // CM PoT?

            /*Addition to AC Base*/
            writer.Write7BitEncodedInt64(0);

            writer.Write(new byte[] { 0x04, //CM OFF Mod's?
                                      0x00, // CM DEF Mod's?
                                      0x0c });//CM HP Factor's?

            /*Addition to FR Base*/
            writer.Write7BitEncodedInt64(0);
            /*Addition to LR Base*/
            writer.Write7BitEncodedInt64(0);
            /*Addition to CR Base*/
            writer.Write7BitEncodedInt64(0);
            /*Addition to AR Base*/
            writer.Write7BitEncodedInt64(0);
            /*Addition to PR Base*/
            writer.Write7BitEncodedInt64(0);
            /*Addition to DR Base*/
            writer.Write7BitEncodedInt64(0);

            writer.Write(new byte[] { 0x00,
                                      0xde, 0x02, //CM Strength Max
                                      0xde, 0x02, //CM Stamina Max
                                      0x00, //CM Agility Max
                                      0xfa, 0x01,  // CM Dexterity Max
                                      0x00, // CM Wisdom Max
                                      0x00, // CM Intelligence Max
                                      0x00 }); // CM Charisma Max

            ServerTime.Time(session);

            message.Size = writer.Position;
            session.sessionQueue.Add(message);

            //Put player into channel 0?
            session.rdpCommIn.connectionData.serverObjects.Span[0].AddObject(session.MyCharacter);

            ServerPlayerIgnoreList.PlayerIgnoreList(session);
            //ServerPlayerSpeed.PlayerSpeed(session);
        }
    }
}
