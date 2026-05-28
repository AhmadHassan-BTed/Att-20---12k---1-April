// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ReturnHome.Server.EntityObject.Player;

namespace ReturnHome.Server.EntityObject.EntityState
{
    class InactiveState : State
    {
        private DateTime endTime;
        private uint durationMs;
        public InactiveState(Character owner, uint durationMs, bool canChangeState) :
            base(owner, null)
        {
            if (!canChangeState)
            this.durationMs = durationMs;
            endTime = DateTime.Now.AddMilliseconds(durationMs);
        }

        public override bool Update(DateTime tick)
        {
            if (durationMs == 0)
            {
                if (owner.IsDead())
                    return true;

                //if (!owner.statusEffects.HasStatusEffectsByFlag(StatusEffectFlags.PreventMovement))
                // return true;
            }

            if (durationMs != 0 && tick > endTime)
            {
                return true;
            }

            return false;
        }
    }
}
