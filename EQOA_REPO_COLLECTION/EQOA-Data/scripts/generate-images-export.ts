import { promises as fs } from "node:fs";
import path from "node:path";
import { fileURLToPath } from "node:url";

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const rootDir = path.resolve(__dirname, "..");

const imagesDir = path.join(rootDir, "images");
const indexFile = path.join(rootDir, "src", "index.ts");

const IMAGE_EXTENSIONS = new Set([".png", ".jpg", ".jpeg", ".gif", ".webp", ".svg"]);

function toIdentifier(baseName: string): string {
  const cleaned = baseName.replace(/[^A-Za-z0-9_$]/g, "_");
  return /^[A-Za-z_$]/.test(cleaned) ? cleaned : `_${cleaned}`;
}

async function collectImageFiles(dir: string, parentRelative = ""): Promise<string[]> {
  const entries = await fs.readdir(dir, { withFileTypes: true });
  const files: string[] = [];

  for (const entry of entries) {
    const relativePath = parentRelative ? path.join(parentRelative, entry.name) : entry.name;
    const fullPath = path.join(dir, entry.name);

    if (entry.isDirectory()) {
      const nestedFiles = await collectImageFiles(fullPath, relativePath);
      files.push(...nestedFiles);
      continue;
    }

    if (!entry.isFile()) {
      continue;
    }

    if (!IMAGE_EXTENSIONS.has(path.extname(entry.name).toLowerCase())) {
      continue;
    }

    files.push(relativePath);
  }

  return files;
}

async function main(): Promise<void> {
  const imageFiles = (await collectImageFiles(imagesDir)).sort((a, b) => a.localeCompare(b));

  if (imageFiles.length === 0) {
    throw new Error(`No image files found in ${imagesDir}`);
  }

  const usedKeys = new Set<string>();
  const lines = imageFiles.map((relativeFilePath) => {
    const normalizedPath = relativeFilePath.split(path.sep).join("/");
    const relativeWithoutExt = normalizedPath.replace(/\.[^./]+$/, "");
    const key = toIdentifier(relativeWithoutExt);
    if (usedKeys.has(key)) {
      throw new Error(`Duplicate generated key "${key}" for image "${relativeFilePath}"`);
    }
    usedKeys.add(key);
    return `  ${key}: new URL("../images/${normalizedPath}", import.meta.url).href,`;
  });

  const block = `export const Images = {\n${lines.join("\n")}\n} as const;`;
  const source = await fs.readFile(indexFile, "utf8");

  const pattern = /export const Images = \{[\s\S]*?\} as const;/m;
  if (!pattern.test(source)) {
    throw new Error(`Could not find Images export block in ${indexFile}`);
  }

  const updated = source.replace(pattern, block);
  await fs.writeFile(indexFile, updated, "utf8");

  console.log(`Updated ${indexFile} with ${imageFiles.length} images.`);
}

main().catch((error: unknown) => {
  const message = error instanceof Error ? error.message : String(error);
  console.error(message);
  process.exit(1);
});