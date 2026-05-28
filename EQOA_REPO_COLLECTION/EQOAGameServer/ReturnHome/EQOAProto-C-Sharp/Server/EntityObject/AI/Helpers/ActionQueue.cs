// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ReturnHome.Server.EntityObject.Player;

namespace ReturnHome.Server.EntityObject.AI.Helpers
{
    class Action
    {
        public DateTime startTime;
        public uint durationMs;
        public bool checkState;
        // todo: lua function
        //LuaScript script;
    }

    class ActionQueue
    {
        private Character owner;
        private Queue<Action> actionQueue;
        private Queue<Action> timerQueue;

        public bool IsEmpty { get { return actionQueue.Count > 0 || timerQueue.Count > 0; } }

        public ActionQueue(Character owner)
        {
            this.owner = owner;
            actionQueue = new Queue<Action>();
            timerQueue = new Queue<Action>();
        }

        public void PushAction(Action action)
        {

        }

        public void Update(DateTime tick)
        {

        }

        public void HandleAction(Action action)
        {

        }

        public void CheckAction(DateTime tick)
        {

        }
    }
}
