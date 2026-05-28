using ReturnHome.Server.EntityObject.Stats;

using System;
using System.Numerics;

namespace ReturnHome.Server.EntityObject.Actors
{
  public class Actor : Entity
  {
        public long killtime = 0;
        public int Tunar = 0;
        public Actor() : base(false, 0)
        {

        }

        public Actor(string charName, float xCoord, float yCoord, float zCoord, int facing, int world, int modelid, float size,
            int primary, int secondary, int shield, int hair_color, int hair_length, int hair_style, int level, int torso, int forearms,
            int gloves, int legs, int feet, int head, uint npcType) : base(false, level)
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
            HPFlag = true;
            CurrentStats.Add(StatModifiers.STA, 300);
            Target = 0xFFFFFFFF;
            NPCType = (ushort)npcType;

            //staticly assign tunar onhand to a npc for now, only really relevant for when mobs die and money goes around
            //Inventory = new(3000);
        }
  }
}
