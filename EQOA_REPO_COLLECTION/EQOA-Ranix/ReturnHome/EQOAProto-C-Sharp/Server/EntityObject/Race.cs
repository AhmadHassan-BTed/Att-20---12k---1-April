namespace ReturnHome.Server.EntityObject
{
    public enum Race : byte
    {
        Human     = 0,
        Elf       = 1,
        Dark_Elf  = 2,
        Gnome     = 3,
        Dwarf     = 4,
        Troll     = 5,
        Barbarian = 6,
        Halfing   = 7,
        Erudite   = 8,
        Ogre      = 9
    }
    
    //Including this here due to being so small and every race has a sex male/female
    public enum Sex : byte
    {
        Male   = 0,
        Female = 1
    }

    //another small enum related to Race
    public enum HumanType : byte
    {
        Other    = 0,
        Freeport = 1,
        Qeynos   = 2
    }
}
