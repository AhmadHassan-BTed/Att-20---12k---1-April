using System;
using System.Net;
using System.Net.Sockets;
using System.Threading.Tasks;

namespace AuthServer.Server
{
    //Based on
    //https://docs.microsoft.com/en-us/dotnet/framework/network-programming/asynchronous-server-socket-example
    // State object for reading client data asynchronously  

    public class AsynchronousSocketListener
    {
        private readonly int _buffSize = 500;
        private byte[] _buffer = new byte[500];
        private readonly EQOAClientTracker _eqoaClientTracker;
        private readonly Socket _listener = new(SocketType.Stream, ProtocolType.Tcp);

        public AsynchronousSocketListener(EQOAClientTracker eqoaclientTracker)
        {
            _eqoaClientTracker = eqoaclientTracker;
        }

        public async Task StartListener()
        {
            _listener.Bind(new IPEndPoint(IPAddress.Any, 9735));

            _listener.Listen(100);

            while (true)
            {
                Socket socket = await _listener.AcceptAsync();
                Task.Run(() => ProcessAsync(socket));
            }
        }

        private async Task ProcessAsync(Socket socket)
        {

            //Create Client session
            EQOAClient theClient = new((IPEndPoint)socket.RemoteEndPoint, socket);
            //Add Session to Tracking
            _eqoaClientTracker.AddEQOAClient(theClient);

            while (true)
            {
                // note we'll usually get *much* more than we ask for
                SocketReceiveFromResult result = await socket.ReceiveFromAsync(
                        new ArraySegment<byte>(_buffer, 0, _buffSize), SocketFlags.None, new IPEndPoint(IPAddress.Any, 0));
                byte[] buffer = result.ReceivedBytes < _buffSize ? _buffer.AsSpan(0, result.ReceivedBytes).ToArray() : _buffer;
                theClient.CheckClientRequest(buffer);
                Console.WriteLine($"Processing data for {theClient.ClientIPEndPoint}");

                if (result.ReceivedBytes == 0)
                    break;
            }
            _eqoaClientTracker.RemoveEQOAClient(theClient);

            Console.WriteLine($"[{socket.RemoteEndPoint}]: disconnected");
        }

        public static void ProcessSend(EQOAClient client)
        {
            client.Handler.Send(client.ResponsePacket.Span);
        }
    }
}
