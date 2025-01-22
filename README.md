Hantek DSO2000 Flash Tool
=========================

[![CodeFactor](https://www.codefactor.io/repository/github/jorenar/hantek-dso2k-flash-tool/badge)](https://www.codefactor.io/repository/github/jorenar/hantek-dso2k-flash-tool)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/8e69adea5ec94007b5e98b96c2804643)](https://app.codacy.com/gh/Jorenar/hantek-dso2k-flash-tool/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=Jorenar_hantek-dso2k-flash-tool&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=Jorenar_hantek-dso2k-flash-tool)

Flash tool for Hantek DSO2000 oscilloscopes (DSO2C10, DSO2C15, DSO2D10, DSO2D15)

Might also work with other SPI NAND flash attached to a F1C100s/200s CPU (although untested).

Includes the ECC area, so the file will be slightly bigger than the theoretical flash size, e.g. for 1 Gbit (128 MiB), the file will be 132 MiB.

When writing, the input file size will be compared against the flash capacity, they should match or the operation will be aborted.

## Usage
```sh
dsoflash detect            - Detect spi flash
dsoflash erase             - Erase spi flash
dsoflash read <file>       - Read spi contents into a file
dsoflash write <file>      - Write file to spi flash  (erase not required)
```

---

This is a fork of [DavidAlfa](https://www.eevblog.com/forum/profile/?u=555408)'s
Dsoflash ([available on Google Drive](https://drive.google.com/drive/folders/13hwsIJd3eGrnHWCLPcQrPQiAb7rRx3W0)),
which in turn was derived from [xboot/xfel](https://github.com/xboot/xfel).
