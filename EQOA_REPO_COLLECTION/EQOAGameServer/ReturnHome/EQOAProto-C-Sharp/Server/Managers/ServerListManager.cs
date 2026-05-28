using System;
using System.Collections.Concurrent;
using System.Configuration;
using System.Net;
using System.Text;
using ReturnHome.Server.Network;
using ReturnHome.Server.Opcodes;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Managers
{
    public static class ServerListManager
    {

        private static Encoding unicode = Encoding.Unicode;
        private static ConcurrentDictionary<IPEndPoint, Session> sessionDict = new();
        private static Message _message;
        public static void AddSession(Session session)
        {
            if (sessionDict.TryAdd(session.MyIPEndPoint, session))
                return;

            Console.WriteLine("Error occured and session was not added to ServerList Queue");
        }

        public static void DistributeServerList()
        {
            //Check to see if there is any point to even send a server list

            if (sessionDict.IsEmpty)
                return;

            //Means there is client's on the server list menu, let's send the list
            foreach (var result in sessionDict)
            {
                result.Value.sessionQueue.Add(_message);
            }
        }

        public static void RemoveSession(Session session)
        {
            if (session != null)
            {
                if (sessionDict.TryRemove(session.MyIPEndPoint, out _))
                {
                    Console.WriteLine("Session removed from ServerList");
                    return;
                }
            }
        }

        public static void ReadConfig()
        {
            try
            {
                Logger.Info("Connecting to config file");
                ///Connects to our config file
                var appSettings = ConfigurationManager.AppSettings;
                ///Checks for our "ServerCount" config option
                string result = appSettings["Servercount"] ?? "Not Found";

                ///Check if results were found
                if (result == "Not Found")
                {


                    Logger.Err("Config file not found, is it present?");
                }

                else
                {

                    try
                    {
                        Logger.Info($"Acquiring Server Config, total of {result} config's");

                        ///Convert our result to an int
                        int ServerCount = int.Parse(result);

                        //Standard server listing has 14 bytes to represent data not including length of servername and 3 bytes for server count and opcode
                        int size = 14 * ServerCount + 3;

                        //Get length of server names
                        for (int i = 0; i < ServerCount; i++)
                        {
                            //Length * 2 because unicode
                            size += appSettings[$"Server{i}"].Length * 2;
                        }

                        _message = new Message(MessageType.UnreliableMessage, GameOpcode.GameServers);
                        BufferWriter writer = new BufferWriter(_message.Span);

                        writer.Write(_message.Opcode);

                        ///Add Server count to our preformed packet
                        ///KEep this simple since value can't get very high
                        writer.Write7BitEncodedInt64(ServerCount);

                        ///Cycle through servers in Config List
                        for (int i = 0; i < ServerCount; i++)
                        {

                            ///Gets Server Name and Name Length
                            string ServerName = appSettings[$"Server{i}"];
                            writer.WriteString(Encoding.Unicode, ServerName);

                            ///Add Server Recommendation
                            writer.Write(Convert.ToByte(appSettings[$"ServerRecommended{i}"]));

                            ///Add Server End Point
                            writer.Write(Convert.ToUInt16(appSettings[$"ServerEndPointID{i}"]));

                            ///Add Server Port
                            writer.Write(Convert.ToUInt16(appSettings[$"ServerPort{i}"]));

                            IPAddress test2 = IPAddress.Parse(appSettings[$"ServerIP{i}"]);
                            byte[] test = test2.GetAddressBytes();
                            Array.Reverse(test);
                            writer.Write(test);

                            writer.Write(Convert.ToByte(appSettings[$"ServerLanguage{i}"]));

                            Logger.Info($"Acquired Server #{i + 1}");
                        }

                        Memory<byte> temp = _message.message;
                        _message.message = temp.Slice(0, writer.Position);
                        _message.Size = writer.Position;

                        Logger.Info("Done...");
                    }

                    catch (FormatException)
                    {
                        Logger.Err($"Unable to parse '{result}'");
                    }
                }

            }
            catch (ConfigurationErrorsException)
            {
                Logger.Err("Error reading app settings");
            }
        }
    }
}
