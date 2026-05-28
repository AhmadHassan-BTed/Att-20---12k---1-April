import { type CMSData, getCMSFilters } from './cms.js';
import type {
  ArmorItemsData,
  CraftingItemsData,
  GemsItemsData,
  HeldItemsData,
  JewelryItemsData,
  MiscellaneousItemsData,
  UnknownItemsData,
  WeaponsItemsData,
} from './items.js';
import type { SpellsData } from './spells.js';

export {
  CMSClassTypes,
  CMSFilterDefinitions,
  CMSRaceToClasses,
  getCMSFilters,
} from './cms.js';
export type * from './cms.js';
export type * from './items.js';
export type * from './spells.js';

export interface EQOAImages {
  [key: string]: string;
}

export interface Contributor {
  login: string;
  id: number;
  commits: number;
}

export type Alignment = 'good' | 'neutral' | 'evil';
export type PoiCategory = 'quest' | 'rare-mob' | 'raid-mob' | 'other';

export interface Location {
  x: number;
  y: number;
}

export interface Town extends Location {
  name: string;
  alignment: Alignment;
  aka?: string;
}

export interface Biome extends Location {
  name: string;
}

export interface POI extends Location {
  name: string;
  icon: string;
  style?: Record<string, string | number>;
  category?: PoiCategory;
  questGuideIds?: string[];
  poiImageKey?: string;
  levelRange?: `${number}-${number}`;
}

export interface MapData {
  towns: Town[];
  biomes: Biome[];
  pois: POI[];
}

export type CoachMapData = Record<string, string[]>;

export const Quests: Record<string, string | object>;
export interface InformationData extends Record<string, string | object> {
  'databases/cms/alchemist': CMSData;
  'databases/cms/archetype': CMSData;
  'databases/cms/bard': CMSData;
  'databases/cms/cleric': CMSData;
  'databases/cms/druid': CMSData;
  'databases/cms/enchanter': CMSData;
  'databases/cms/general': CMSData;
  'databases/cms/magician': CMSData;
  'databases/cms/monk': CMSData;
  'databases/cms/necromancer': CMSData;
  'databases/cms/paladin': CMSData;
  'databases/cms/racial': CMSData;
  'databases/cms/ranger': CMSData;
  'databases/cms/rogue': CMSData;
  'databases/cms/shadowknight': CMSData;
  'databases/cms/shaman': CMSData;
  'databases/cms/warrior': CMSData;
  'databases/cms/were': CMSData;
  'databases/cms/werehunter': CMSData;
  'databases/cms/wizard': CMSData;
  'databases/items/armor': ArmorItemsData;
  'databases/items/crafting': CraftingItemsData;
  'databases/items/gems': GemsItemsData;
  'databases/items/held': HeldItemsData;
  'databases/items/jewelry': JewelryItemsData;
  'databases/items/miscellaneous': MiscellaneousItemsData;
  'databases/items/unknown': UnknownItemsData;
  'databases/items/weapons': WeaponsItemsData;
  'databases/spells/alchemist': SpellsData;
  'databases/spells/bard': SpellsData;
  'databases/spells/cleric': SpellsData;
  'databases/spells/druid': SpellsData;
  'databases/spells/enchanter': SpellsData;
  'databases/spells/magician': SpellsData;
  'databases/spells/monk': SpellsData;
  'databases/spells/necromancer': SpellsData;
  'databases/spells/paladin': SpellsData;
  'databases/spells/ranger': SpellsData;
  'databases/spells/rogue': SpellsData;
  'databases/spells/shadowknight': SpellsData;
  'databases/spells/shaman': SpellsData;
  'databases/spells/warrior': SpellsData;
  'databases/spells/wizard': SpellsData;
}

export const Information: InformationData;
export const Map: MapData;
export const CoachMap: CoachMapData;
export const MapContributors: Contributor[];
export const Images: EQOAImages;

declare const _default: {
  Quests: typeof Quests;
  Information: typeof Information;
  Map: typeof Map;
  CoachMap: typeof CoachMap;
  MapContributors: typeof MapContributors;
  Images: typeof Images;
  getCMSFilters: typeof getCMSFilters;
};

export default _default;