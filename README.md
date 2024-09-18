# DSOflash

Flash tool specially made for Hantek DSO2x1x (DSO2C10, DSO2C15, DSO2D10, DSO2D15)
Should also work with other spi nand flash attached to a F1C100s/200s cpu.
SPI nor support was removed, use xfel for that.
Includes the ECC area, so the file will be slight bigger than the theorical flash size,
ex. for 1Gbit(128MB), the file will be 132MB.

When writing, the input file size will be compared against the flash capacity,
they should match or the operation will be aborted.

## Usage
```sh
dsoflash detect                                  - Detect spi flash
dsoflash erase                                   - Erase spi flash
dsoflash read <file>                             - Read spi contents into a file
dsoflash write <file>                            - Write file to spi flash  (Erase not required)
```

DSOflash was derived from xfel.
