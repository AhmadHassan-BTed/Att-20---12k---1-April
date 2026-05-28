using System;
using System.Net;
using System.Net.Sockets;
using System.Threading.Channels;
using System.Threading.Tasks;
using ReturnHome.Server.Network.Managers;
using ReturnHome.Utilities;

///Create a file revolved around UDP to place class into
///
namespace ReturnHome.Server.Network
{
    public class ServerListener
    {
        ///For now hard code port, config file eventually
        public ushort serverEndPoint { get; private set; }
		public string serverName { get; private set; }
        private readonly int _buffSize = 300;
        private byte[] _buffer = new byte[300];
        public Socket socket;
        
        public ServerListener(IPAddress IP, ushort Port, ushort EndPoint)
        {
            serverEndPoint = EndPoint;
            serverName = "Default";
        }

		public ServerListener(IPAddress IP, ushort Port, ushort EndPoint, string ServerName)
		{
			serverEndPoint = EndPoint;
			serverName = ServerName;
			socket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);

        //Specifically helps to prevent ICMP's causing a socket exception special to windows..
#if Windows
            socket.IOControl((IOControlCode)(-1744830452), new byte[] { 0, 0, 0, 0 }, null);
#endif
            socket.SetSocketOption(SocketOptionLevel.IP, SocketOptionName.PacketInformation, true);
            socket.Bind(new IPEndPoint(IP, Port));
		}
		
        public async Task StartServer()
        {
            SocketReceiveFromResult result = new();
            while (true)
            {
                byte[] buffer= null;
                try
                {
                    result = await socket.ReceiveFromAsync(
                        new ArraySegment<byte>(_buffer, 0, _buffSize), SocketFlags.None, new IPEndPoint(IPAddress.Any, 0));
                    buffer = result.ReceivedBytes < _buffSize ? _buffer.AsSpan(0, result.ReceivedBytes).ToArray() : _buffer;
                }
                catch (SocketException e)
                {
                    Logger.Err("Wat??");
                }
                catch (ObjectDisposedException e)
                {
                    Logger.Err("This socket is closed!!");
                }
                catch (ArgumentOutOfRangeException e)
                {
                    Logger.Err("This is < -1");
                }

                if (buffer != null)
                {
                    ClientPacket clientPacket = new();
                    if (clientPacket.ProcessPacket(new Memory<byte>(buffer)))
                        SessionManager.ProcessPacket(this, clientPacket, (IPEndPoint)result.RemoteEndPoint);
                }
            }
        }
    }
}
