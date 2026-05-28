 using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.EntityObject.Stats;
using ReturnHome.Server.Managers;
using System;
using System.Collections.Generic;
using System.Numerics;

namespace ReturnHome.Server.EntityObject.Actors
{
    public class Actor : Entity
    {
        public long killtime = 0;
        public int Tunar = 0;
        public Corpse corpse;
        public Dictionary<uint, int> aggroTable = new Dictionary<uint, int>();

        public Actor() : base(false, 0)
        {

        }

        public Actor(string charName, float xCoord, float yCoord, float zCoord, int facing, int world, int modelid, float size,
            int primary, int secondary, int shield, int hair_color, int hair_length, int hair_style, int level, int torso, int forearms,
            int gloves, int legs, int feet, int head, EntityType npcType, int serverID) : base(false, level)
        {

            CharName = charName;
            x = xCoord;
            y = yCoord;
            z = zCoord;
            Position = new Vector3(x, y, z);
            Facing = (byte)facing;
            World = (World)world;
            ModelID = modelid;
            ModelSize = size;
            Primary = primary;
            Secondary = secondary;
            Shield = shield;
            HairColor = hair_color;
            HairLength = hair_length;
            HairStyle = hair_style;
            Level = level;
            Chest = (byte)torso;
            Bracer = (byte)forearms;
            Gloves = (byte)gloves;
            Legs = (byte)legs;
            Boots = (byte)feet;
            Helm = (byte)head;
            CurrentStats.Add(StatModifiers.STA, 300);
            HPFlag = true;
            Target = 0xFFFFFFFF;
            EntityType = npcType;
            ServerID = serverID;

            corpse = new(this);

            //staticly assign tunar onhand to a npc for now, only really relevant for when mobs die and money goes around
            //Inventory = new(3000);
        }

        public async void AttackPlayer(int damage, uint targetID)
        {

            if(PlayerManager.QueryForPlayer(targetID, out Character player))
            {
                //Console.WriteLine($"Found player to attack: {player.CharName}");
                player.Animation = 0x2b;
                player.CurrentHP -= 5;
            }

            this.Animation = 0x17;
            this.Animation = 0x22;



        }

        public void EvaluateAggro(int damage, uint playerID)
        {

            int mostAggro = damage;
            int aggro = 0;
            uint targetPlayer = 0;
            aggro += damage;

            if (aggroTable.ContainsKey(playerID))
            {
                aggroTable[playerID] += damage;
                //Console.WriteLine($"Aggro table already contains playerID {playerID}");
            }
            else
            {
                aggroTable.Add(playerID, damage);
                //Console.WriteLine($"Adding player with playerID {playerID} to aggro Table.");
            }


            foreach (KeyValuePair<uint, int> player in aggroTable)
            {
                if (player.Value > mostAggro)
                {
                    mostAggro = player.Value;
                    targetPlayer = player.Key;
                }
            }

            //AttackPlayer(damage, playerID);

        }
    }
}
