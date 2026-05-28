// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

using System;
using System.Collections.Generic;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.EntityObject.Controllers;
using ReturnHome.Server.EntityObject.AI.Helpers;
using ReturnHome.Server.EntityObject.EntityState;

namespace ReturnHome.Server.EntityObject
{
    public class AIContainer
    {
        private Character owner;
        private Controller controller;
        private Stack<State> states;
        private DateTime latestUpdate;
        private DateTime prevUpdate;
        //TODO: Once pathfinding is added
        //public readonly PathFind pathFind;
        //private TargetFind targetFind;
        private ActionQueue actionQueue;
        private DateTime lastActionTime;

        public AIContainer(Character actor, Controller controller)
        {
            this.owner = actor;
            this.states = new Stack<State>();
            this.controller = controller;
            //this.pathFind = pathFind;
            //this.targetFind = targetFind;
            latestUpdate = DateTime.Now;
            prevUpdate = latestUpdate;
            actionQueue = new ActionQueue(owner);
        }

        public void UpdateLastActionTime(uint delay = 0)
        {
            lastActionTime = DateTime.Now.AddSeconds(delay);
        }

        public DateTime GetLastActionTime()
        {
            return lastActionTime;
        }

        public void Update(DateTime tick)
        {
            prevUpdate = latestUpdate;
            latestUpdate = tick;

            // todo: trigger listeners

            /* if (controller == null && pathFind != null)
             {
                 pathFind.FollowPath();
             }*/

            // todo: action queues
            if (controller != null && controller.canUpdate)
                controller.Update(tick);

            State top;

            while (states.Count > 0 && (top = states.Peek()).Update(tick))
            {
                if (top == GetCurrentState())
                {
                    states.Pop().Cleanup();
                }
            }
            owner.PostUpdate(tick);
        }

        public void CheckCompletedStates()
        {
            while (states.Count > 0 && states.Peek().IsCompleted())
            {
                states.Peek().Cleanup();
                states.Pop();
            }
        }



        public void ClearStates()
        {
            while (states.Count > 0)
            {
                states.Peek().Cleanup();
                states.Pop();
            }
        }

        public void ChangeController(Controller controller)
        {
            this.controller = controller;
        }

        public T GetController<T>() where T : Controller
        {
            return controller as T;
        }

        public bool CanChangeState()
        {
            return GetCurrentState() == null || states.Peek().CanChangeState();
        }

        public void ChangeTarget(Character target)
        {
            if (controller != null)
            {
                controller.ChangeTarget(target);
            }
        }

        public void ChangeState(State state)
        {
            if (CanChangeState())
            {
                if (states.Count <= 10)
                {
                    CheckCompletedStates();
                    states.Push(state);
                }
                else
                {
                    throw new Exception("shit");
                }
            }
        }

        public void ForceChangeState(State state)
        {
            if (states.Count <= 10)
            {
                CheckCompletedStates();
                states.Push(state);
            }
            else
            {
                throw new Exception("force shit");
            }
        }

        public bool IsCurrentState<T>() where T : State
        {
            return GetCurrentState() is T;
        }

        public State GetCurrentState()
        {
            return states.Count > 0 ? states.Peek() : null;
        }

        public DateTime GetLatestUpdate()
        {
            return latestUpdate;
        }

        public void Reset()
        {
            // todo: reset cooldowns and stuff here too?
            //targetFind?.Reset();
            //pathFind?.Clear();
            ClearStates();
        }

        public bool IsSpawned()
        {
            return true; ;
        }

        public bool IsEngaged()
        {
            return false;
            //return owner.currentMainState == SetActorStatePacket.MAIN_STATE_ACTIVE;
        }

        public bool IsDead()
        {
            //return owner.currentMainState == SetActorStatePacket.MAIN_STATE_DEAD ||
            //owner.currentMainState == SetActorStatePacket.MAIN_STATE_DEAD2;

            return false;
        }

        public void IsRoaming()
        {
            // todo: check mounted?
            //return owner.currentMainState == SetActorStatePacket.MAIN_STATE_PASSIVE;
        }

        public void Engage(Character target)
        {
            if (controller != null)
                controller.Engage(target);

        }

        public void Disengage()
        {
            if (controller != null)
                controller.Disengage();

        }

        public void Ability(Character target, uint abilityId)
        {
            if (controller != null)
                controller.Ability(target, abilityId);
        }

        public void Cast(Character target, uint spellId)
        {
            if (controller != null)
                controller.Cast(target, spellId);

        }

        public void WeaponSkill(Character target, uint weaponSkillId)
        {
            if (controller != null)
                controller.WeaponSkill(target, weaponSkillId);
        }

        public void MobSkill(Character target, uint mobSkillId)
        {
            if (controller != null)
                controller.MonsterSkill(target, mobSkillId);
        }

        public void UseItem(Character target, uint slot, uint itemId)
        {
            if (controller != null)
                controller.UseItem(target, slot, itemId);
        }



    }
}
