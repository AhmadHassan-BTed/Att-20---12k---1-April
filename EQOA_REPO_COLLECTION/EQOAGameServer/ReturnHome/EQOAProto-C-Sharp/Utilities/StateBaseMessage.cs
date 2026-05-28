using System;

namespace ReturnHome.Utilities
{
    public struct StateBaseMessage
    {
        public ushort SeqNum;
        public Memory<byte> BaseMessage;

        public StateBaseMessage(ushort seqnum, Memory<byte> baseMessage)
        {
            SeqNum = seqnum;
            BaseMessage = baseMessage;
        }

    }
}
