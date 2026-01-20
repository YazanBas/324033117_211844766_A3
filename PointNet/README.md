# PointNet setup

## Place Kaggle assets

- Notebook: `PointNet/pointnet-shapenet-dataset.ipynb`
- Dataset: `PointNet/shapenet-core-seg/<unzipped-content>/`
- Utils: `PointNet/lidar-od-scripts/<unzipped-content>/`

## Python 3.10 environment

```
cd PointNet
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
code .
```

## Subset settings

- Categories: Airplane, Chair, Table, Lamp, Car, Mug
- Samples per category: 120
- Epochs: 25 (CPU-safe)

## Subset build

```
python make_subset.py --data-root shapenet-core-seg --out subset --samples-per-class 120 --seed 42
```

## Notebook paths (to be updated once the notebook is present)

- Dataset root: `PointNet/shapenet-core-seg/`
- Utils root: `PointNet/lidar-od-scripts/`
- Subset root: `PointNet/subset/`

## Analysis plan (exactly two options)

- Option 3.1: feature importance (dimension drop + projection)
- Option 3.2: architecture modification + T-Net visualization
