# Genesis DH head-unit packages

This repository contains the three source components and the complete
deployable feature-package trees used by the head unit:

- `sources/fuel_economy/` contains the passive AppInfo fuel-data runtime source.
- `sources/projected_guidance/` builds the Android Auto/CarPlay IC and HUD guidance
  runtime.
- `sources/vehicle_home/` builds the AppHomeMenu integration runtime.
- `feature_packages/projected-guidance/` contains the projected-guidance
  manifest, byte-patch recipes, and payload layout.
- `feature_packages/vehicle-information/` contains the Vehicle Information
  manifest, regional recipes, QML, icons, and payload layout.

The package metadata and static payload files are tracked. Only the three
compiled `.so` payloads are generated and ignored by Git. All builds are
controlled by the single `Makefile`.

The package-private patch recipes are included in `feature_packages/`. Projected
Guidance's five public prerequisite recipes remain in the patcher's root
`byte_patches/` catalog.

## Update basis

The primary development and in-car test target is the
`2017_Genesis_G80_EU` update, version `DHPE.EUR.SOP.03.013`.

Compatibility with other regions and software versions is controlled by the
exact stock-binary identities in the patcher recipes.

## Build

Build all three IA-32 libraries:

```sh
make
```

This builds each library directly into its manifest-declared package path:

```text
feature_packages/projected-guidance/payload/app/lib/libPG.so
feature_packages/vehicle-information/payload/app/lib/libFE.so
feature_packages/vehicle-information/payload/app/lib/libVH.so
```

`make clean` removes only the three generated libraries.
