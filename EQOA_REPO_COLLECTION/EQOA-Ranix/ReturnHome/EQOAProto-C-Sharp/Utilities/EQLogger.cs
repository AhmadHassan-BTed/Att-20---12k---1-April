using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace ReturnHome.Utilities
{
    public static class Logger
    {
        static StreamWriter writer = File.AppendText("info.log");
        static StreamWriter errwriter = File.AppendText("err.log");


        public static void Info(string logMessage)
        {
            if (writer.AutoFlush == false)
            {
                writer.AutoFlush = true;
            }

            writer.WriteLine($"{DateTime.Now.ToString("MM/dd/yyyy hh:mm:ss.fff tt")} : {logMessage}");
        }

        public static void Info(byte[] logMessage)
        {
            if (writer.AutoFlush == false)
            {
                writer.AutoFlush = true;
            }

            ///Writes lists into logs for us
            writer.Write($"{DateTime.Now.ToString("MM/dd/yyyy hh:mm:ss.fff tt")} :");
            for(int i = 0; i < logMessage.Length; i++)
            {
                writer.Write($" {logMessage[i].ToString("X")}");
            }

            ///Ends our current list/message line
            writer.WriteLine("");
        }

        public static void Info(List<byte> logMessage)
        {
            ///Writes lists into logs for us
            writer.Write($"{DateTime.Now.ToString("MM/dd/yyyy hh:mm:ss.fff tt")} :");
            for( int i = 0; i < logMessage.Count(); i++)
            { 
                writer.Write($" {logMessage[i].ToString("X")}");
            }

            ///Ends our current list/message line
            writer.WriteLine("");
        }

        public static void Info(ReadOnlyMemory<byte> LogMessage)
        {
            ReadOnlySpan<byte> logMessage = LogMessage.Span;
            if (writer.AutoFlush == false)
            {
                writer.AutoFlush = true;
            }

            ///Writes lists into logs for us
            writer.Write($"{DateTime.Now.ToString("MM/dd/yyyy hh:mm:ss.fff tt")} :");
            for (int i = 0; i < logMessage.Length; i++)
            {
                writer.Write($" {logMessage[i].ToString("X")}");
            }

            ///Ends our current list/message line
            writer.WriteLine("");
        }

        public static void Err(string errMessage)
        {
            if (errwriter.AutoFlush == false)
            {
                errwriter.AutoFlush = true;
            }

            errwriter.WriteLine($"{DateTime.Now.ToString("MM/dd/yyyy hh:mm:ss.fff tt")} : {errMessage}");
        }
    }
}
