# DCF77_CH341_decoder

## Hardware
DCF77 receiver from some source, that outputs a simple digital signal.
CH341 USB thing.
Connect TCO to D0
Connect PON to D3

## Software
The Software will compile under windows with GCC. 

```bash
make.bat
```

When everyting is ok, it will start to receive the signal from the DCF77 receiver and will retrive the time and data.

```bash
c:\temp\DCF77_CH341_decoder>main.exe
001001100001011000001110100101001001X
00101001000011000100101101001100001011000001110100101001001X
OK

2025.05.03 19:16:00
0000000100001110010011110100010000101100000111010010
```

### GCC
```bash
gcc (MinGW-W64 x86_64-ucrt-posix-seh, built by Brecht Sanders, r8) 13.2.0
Copyright (C) 2023 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

### CH341
You need to have CH341DLL.DLL. If you dont have it, then install the drivers for the CH341.

### Meteotime decrypt
The meteotime decrypt is empty.
