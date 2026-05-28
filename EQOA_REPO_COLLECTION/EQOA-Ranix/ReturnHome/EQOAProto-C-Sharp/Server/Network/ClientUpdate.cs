// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.


using System;

namespace ReturnHome.Server.Network
{
    public class ClientObjectUpdate
    {
        public ushort BaseXorMessage { get; set; } = 0;
        private Memory<byte> BaseClientUpdate { get; set; }

        public void UpdateBaseClientArray(Memory<byte> update)
        {
            BaseClientUpdate = update;
        }

        public Memory<byte> GetBaseClientArray()
        {
            return BaseClientUpdate;
        }
    }
}
