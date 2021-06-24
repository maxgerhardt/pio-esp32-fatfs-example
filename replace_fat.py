import os
Import("env")

is_windows = os.name == 'nt'
# Since PlatformIO does not yet have a tool-mkfatfs, we have
# to last-minute correct the path to a locally downloaded version.
env.Replace(
    MKSPIFFSTOOL="tool-mkfatfs/" + "mkfatfs_windows_x86" if is_windows else "mkfatfs_linux_x64",
    ESP32_SPIFFS_IMAGE_NAME="ffat.bin"
)