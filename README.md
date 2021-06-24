# PlatformIO + ESP32 + FatFs

## Description 

Uses an adapted example code from https://techtutorialsx.com/2018/08/05/esp32-arduino-spiffs-reading-a-file/ with a custom platform https://github.com/maxgerhardt/platform-espressif32 and a tiny bit of scripting to implement a FatFS filesystem instead of the default SPIFFS one.

## How does this work?

Through some [code additions](https://github.com/maxgerhardt/platform-espressif32/commit/e9a8ff797f34656b8a5c6cb4d5512d70c9065e70), projects using the above mentioned `platform = ..` can use, just like [with the ESP8266](https://docs.platformio.org/en/latest/platforms/espressif8266.html#selecting-appropriate-filesystem) the filesystem type they want to use.

```ini
board_build.filesystem = ffat
```

is now a valid option.

The files in the `data/` directory are passed into a filesystem generated tool (e.g., `mkspiffs` `mkfatfs`) with some meta-information (total partition size, page size, ..) to generate a filesystem binary blob (e.g. `ffat.bin`). This blob is then flashed on the ESP32 at a specific address, as indiciated in the partition table.

The platform code does the part of recognizing the `board_build` type, parsing the partition table CSV to get the start and length of the data partiton and preparing the execution of the file system builder tool (e.g., `mkfatfs`). However, since PlatformIO does not yet have `mkfatfs` as a package, the script file `replace_fat.py`, together with the corresponding `platformio.ini` instruction, redirect the executable to be executed from `mkfatfs` to the path of the tool in the local project folder, specific for the used OS (e.g., `tool-mkfatfs/mkfatfs_windows_x86.exe`). These executables come from https://github.com/lorol/arduino-esp32fatfs-plugin/tree/master/extra.

## Quirks

There is a quirk with a needed 0x1000 byte offset as documented [here](https://github.com/maxgerhardt/platform-espressif32/blob/e9a8ff797f34656b8a5c6cb4d5512d70c9065e70/builder/main.py#L119-L128). 

The pre-built binaries are only for Windows x86 (and thus also x64) and Linux x64. If that is not your system, compile [the tool](https://github.com/labplus-cn/mkfatfs) yourself and replace the binaries accordingly (or adapt `replace_fat.py`).

## How do I use this in my own projects?

First of all, look into the issue https://github.com/platformio/platform-espressif32/issues/570 if it has not been yet officially resolved. If yes, follow their instructions.

If not, you can use this method. Make sure your `platformio.ini` 

1. uses my modified platform with `platform = https://github.com/maxgerhardt/platform-espressif32` 
2. selects a [partition table](https://docs.platformio.org/en/latest/platforms/espressif32.html#partition-tables) that has a `fat`-type partition in it. In [Arduino-ESP32's default partitions](https://github.com/espressif/arduino-esp32/tree/master/tools/partitions) there is `default_ffat.csv` and `noota_ffat.csv` that you may use, but you may also write your own partition file
3. make sure you use the `replace_fat.py` script to correctly point to the path of `mkfatfs`, wherever it may be stored for your filesystem and OS. In the standard case, copy the `tool-mkfatfs` folder and `replace_fat.py` in your project and add `extra_scripts = replace_fat.py` to the `platformio.ini`