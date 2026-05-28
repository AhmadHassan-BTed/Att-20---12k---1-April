using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using ReturnHome.Utilities;

namespace ReturnHome.Server.EntityObject.Player
{
    //Memory dump always expects 4 weapon hotbars, even if it's just "null"
    public class WeaponHotbar
    {
        public string HotbarName { get; private set; }
        public int PrimaryHandID { get; private set; }
        public int SecondaryHandID { get; private set; }

        //Default constructor
        //Even if not hotbar data, these must be -1 (Techniqued is 1)
        public WeaponHotbar()
        {
            HotbarName = "";
            PrimaryHandID = -1;
            SecondaryHandID = -1;
        }

        public WeaponHotbar(string thisHotBarName, int thisPrimaryHandID, int thisSecondaryHandID)
        {
            HotbarName = thisHotBarName;
            PrimaryHandID = thisPrimaryHandID;
            SecondaryHandID = thisSecondaryHandID;
        }
        
        public void DumpWeaponHotbar(ref BufferWriter writer)
        {
            writer.Write7BitEncodedInt64(PrimaryHandID);
            writer.Write7BitEncodedInt64(SecondaryHandID);
            writer.WriteString(Encoding.Unicode, HotbarName);
        }
    }
}
