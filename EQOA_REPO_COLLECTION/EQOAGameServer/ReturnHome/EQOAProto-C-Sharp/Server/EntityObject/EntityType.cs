using System;

namespace ReturnHome.Server.EntityObject
{
    //TODO: Need to set all entities(Characters and Actors) to be utilizing this as it will ditate atatckable, or unattackable types + various resources merchant side
    [Flags]
    public enum EntityType : ushort
    {
        Merchant = 0x01,
        Banker = 0x02,
        WontSummon = 0x04,
        Blacksmith = 0x08,
        PetFamiliar = 0x10,
        PetGuardian = 0x20,
        PetFull = 0x40,
        NoTurn = 0x80,
        Coachmen = 0x100
    }
}
