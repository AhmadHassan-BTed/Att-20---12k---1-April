
namespace ReturnHome.Server.Network
{
    public class SegmentBodyFlags
    {
        private bool _rdpReport = false;
        private bool _rdpMessage = false;
        private bool _sessionAck = false;
        private bool _clientUpdateAck = false;

        public bool RdpReport
        {
            get { return _rdpReport; }
            set
            {
                _rdpReport = value;
            }
        }

        public bool RdpMessage
        {
            get { return _rdpMessage; }
            set
            {
                _rdpMessage = value;
            }
        }

        public bool SessionAck
        {
            get { return _sessionAck; }
            set
            {
                _sessionAck = value;
            }
        }

        public bool clientUpdateAck
        {
            get { return _clientUpdateAck; }
            set
            {
                _clientUpdateAck = value;
            }
        }
    }
}
