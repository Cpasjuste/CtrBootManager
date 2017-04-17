# CtrBootManager

CtrBootManager is a boot manager for some 3DS homebrew applications, like HBL and CFW.

## Dependencies
- devkitpro-4.5 at least (ctrulib-1.0 at least)
- libconfig in portlibs (tried with v1.5)

## Building
### The recommended way:
 1. `mkdir build/`
 2. `cd build/`
 3. `cmake -DCMAKE_TOOLCHAIN_FILE=../DevkitArm3DS.cmake ../`
 5. `make`

Binaries should now be in the `build` folder.

## Credits
### For contributions to hb_menu:
 * smea : code
 * GEMISIS : code
 * fincs : code
 * mtheall : code
 * Fluto : graphics
 * Arkhandar : graphics
 * dotjasp : graphics (regionfree icon)

### Other credits:
Readme by gemarcano
This project also uses [Brahma](https://github.com/patois/Brahma). See its repository for its credits.
