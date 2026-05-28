using System;
using System.Text;
using ReturnHome.Database.SQL;
using ReturnHome.Server.EntityObject;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.Network;
using ReturnHome.Server.Opcodes.Messages.Server;
using ReturnHome.Server.EntityObject.Stats;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Client
{
    class ClientCreateCharacter
    {
        //Method to create new character when new character opcode is received
        public static void CreateCharacter(Session session, Message ClientPacket)
        {
            CharacterSQL createCharacter = new CharacterSQL();
            BufferReader reader = new(ClientPacket.message.Span);

            //Get length of characters name expected in packet
            int nameLength = reader.Read<int>();

            //Get Character Name
            string CharName = reader.ReadString(Encoding.UTF8, nameLength);

            //Before processing a full character creation check if the characters name already exists in the DB.
            //Later this will need to include a character/world combination if additional servers are spun up.
            if (CharName == createCharacter.CheckName(CharName))
            {
                //Close SQL connection
                createCharacter.CloseConnection();
                ServerCharacterNameTaken.CharacterNameTaken(session);
            }

            //If name not found continue to actually create character
            else
            {
                string charName = CharName;
                //Get starting level
                int Level = (int)reader.Read7BitEncodedInt64();

                //Divide startLevel by 2 because client doubles it
                //Get single byte attributes
                Race Race = (Race)reader.Read7BitEncodedInt64();
                Class Class = (Class)reader.Read7BitEncodedInt64();
                Sex Gender = (Sex)reader.Read7BitEncodedInt64();
                int HairColor = (int)reader.Read7BitEncodedInt64();
                int HairLength = (int)reader.Read7BitEncodedInt64();
                int HairStyle = (int)reader.Read7BitEncodedInt64();
                int FaceOption = (int)reader.Read7BitEncodedInt64();
                HumanType HumType = (HumanType)reader.Read7BitEncodedInt64();
                
                if(DefaultCharacter.DefaultCharacterDict.TryGetValue((Race, Class, HumType, Gender), out Character defaultCharacter))
                {
                    Character newCharacter = defaultCharacter.Copy();
                    
                    int Strength = reader.Read<int>();
                    int Stamina = reader.Read<int>();
                    int Agility = reader.Read<int>();
                    int Dexterity = reader.Read<int>();
                    int Wisdom = reader.Read<int>();
                    int Intelligence = reader.Read<int>();
                    int Charisma = reader.Read<int>();

                    newCharacter.CharName = charName;

                    if (newCharacter.PlayerTrainingPoints.SpendTrainingPoints(Strength + Stamina + Agility + Dexterity + Wisdom + Intelligence + Charisma))
                    {
                        newCharacter.CurrentStats.Add(StatModifiers.TPSTR, Strength);
                        newCharacter.CurrentStats.Add(StatModifiers.TPSTA, Stamina);
                        newCharacter.CurrentStats.Add(StatModifiers.TPAGI, Agility);
                        newCharacter.CurrentStats.Add(StatModifiers.TPDEX, Dexterity);
                        newCharacter.CurrentStats.Add(StatModifiers.TPWIS, Wisdom);
                        newCharacter.CurrentStats.Add(StatModifiers.TPINT, Intelligence);
                        newCharacter.CurrentStats.Add(StatModifiers.TPCHA, Charisma);
                    }

                    newCharacter.HairColor = HairColor;
                    newCharacter.HairLength = HairLength;
                    newCharacter.HairStyle = HairStyle;
                    newCharacter.FaceOption = FaceOption;

                    //Call SQL method for character creation
                    createCharacter.CreateCharacter(session, newCharacter);

                    //CLose SQL connection
                    createCharacter.CloseConnection();

                }
            }
        }
    }
}
