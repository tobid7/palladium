# Palladium

**Framework / Engine to create Homebrew Apps / Games**

## Building

Install Dependencies:

```bash
# This Command will install everything
(dkp-)pacman -S --noconfirm 3ds-dev 3ds-portlibs
```

Building:

If you want to have the lib in a projects `libs` dir or so you can simply add `-DCMAKE_INSTALL_PREFIX=./res` to the cmake command and copy the dirs from res to you libs folder

```bash
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release # Make sure to build in Release Mode (exept you want to debug some issues)
make
make install
```

## Libraries

| Name | Last Updated | Platform | Depends |
|---|---|---|---|
| pd-core | 0.3.3 | multi | none |
| pd-external | 0.1.0 | multi | none |
| pd-image | 0.3.3 | multi | pd-core |
| pd-lib3ds | 0.2.4 | 3ds | pd-core, pd-drivers |
| pd-net | 0.2.4 | 3ds | pd-core, pd-lib3ds |
| pd-lithium | 0.3.3 | multi | pd-core |
| pd-sound | 0.2.4 | 3ds | pd-core, mpg123 |
| pd-overlays | 0.2.4 | 3ds | pd-core, pd-image, pd-lib3ds, pd-lithium, pd-ui7 |
| pd-ui7 | 0.3.3 | multi | pd-core, pd-lithium |
| pd-app | 0.2.4 | 3ds | pd-core, pd-image, pd-lib3ds, pd-lithium |

## Credits

| Icon | Username | Description |
|---|---|---|
| <img src="https://github.com/tobid7.png" alt="https://github.com/tobid7" width="48"/>                 | [tobid7](https://github.com/tobid7)                 | main dev of RenderD7, Palladium                               |
| <img src="https://github.com/devkitpro.png" alt="https://github.com/devkitpro" width="48"/>           | [devkitpro](https://github.com/devkitpro)           | devkitarm, picasso, libctru and citro3d                       |
| <img src="https://github.com/Universal-Team.png" alt="https://github.com/Universal-Team" width="48"/> | [Universal-Team](https://github.com/Universal-Team) | Inspiration for Lang System, Cia Installer Code |
| <img src="https://github.com/nothings.png" alt="https://github.com/nothings" width="48"/>             | [nothings](https://github.com/nothings)             | stb_image(_write) and stb_truetype                            |
| <img src="https://github.com/nlohmann.png" alt="https://github.com/nlohmann" width="48"/>             | [nlohmann](https://github.com/nlohmann)             | for json.hpp                                                  |
