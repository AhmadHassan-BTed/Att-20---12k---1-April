using System;
using System.IO;
using System.Text;

using ReturnHome.Utilities;

namespace ReturnHome.Server.EntityObject.Player
{
    public class Hotkey
    {
        public string Direction { get; private set; }
        public string NLabel { get; private set; }
        public string NMessage { get; private set; }
        public string WLabel { get; private set; }
        public string WMessage { get; private set; }
        public string ELabel { get; private set; }
        public string EMessage { get; private set; }
        public string SLabel { get; private set; }
        public string SMessage { get; private set; }

        public Hotkey()
        { }

        //Will instantiate a Hotkey object
        public Hotkey(string thisDirection, string thisNMessage, string thisNLabel, string thisWMessage, string thisWLabel, string thisEMessage, string thisELabel, string thisSMessage, string thisSLabel)
        {
            Direction = thisDirection;
            NLabel = thisNLabel;
            NMessage = thisNMessage;
            WLabel = thisWLabel;
            WMessage = thisWMessage;
            ELabel = thisELabel;
            EMessage = thisEMessage;
            SLabel = thisSLabel;
            SMessage = thisSMessage;
        }

        public void PullHotkey(ref BufferWriter writer)
        {
            //Packdata in list here and return it to calling method
            //Get directions integer value and perform technique
            writer.Write7BitEncodedInt64(HotKeyFuncs.OutHoingHotkeyDict[Direction]); 

            //North HK
            writer.Write((byte)0);
            ConvertHotKey(ref writer, NLabel, NMessage);

            //West HK
            writer.Write((byte)2);
            ConvertHotKey(ref writer, WLabel, WMessage);

            //East HK
            writer.Write((byte)4);
            ConvertHotKey(ref writer, ELabel, EMessage);

            //South HK
            writer.Write((byte)6);
            ConvertHotKey(ref writer, SLabel, SMessage);
        }

        private void ConvertHotKey(ref BufferWriter writer, string label, string message)
        {
            //If message
            if(label != null)
            {
                //Add string length, 4 bytes then string as utf-16-le
                writer.WriteString(Encoding.Unicode, label);

                //If label
                if (message != null)
                    //Add string length, 4 bytes then string as utf-16-le
                    writer.WriteString(Encoding.Unicode, message);

                //no label
                else
                    //Length of 0, 4 bytes
                    writer.Write((byte)0);
            }

            //No message
            else
            {
                //Length of 0, 4 bytes
                writer.Write((byte)0);

                //If label
                if (message != null)
                    //Add string length, 4 bytes then string as utf-16-le
                    writer.WriteString(Encoding.Unicode, message);

                //no label
                else
                    //Length of 0, 4 bytes
                    writer.Write((byte)0);
            }
        }
    }
}
