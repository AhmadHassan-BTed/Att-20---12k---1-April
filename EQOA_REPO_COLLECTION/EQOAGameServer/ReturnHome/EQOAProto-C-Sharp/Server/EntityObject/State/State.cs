// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.EntityObject.Combat;

namespace ReturnHome.Server.EntityObject.EntityState
{
    public class State
    {
        protected Entity owner;
        protected Entity target;

        protected bool canInterrupt;
        protected bool interrupt = false;

        protected DateTime startTime;

        protected CommandResult errorResult;

        protected bool isCompleted;

        public State(Character owner, Character target)
        {
            this.owner = owner;
            this.target = target;
        }

        public virtual bool Update(DateTime tick) { return true; }
        public virtual void OnStart() { }
        public virtual void OnComplete() { isCompleted = true; }
        public virtual bool CanChangeState() { return false; }

        public virtual void Cleanup() { }

        public bool IsCompleted()
        {
            return isCompleted;
        }

        public void ChangeTarget(Entity target)
        {
            this.target = target;
        }

        public Entity GetTarget()
        {
            return target;
        }

    }
}
