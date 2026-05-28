using System;
using System.Threading.Tasks;
using AuthServer.Account;
using AuthServer.Server;

namespace AuthServer
{
    class AuthServer
    {
        static async Task Main(string[] args)
        {
            EQOAClientTracker eqoaClientTracker = new();
            //Start TCP Server

            AccountManagement.Initialize();

            AsynchronousSocketListener TcpServer = new(eqoaClientTracker);
            await TcpServer.StartListener();

            string stuff = Console.ReadLine();
        }
    }
}
