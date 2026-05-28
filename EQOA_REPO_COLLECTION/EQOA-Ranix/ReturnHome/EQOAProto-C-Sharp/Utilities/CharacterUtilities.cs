using System.Collections.Generic;

namespace ReturnHome.Utilities
{
    public static class CharacterUtilities
    {
        //Dictionary mapping the client value to the string value expected in the DB
        //Eastern Human = 1, Western Human = 2, All non humans = 0
        public static readonly Dictionary<int, string> HumTypeDict = new Dictionary<int, string>
        {
            {0, "Other" },
            {1, "Freeport" },
            {2, "Qeynos" }
        };

        //Dictionary mapping the client value to the string value expected in the DB for Player Class
        public static readonly Dictionary<int, string> CharClassDict = new Dictionary<int, string>
        {
            {0, "WAR" },  {1, "RAN" }, {2, "PAL" }, {3, "SK" }, {4, "MNK" },{5, "BRD" }, {6, "RGE" },
            {7, "DRD" }, {8, "SHA" }, {9, "CL" }, {10, "MAG" }, {11, "NEC" },{12, "ENC" }, {13, "WIZ" }, {14, "ALC" }
        };

        //Dictionary mapping the client value to the string value expected in the DB for Player Race
        public static readonly Dictionary<int, string> CharRaceDict = new Dictionary<int, string>
        {
            {0, "HUM" }, {1, "ELF" }, {2, "DELF" }, {3, "GNO" }, {4, "DWF" }, {5, "TRL" }, {6, "BAR" }, {7, "HLF" },
            {8, "ERU" }, {9, "OGR" }
        };

        //Dictionary mapping the client value to the string value expected in the DB for Player Gender
        public static readonly Dictionary<int, string> CharSexDict = new Dictionary<int, string>
        {
            {0, "Male" }, {1, "Female"}
        };

        public static readonly Dictionary<int, int> CharXPDict = new Dictionary<int, int>
        {
            {0, 0 }, {1, 424}, {2, 2187}, {3, 6976}, {4, 17125}, {5, 35640}, {6, 66199}, {7, 113152}, {8, 181521},
            {9, 277000}, {10, 405955}, {11, 575424}, {12, 793117}, {13, 1067416}, {14, 1407375}, {15, 1822720},
            {16, 2323849}, {17, 2921832}, {18, 3628411}, {19, 4456000}, {20, 5417685}, {21, 6527224}, {22, 7799047},
            {23, 9248256}, {24, 10890625}, {25, 12742600}, {26, 14821299}, {27, 17144512}, {28, 19730701}, {29, 22599000},
            {30, 25769215}, {31, 29261824}, {32, 33097977}, {33, 37299496}, {34, 41888875}, {35, 46889280}, {36, 52324549},
            {37, 58219192}, {38, 64598391}, {39, 71488000}, {40, 78914545}, {41, 86905224}, {42, 95487907}, {43, 104691136},
            {44, 114544125}, {45, 125076760}, {46, 136319599}, {47, 148303872}, {48, 161061481}, {49, 174625000}, {50, 189027675},
            {51, 204303424}, {52, 220486837}, {53, 237613176}, {54, 255718375}, {55, 274839040}, {56, 295012449}, {57, 316276552},
            {58, 338669971}, {59, 362232000}
        };
    }
}
