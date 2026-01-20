#!/usr/bin/env python3
import argparse
import os
import random
import shutil
from pathlib import Path

def find_dataset_root(root: Path) -> Path:
    synset_path = root / "synsetoffset2category.txt"
    if synset_path.exists():
        return root
    candidates = list(root.glob("*/synsetoffset2category.txt"))
    if len(candidates) == 1:
        return candidates[0].parent
    raise FileNotFoundError("synsetoffset2category.txt not found under data root")

def read_synset_map(path: Path) -> dict:
    mapping = {}
    with path.open("r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            parts = line.split()
            if len(parts) >= 2:
                name = " ".join(parts[:-1])
                synset = parts[-1]
                mapping[name.lower()] = synset
    return mapping

def resolve_category_dir(root: Path, synset: str, name: str, subdir: str | None = None) -> Path | None:
    base = root / subdir if subdir else root
    by_synset = base / synset
    if by_synset.exists():
        return by_synset
    by_name = base / name
    if by_name.exists():
        return by_name
    return None

def copy_points_mode(dataset_root: Path, out_root: Path, name: str, synset: str, samples: int, rng: random.Random) -> int:
    points_dir = resolve_category_dir(dataset_root, synset, name, "points")
    labels_dir = resolve_category_dir(dataset_root, synset, name, "points_label")
    if not points_dir or not points_dir.is_dir():
        raise FileNotFoundError(f"points directory not found for category {name}")
    if not labels_dir or not labels_dir.is_dir():
        raise FileNotFoundError(f"points_label directory not found for category {name}")

    files = [p for p in points_dir.iterdir() if p.is_file() and not p.name.startswith(".")]
    files.sort()
    pick = min(samples, len(files))
    chosen = rng.sample(files, pick) if len(files) > pick else files

    out_points = out_root / "points" / synset
    out_labels = out_root / "points_label" / synset
    out_points.mkdir(parents=True, exist_ok=True)
    out_labels.mkdir(parents=True, exist_ok=True)

    for f in chosen:
        shutil.copy2(f, out_points / f.name)
        label_file = labels_dir / f.name
        if label_file.exists():
            shutil.copy2(label_file, out_labels / f.name)

    return len(chosen)

def copy_folder_mode(dataset_root: Path, out_root: Path, name: str, synset: str, samples: int, rng: random.Random) -> int:
    category_dir = resolve_category_dir(dataset_root, synset, name, None)
    if not category_dir or not category_dir.is_dir():
        raise FileNotFoundError(f"category directory not found for {name}")

    entries = [p for p in category_dir.iterdir() if not p.name.startswith(".")]
    entries.sort()
    pick = min(samples, len(entries))
    chosen = rng.sample(entries, pick) if len(entries) > pick else entries

    out_cat = out_root / synset
    out_cat.mkdir(parents=True, exist_ok=True)

    for entry in chosen:
        target = out_cat / entry.name
        if entry.is_dir():
            shutil.copytree(entry, target, dirs_exist_ok=True)
        else:
            shutil.copy2(entry, target)

    return len(chosen)

def main() -> None:
    parser = argparse.ArgumentParser(description="Create a CPU-friendly ShapeNet subset.")
    parser.add_argument("--data-root", default="shapenet-core-seg", help="Path to ShapeNet dataset root")
    parser.add_argument("--out", default="subset", help="Output directory for subset")
    parser.add_argument("--samples-per-class", type=int, default=120, help="Samples per category")
    parser.add_argument("--seed", type=int, default=42, help="Random seed")
    parser.add_argument(
        "--categories",
        nargs="+",
        default=["Airplane", "Chair", "Table", "Lamp", "Car", "Mug"],
        help="Category names (match synsetoffset2category.txt)",
    )
    parser.add_argument("--force", action="store_true", help="Overwrite existing subset directory")
    args = parser.parse_args()

    data_root = Path(args.data_root)
    dataset_root = find_dataset_root(data_root)
    synset_path = dataset_root / "synsetoffset2category.txt"

    mapping = read_synset_map(synset_path)

    out_root = Path(args.out)
    if out_root.exists():
        if not args.force:
            raise FileExistsError(f"Output directory {out_root} already exists. Use --force to overwrite.")
        shutil.rmtree(out_root)
    out_root.mkdir(parents=True, exist_ok=True)

    shutil.copy2(synset_path, out_root / "synsetoffset2category.txt")

    rng = random.Random(args.seed)

    points_mode = (dataset_root / "points").is_dir() and (dataset_root / "points_label").is_dir()

    summary = []
    for name in args.categories:
        synset = mapping.get(name.lower())
        if not synset:
            raise KeyError(f"Category '{name}' not found in synsetoffset2category.txt")

        if points_mode:
            count = copy_points_mode(dataset_root, out_root, name, synset, args.samples_per_class, rng)
        else:
            count = copy_folder_mode(dataset_root, out_root, name, synset, args.samples_per_class, rng)
        summary.append((name, synset, count))

    print("Subset summary:")
    print("Category\tSynset\tSamples")
    for name, synset, count in summary:
        print(f"{name}\t{synset}\t{count}")

if __name__ == "__main__":
    main()
