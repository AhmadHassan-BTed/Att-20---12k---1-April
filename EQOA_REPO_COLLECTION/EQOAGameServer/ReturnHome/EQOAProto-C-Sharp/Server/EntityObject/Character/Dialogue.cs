// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ReturnHome.Server.Network;
using ReturnHome.Server.Managers;
using ReturnHome.Utilities;
using ReturnHome.Server.Opcodes;

namespace ReturnHome.Server.EntityObject.Player
{
    public class Dialogue
    {
        public string npcName;
        public uint counter = 0;
        public uint choice = 0;
        public string dialogue;
        public List<string> diagOptions = new List<string>();
        public List<string> multiDialogue = new List<string>();

        public Dialogue()
        {

        }

        public Dialogue(string npcName, uint counter, uint choice)
        {
            this.npcName = npcName;
            this.counter = counter;
            this.choice = choice;
        }

       

    }
}


