using System;
using System.Collections.Generic;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Network
{
    public class ClientObjectUpdate
    {
        //Save base messages coming?
        private List<StateBaseMessage> _bases = new ();
        public ushort SeqNum;
        public void AddBaseClientArray(Memory<byte> update, ushort seq) => _bases.Add(new StateBaseMessage(seq, update));

        public bool GetBaseClientArray(ushort BaseSeqnum, out Memory<byte> temp2)
        {
            temp2 = default;
            foreach(StateBaseMessage s in _bases)
            {
                if(s.SeqNum == BaseSeqnum)
                {
                    temp2 = s.BaseMessage;
                    return true;
                }
            }
            return false;
        }

        public void seqnum_remove_thru(ushort sn)
        {
            while (_bases.Count > 0)
            {
                StateBaseMessage msg = _bases[0];
                if ((msg.SeqNum - (sn & 0xffff)) * 0x10000 < 0)
                    _bases.RemoveAt(0);

                else
                    return;
            }
        }
    }
}
