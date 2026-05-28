using System;
using System.Threading.Tasks;
using AuthServer.Server;

namespace AuthServer
{
    class AuthServer
    {
        static async Task Main(string[] args)
        {
            EQOAClientTracker eqoaClientTracker = new();
            //Start TCP Server

            AsynchronousSocketListener TcpServer = new(eqoaClientTracker);
            await TcpServer.StartListener();

            string stuff = Console.ReadLine();
        }
    }
}
