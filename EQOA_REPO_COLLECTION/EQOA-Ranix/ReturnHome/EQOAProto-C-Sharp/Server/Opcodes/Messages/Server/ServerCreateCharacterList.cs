using System.Collections.Generic;
using System.Text;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    class ServerCreateCharacterList
    {
        public static void CreateCharacterList(List<Character> MyCharacterList, Session session)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.CharacterSelect);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);

            ///Gets our character count and uses technique to "double" it
            writer.Write7BitEncodedInt64(MyCharacterList.Count);

            //Iterates through each charcter in the list and converts attribute values to packet values
            foreach (Character character in MyCharacterList)
            {
                ///Add character name
                writer.WriteString(Encoding.UTF8, character.CharName);

                ///Add Server ID
                writer.Write7BitEncodedInt64(character.ServerID);

                ///Add Model
                writer.Write7BitEncodedInt64(character.ModelID);

                ///Add Class
                writer.Write7BitEncodedInt64((int)character.EntityClass);

                ///Add Race
                writer.Write7BitEncodedInt64((int)character.EntityRace);

                ///Add Level
                writer.Write7BitEncodedInt64(character.Level);

                ///Add Hair color
                writer.Write7BitEncodedInt64(character.HairColor);

                ///Add Hair Length
                writer.Write7BitEncodedInt64(character.HairLength);

                ///Add Hair Style
                writer.Write7BitEncodedInt64(character.HairStyle);

                ///Add Face option
                writer.Write7BitEncodedInt64(character.FaceOption);

                //Equip Gear
                character.EquipGear(character);

                ///Add Robe
                writer.Write(character.Robe);

                ///Add Primary
                writer.Write(character.Primary);

                ///Add Secondary
                writer.Write(character.Secondary);

                ///Add Shield
                writer.Write(character.Shield);

                ///Add Character animation here, dumby for now
                writer.Write((ushort)0x0004);

                ///unknown value?
                writer.Write((byte)0);

                ///Chest Model
                writer.Write(character.Chest);

                ///uBracer Model
                writer.Write(character.Bracer);

                ///Glove Model
                writer.Write(character.Gloves);

                ///Leg Model
                writer.Write(character.Legs);

                ///Boot Model
                writer.Write(character.Boots);

                ///Helm Model
                writer.Write(character.Helm);

                ///unknown value?
                writer.Write(0); 

                ///unknown value?
                writer.Write((ushort)0);

                ///unknown value?
                writer.Write(0xFFFFFFFF);

                ///unknown value?
                writer.Write(0xFFFFFFFF);

                ///unknown value?
                writer.Write(0xFFFFFFFF);

                ///Chest color
                writer.Write(ByteSwaps.SwapBytes(character.ChestColor));

                ///Bracer color
                writer.Write(ByteSwaps.SwapBytes(character.BracerColor));

                ///Glove color
                writer.Write(ByteSwaps.SwapBytes(character.GloveColor));

                ///Leg color
                writer.Write(ByteSwaps.SwapBytes(character.LegColor));

                ///Boot color
                writer.Write(ByteSwaps.SwapBytes(character.BootsColor));

                ///Helm color
                writer.Write(ByteSwaps.SwapBytes(character.HelmColor));

                ///Robe color
                writer.Write(ByteSwaps.SwapBytes(character.RobeColor)); //78
            }

            message.Size = writer.Position;
            ///Character list is complete
            ///Handles packing message into outgoing packet
            session.sessionQueue.Add(message);
        }
    }
}
