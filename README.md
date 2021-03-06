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

The platform code does the part of recognizing the `board_build.filesystem` type, parsing the partition table CSV to get the start and length of the data partiton and preparing the execution of the file system builder tool (e.g., `mkfatfs`). However, since PlatformIO does not yet have `mkfatfs` as a package, the script file `replace_fat.py`, together with the corresponding `platformio.ini` instruction, redirect the executable to be executed from `mkfatfs` to the path of the tool in the local project folder, specific for the used OS (e.g., `tool-mkfatfs/mkfatfs_windows_x86.exe`). These executables come from https://github.com/lorol/arduino-esp32fatfs-plugin/tree/master/extra.

## Quirks

There is a quirk with a needed 0x1000 byte offset as documented [here](https://github.com/maxgerhardt/platform-espressif32/blob/e9a8ff797f34656b8a5c6cb4d5512d70c9065e70/builder/main.py#L119-L128). 

The pre-built binaries are only for Windows x86 (and thus also x64) and Linux x64. If that is not your system, compile [the tool](https://github.com/labplus-cn/mkfatfs) yourself and replace the binaries accordingly (or adapt `replace_fat.py`).

## How do I use this in my own projects?

First of all, look into the issue https://github.com/platformio/platform-espressif32/issues/570 if it has not been yet officially resolved. If yes, follow their instructions.

If not, you can use this method. Make sure to 

1. use my modified platform with `platform = https://github.com/maxgerhardt/platform-espressif32` in the `platformio.ini`
2. select a [partition table](https://docs.platformio.org/en/latest/platforms/espressif32.html#partition-tables) that has a `fat`-type partition in it. In [Arduino-ESP32's default partitions](https://github.com/espressif/arduino-esp32/tree/master/tools/partitions) there is `default_ffat.csv` and `noota_ffat.csv` that you may use, but you may also write your own partition file. Example: `board_build.partitions = default_ffat.csv` in the `platformio.ini`
3. Add `board_build.filesystem = ffat` in the `platformio.ini` to convey to the platform code that you want to use FFat
4. use the `replace_fat.py` script to correctly point to the path of `mkfatfs`, wherever it may be stored for your filesystem and OS. In the standard case, copy the `tool-mkfatfs` folder and `replace_fat.py` in your project and add `extra_scripts = replace_fat.py` to the `platformio.ini`

## Expected output

First, import the project into VSCode using the PlatformIO extension. If you work from the CLI, open a CLI.

First, use the [project task](https://docs.platformio.org/en/latest/integration/ide/vscode.html#project-tasks) "esp32dev -> Platform -> Upload Filesystem Image", or `pio run -t uploadfs` from the CLI, to build and upload the filesystem to a connected ESP32.

Output should involve

```
..
Building in release mode
Building FFat image from 'data' directory to .pio\build\esp32dev\ffat.bin
Pack file, dir_s: data\my_test_file.txtdir_d: /spiflash/my_test_file.txt  file name: my_test_file.txt
g_flashmem[0]: \xebsize: 1503232
Looking for upload port...
Auto-detected: COM40
Uploading .pio\build\esp32dev\ffat.bin
esptool.py v3.0
Serial port COM40
...
```

Then, upload & monitor the firmware using the esp32dev -> General -> Upload and Monitor project task, or `pio run -t upload -t monitor` from the CLI.

The serial monitor should now hopefully show that the FAT32 partition has been successfully found and the filesystem structure and content is printed.

```
--- Quit: Ctrl+C | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---
ets Jun  8 2016 00:22:57

rst:0x10 (RTCWDT_RTC_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0018,len:4
load:0x3fff001c,len:1044
load:0x40078000,len:10124
load:0x40080400,len:5828
entry 0x400806a8
Inizializing FS...
done.
File sistem info.
Total space:      1478656byte
Total space used: 4096byte
Total space free: 1474560byte

Printing filesystem structure and sizes.
                        /my_test_file.txt               26
Printing file contents of file: /my_test_file.txt
This is some test content.
Printing file done.
```
