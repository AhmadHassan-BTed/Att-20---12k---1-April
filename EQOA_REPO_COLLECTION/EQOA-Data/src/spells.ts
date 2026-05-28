export interface SpellContributor {
  login: string;
  id: number;
  commits: number;
}

export type SpellSource =
  | 'Animator MCA'
  | 'Buglar MCA'
  | 'Channeler MCA'
  | 'CM'
  | 'Crusader MCA'
  | 'Death Knight MCA'
  | 'Defender MCA'
  | 'Defiler MCA'
  | 'EG Stone'
  | 'Elder MCA'
  | 'Exorcist MCA'
  | 'Faithful MCA'
  | 'Forester MCA'
  | 'Gaurdian MCA'
  | 'Geomancer MCA'
  | 'Hero MCA'
  | 'Hunter MCA'
  | 'Lich MCA'
  | 'MC'
  | 'MCA'
  | 'Merchant'
  | 'Minstral MCA'
  | 'Mob Drop'
  | 'Mystic MCA'
  | 'Ninja MCA'
  | 'None'
  | 'PoD Drop'
  | 'Pyro MCA'
  | 'Quest'
  | 'Sensei MCA'
  | 'Slayer MCA'
  | 'Sorcerer MCA'
  | 'Spellbinder MCA'
  | 'Storm Caller MCA'
  | 'Unknown'
  | 'Wilding MCA';

export type SpellIcon =
  | 'Area_of_effect'
  | 'Bleeding_Heart'
  | 'Boot'
  | 'Boot_Shackled'
  | 'Boot_Sticky'
  | 'Bow'
  | 'Brazier'
  | 'Cold'
  | 'Defense_Active'
  | 'DoT'
  | 'Fear'
  | 'Fire'
  | 'Fist'
  | 'Forage'
  | 'Gate'
  | 'Heal'
  | 'Mask'
  | 'Moon'
  | 'Muscle_Arm'
  | 'Phys_stat_decrease'
  | 'Physical_Alteration'
  | 'Physical_Stat'
  | 'Play_Instruments'
  | 'Poison'
  | 'Profile_Cloud'
  | 'Profile_Lightning'
  | 'Profile_Pinch'
  | 'Profile_Web'
  | 'Red_Cross'
  | 'Scales'
  | 'Skeletal_Arm'
  | 'Stun'
  | 'Summon'
  | 'Summoning'
  | 'Swimming'
  | 'Sword'
  | 'Wand'
  | 'Weapons_Ranged';

export interface SpellAbility {
  name: string;
  description: string | null;
  source: SpellSource | null;
  icon: SpellIcon | null;
}

export interface SpellLevel {
  level: number;
  abilities: SpellAbility[];
}

export interface SpellsData {
  title: string;
  description: string;
  contributors: SpellContributor[];
  list: SpellLevel[];
}