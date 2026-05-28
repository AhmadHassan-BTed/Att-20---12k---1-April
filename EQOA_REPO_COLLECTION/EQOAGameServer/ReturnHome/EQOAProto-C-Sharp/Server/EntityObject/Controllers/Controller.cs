// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.EntityObject;

namespace ReturnHome.Server.EntityObject.Controllers
{
    public abstract class Controller
    {
        protected Character owner;

        protected DateTime lastCombatTickScript;
        protected DateTime lastUpdate;
        public bool canUpdate = true;
        protected bool autoAttackEnabled = true;
        protected bool castingEnabled = true;
        protected bool weaponSkillEnabled = true;
        //TODO: Once pathfinding is enabled
        //protected PathFind pathFind;
        //protected TargetFind targetFind;

        public Controller(Character owner)
        {
            this.owner = owner;
        }

        public abstract void Update(DateTime tick);
        public abstract bool Engage(Character target);
        public abstract void Cast(Character target, uint spellId);
        public virtual void WeaponSkill(Character target, uint weaponSkillId) { }
        public virtual void MonsterSkill(Character target, uint mobSkillId) { }
        public virtual void UseItem(Character target, uint slot, uint itemId) { }
        public abstract void Ability(Character target, uint abilityId);
        public abstract void RangedAttack(Character target);
        public virtual void Spawn() { }
        public virtual void Despawn() { }


        public virtual void Disengage()
        {
        }

        public virtual void ChangeTarget(Character target)
        {
        }

        public bool IsAutoAttackEnabled()
        {
            return autoAttackEnabled;
        }

        public void SetAutoAttackEnabled(bool isEnabled)
        {
            autoAttackEnabled = isEnabled;
        }

        public bool IsCastingEnabled()
        {
            return castingEnabled;
        }

        public void SetCastingEnabled(bool isEnabled)
        {
            castingEnabled = isEnabled;
        }

        public bool IsWeaponSkillEnabled()
        {
            return weaponSkillEnabled;
        }

        public void SetWeaponSkillEnabled(bool isEnabled)
        {
            weaponSkillEnabled = isEnabled;
        }
    }
}
