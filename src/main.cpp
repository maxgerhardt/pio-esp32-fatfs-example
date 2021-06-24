/* from https://www.mischianti.org/2021/04/06/esp32-integrated-ffat-fat-exfat-filesystem-6/ */
#include "Arduino.h"
#include "FS.h"
#include "FFat.h"
#include <esp_spi_flash.h>

void printDirectory(File dir, int numTabs = 3);
void printFile(File &file);

void setup()
{
  Serial.begin(115200);

  delay(500);

  Serial.println(F("Inizializing FS..."));
  if (FFat.begin())
  {
    Serial.println(F("done."));
  }
  else
  {
    Serial.println(F("fail."));
  }

  // To format all space in FFat
  // FFat.format()

  // Get all information of your FFat

  unsigned int totalBytes = FFat.totalBytes();
  unsigned int usedBytes = FFat.usedBytes();
  unsigned int freeBytes = FFat.freeBytes();

  Serial.println("File sistem info.");

  Serial.print("Total space:      ");
  Serial.print(totalBytes);
  Serial.println("byte");

  Serial.print("Total space used: ");
  Serial.print(usedBytes);
  Serial.println("byte");

  Serial.print("Total space free: ");
  Serial.print(freeBytes);
  Serial.println("byte");

  Serial.println();
  Serial.println("Printing filesystem structure and sizes.");

  // Open dir folder
  File dir = FFat.open("/");
  // Cycle all the content
  printDirectory(dir);

  File hello_file = FFat.open("/my_test_file.txt");
  if (hello_file)
  {
    printFile(hello_file);
  }
}

void loop()
{
}

void printDirectory(File dir, int numTabs)
{
  while (true)
  {

    File entry = dir.openNextFile();
    if (!entry)
    {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++)
    {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory())
    {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    }
    else
    {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

void printFile(File &file)
{
  Serial.println("Printing file contents of file: " + String(file.name()));
  while (file.available())
  {
    Serial.write(file.read());
  }
  Serial.println("\nPrinting file done.");
}