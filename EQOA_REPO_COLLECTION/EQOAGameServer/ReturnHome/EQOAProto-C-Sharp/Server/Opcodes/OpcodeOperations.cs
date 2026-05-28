using System;
using System.Collections.Generic;

using ReturnHome.Utilities;
using ReturnHome.Server.Network;
using ReturnHome.Server.Opcodes.Chat;
using ReturnHome.Server.Opcodes.Messages.Client;
using ReturnHome.Server.Managers;

namespace ReturnHome.Server.Opcodes
{
    public static class ProcessOpcode
    {
        public static readonly Dictionary<GameOpcode, Action<Session, Message>> OpcodeDictionary = new()
        {
            { GameOpcode.DiscVersion, ClientDiscVersion.DiscVersion },
            { GameOpcode.Authenticate, ClientAuthenticate.Authenticate },
            { GameOpcode.Authenticate2, ClientAuthenticate.Authenticate },
            { GameOpcode.SELECTED_CHAR, ClientProcessCharacterChanges.ProcessCharacterChanges },
            { GameOpcode.DelCharacter, ClientDeleteCharacter.DeleteCharacter },
            { GameOpcode.CreateCharacter, ClientCreateCharacter.CreateCharacter },
            { GameOpcode.ClientSayChat, ChatMessage.ProcessClientChat },
            { GameOpcode.RandomName, ClientGenerateRandomCharacterName.GenerateRandomCharacterName },
            { GameOpcode.ClientShout, ShoutChat.ProcessShout },
            { GameOpcode.ChangeChatMode, ClientChangeChatMode.ChangeChatMode },
            { GameOpcode.DisconnectClient, ClientDisconnectClient.DisconnectClient },
            { GameOpcode.Target, ClientPlayerTarget.PlayerTarget },
            { GameOpcode.Interact, ClientInteractActor.InteractActor },
            { GameOpcode.DialogueBoxOption, ClientInteractActor.InteractActor },
            { GameOpcode.BankUI, ClientInteractActor.InteractActor },
            { GameOpcode.MerchantDiag, ClientInteractActor.InteractActor },
            { GameOpcode.DepositBankTunar, ClientBank.DepositOrTakeTunar },
            { GameOpcode.PlayerTunar, ClientInteractActor.InteractActor },
            { GameOpcode.ConfirmBankTunar, ClientInteractActor.InteractActor },
            { GameOpcode.BankItem, ClientBank.DepositOrTakeItem },
            { GameOpcode.DeleteQuest, ClientDeleteQuest.DeleteQuest },
            { GameOpcode.MerchantBuy, ClientInteractItem.BuyMerchantItem },
            { GameOpcode.MerchantSell, ClientInteractItem.MerchantSellItem },
            { GameOpcode.ArrangeItem, ClientInteractItem.ArrangeItem },
            { GameOpcode.RemoveInvItem, ClientDeleteItem.DeleteItem },
            { GameOpcode.EnableChannel, EnableChannel },
            { GameOpcode.UpdateTrainingPoints, ClientProcessTrainingPoints.ProcessTrainingPoints },
            { GameOpcode.ClassMastery, ClientClassMastery.ProcessClassMastery },
            { GameOpcode.ClientFaction, ClientFaction.ProcessClientFaction },
            { GameOpcode.Attack, ClientAttack.ClientProcessAttack },
            { GameOpcode.InteractItem, ClientInteractItem.ProcessItemInteraction },
            { GameOpcode.WhoList, ClientWhoListRequest.ProcessWhoList },
            { GameOpcode.GroupInvite, ClientGroup.AddCharacterToGroup },
            { GameOpcode.AcceptGroupInvite, ClientGroup.AcceptGroupInvite },
            { GameOpcode.DisbandGroup, ClientGroup.DisbandGroup },
            { GameOpcode.ClientCloseLoot, ClientLoot.ClientLootClose },
            { GameOpcode.ClientLoot, ClientLoot.ClientLootItem },
            { GameOpcode.DeclineGroupInvite, ClientGroup.DeclineGroupInvite },
            { GameOpcode.LeaveGroup, ClientGroup.LeaveGroup },
            { GameOpcode.BootGroupMember, ClientGroup.BootGroupMember },
            { GameOpcode.LootBoxRequest, ClientLoot.ClientOpenLootMenu },
            { GameOpcode.LootMessages, ClientOptions.ClientMessageOptions },
            { GameOpcode.FactionMessages, ClientOptions.ClientMessageOptions },
            { GameOpcode.BlackSmithMenu, ClientInteractActor.InteractActor },
            { GameOpcode.CloseBlacksmithMenu, ClientBlackSmith.CloseBlackSmithMenu },
            { GameOpcode.RequestRepair, ClientBlackSmith.BlackSmithRepairGear },


        };

        public static void ProcessOpcodes(Session MySession, Message message)
        {

            //Logger.Info($"Message Length: {ClientPacket.Length}; OpcodeType: {MessageTypeOpcode.ToString("X")}; Message Number: {MessageNumber.ToString("X")}; Opcode: {Opcode.ToString("X")}.");
            try
            {
                OpcodeDictionary[message.Opcode].Invoke(MySession, message);
            }

            catch
            {
                ClientOpcodeUnknown(MySession, message.Opcode);
            }
        }

        public static void ClientOpcodeUnknown(Session MySession, GameOpcode opcode)
        {
            if (MySession.unkOpcode)
            {
                string message = $"Unknown Opcode: {((byte)opcode).ToString("X")}";

                ChatMessage.GenerateClientSpecificChat(MySession, message);
            }
        }

        public static void ProcessPingRequest(Session MySession, Message message)
        {
            if (message.message.Span[0] == 0x12)
            {
                Logger.Info("Processed Ping Request");
                //int offset1 = 0;
                //Memory<byte> Message = new byte[1];

                //Message.Write(new byte[] { 0x14 }, ref offset1);
                ///Do stuff here?
                ///Handles packing message into outgoing packet
                //SessionQueueMessages.PackMessage(MySession, Message, MessageOpcodeTypes.ShortReliableMessage);
            }
        }

        public static void EnableChannel(Session MySession, Message message)
        {
            //Activate client channel
            MySession.rdpCommIn.connectionData.serverObjects.Span[0].AddObject(MySession.MyCharacter);

            //If player changed worlds, is currently not existing in a quad tree, let's place into correct quadTree
            if (MySession.MyCharacter.World != MySession.MyCharacter.ExpectedWorld)
            {
                //Update Character world
                MySession.MyCharacter.World = MySession.MyCharacter.ExpectedWorld;
                MapManager.Add(MySession.MyCharacter);
            }
        }
    }
}
