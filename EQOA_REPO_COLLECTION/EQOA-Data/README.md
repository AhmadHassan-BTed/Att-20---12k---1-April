# EQOA Data
Structured EverQuest Online Adventures (EQOA) data package for quests, reference databases, map zone coordinates, and image assets used by Jadiction.com and available for other projects to utilize.

Published as ESM for apps, sites, tools, and editors that need EQOA data without scraping.

## What This Package Includes
- `Quests`: quest datasets split into per-guide files (`Quests/<set>/<guide>.json`), with `_meta.json` for set title, subtitle, and nested group labels.
- `Information`: supplemental reference datasets, including top-level guides and nested database entries such as `Information["databases/spells/alchemist"]`.
- `Map`: structured world map data exported from `Map/zones.json`, including `towns`, `biomes`, and `pois`.
- `MapContributors`: contributor metadata for the map sources, exported from `Map/contributors.json`.
- `Images`: generated URL exports for all files in `images/`, including nested folders.
- CMS helpers: exported class/type/filter helpers built from the Character Mastery System data.

## Install
```bash
npm install eqoa-data
```

## Usage
```ts
import {
  Quests,
  Information,
  Map,
  MapContributors,
  Images,
  getCMSFilters,
  CMSRaceToClasses,
  CMSClassTypes,
} from "eqoa-data";

const lowLevelQuests = Quests["1-20"];
const wizardGuide = lowLevelQuests.fayspire.wizard;
const alchemistSpells = Information["databases/spells/alchemist"];
const startingCities = Information.starting_cities;
const bearWereImage = Images.weres_bearwere;
const levelTaggedPoi = Map.pois.find((poi) => poi.levelRange);
const filters = getCMSFilters();

console.log(lowLevelQuests.title); // from _meta.json
console.log(wizardGuide.guide); // markdown guide body
console.log(alchemistSpells);
console.log(Map.towns[0]); // { name, x, y, alignment, aka? }
console.log(levelTaggedPoi?.levelRange); // e.g. "20-25"
console.log(MapContributors[0]?.login);
console.log(filters.filters);
console.log(CMSRaceToClasses.Human);
console.log(CMSClassTypes.Wizard); // "Caster"

if (wizardGuide.contributors) {
  wizardGuide.contributors.forEach((contributor) => {
    console.log(`Contributed by: ${contributor.login}`);
    // { login, id, commits }
  });
}
```

You can also import published images directly:
```ts
import mapImage from "eqoa-data/images/EQOA_Map.png";
```

## Export Overview
- `Quests`: generated quest data assembled from the `Quests/` directory.
- `Information`: generated information data assembled from the `Information/` directory, including nested database paths.
- `Map`: structured map data from `Map/zones.json`, including `towns`, `biomes`, and `pois`.
- `MapContributors`: aggregated contributor list for the map data from `Map/contributors.json`.
- `Images`: generated image URL map based on filenames under `images/`.
- `CMSRaceToClasses`: race-to-class availability derived from CMS quest applicability data.
- `CMSClassTypes`: class-to-type mapping (`Tank`, `Healer`, `Melee`, `Caster`).
- `CMSFilterDefinitions`: filter metadata used by CMS UIs.
- `getCMSFilters()`: returns filter definitions, `raceToClasses`, `classToRaces`, and `classToType`.

## Repository Layout
- `Quests/`: source JSON folders for quest data (`_meta.json` plus one JSON file per guide).
- `Information/`: source JSON files for non-quest reference data and nested databases.
- `Map/`: map and zone coordinate source data.
- `images/`: static image assets, including nested folders.
- `src/QuestsData.ts`: generated quest aggregation into exported `Quests`.
- `src/InformationData.ts`: generated information aggregation into exported `Information`.
- `src/index.ts`: main package exports for `Quests`, `Information`, `Map`, `Images`, and CMS helpers.

## Contributing
Contributions are welcome for quest corrections, new guides, information updates, map updates, database entries, formatting cleanup, and image assets such as screenshots that improve guide clarity.

When your PR is approved and merged, you'll automatically appear as a contributor on the guide or data files you helped with. Contributor metadata is pulled from GitHub commit history during the release workflow, including the `MapContributors` export for map work.

### Contribution Flow
Use the editor at [https://jadiction.com/en/eqoa/editor](https://jadiction.com/en/eqoa/editor)

Or:
1. Fork the repo and create a branch.
2. Make your changes to the source JSON and asset files.
3. Open a Pull Request with a short summary of what changed.

### Contribution Notes
- Preserve the existing JSON style and key naming patterns.
- Guide content should stay markdown-friendly because it is rendered by React Markdown.
- To the best of your ability, use proper grammar and keep formatting clean and readable.
- Strings containing `cons color` in guides are automatically replaced with a standardized `CONS color [Image]`.
- To reference quest images added under `images/guides/`, use this markdown format:

```md
![image alt attribute aka simple text explanation|WIDTHxHEIGHT](file_name)
![simple image summary|WxH](file_name)
```

In-use examples from quest guides:

```md
![Idol of Malice location|319x205](35_idol_of_malice)
![Idol of Lust location|320x205](35_idol_of_lust)
```

- When adding quest images under `images/guides/`, stick to the existing naming style.

Examples:
- `1-20_freeport_bard_level_1_example.png`
- `17_klick_anon_example.png`
- `35_idol_of_lust.png`