namespace ReturnHome.Server.EntityObject.Stats
{
    public enum StatModifiers : byte
    {
        STR = 0,
        STA = 1,
        AGI = 2,
        DEX = 3,
        WIS = 4,
        INT = 5,
        CHA = 6,
        HPMAX = 8,
        POWMAX = 10,
        PoT = 12,
        HoT = 13,
        AC = 14,
        PoisonResistance = 22,
        DiseaseResistance = 23,
        FireResistance    = 24,
        ColdResistance    = 25,
        LightningResistance = 26,
        ArcaneResistance = 27,
        FISH  = 28,

        //Base stats derived from a default character... May remove eventually
        BaseSTR = 30,
        BaseSTA = 31,
        BaseAGI = 32,
        BaseDEX = 33,
        BaseWIS = 34,
        BaseINT = 35,
        BaseCHA = 36,

        //Training Point stat section
        TPSTR = 40,
        TPSTA = 41,
        TPAGI = 42,
        TPDEX = 43,
        TPWIS = 44,
        TPINT = 45,
        TPCHA = 46,

        //Eventual CM Section
        //50+
    }
}
