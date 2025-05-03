cls
del /f /q main.exe
del /f /q *.o
set LIBRARY=-lmingw32 -lwinmm
set OPTION=
c:\gcc-14.2.0-no-debug\bin\gcc -m32 dcf77.c %LIBRARY% -I.\ -L.\ -c
c:\gcc-14.2.0-no-debug\bin\gcc -m32 dcf77_meteotime.c %LIBRARY% -I.\ -L.\ -c
c:\gcc-14.2.0-no-debug\bin\gcc -m32 dcf77_meteotime_decrypt.c %LIBRARY% -I.\ -L.\ -c
c:\gcc-14.2.0-no-debug\bin\gcc -m32 dcf77_meteotime_packet.c %LIBRARY% -I.\ -L.\ -c
c:\gcc-14.2.0-no-debug\bin\gcc -m32 dcf77_meteotime_text.c %LIBRARY% -I.\ -L.\ -c
c:\gcc-14.2.0-no-debug\bin\gcc -m32 dcf77_parser.c %LIBRARY% -I.\ -L.\ -c
c:\gcc-14.2.0-no-debug\bin\gcc -m32 dcf77_phase.c %LIBRARY% -I.\ -L.\ -c
c:\gcc-14.2.0-no-debug\bin\gcc -m32 dcf77_pon.c %LIBRARY% -I.\ -L.\ -c
c:\gcc-14.2.0-no-debug\bin\gcc -m32 dcf77_simple.c %LIBRARY% -I.\ -L.\ -c
c:\gcc-14.2.0-no-debug\bin\gcc -m32 rtc.c %LIBRARY% -I.\ -L.\ -c
c:\gcc-14.2.0-no-debug\bin\gcc -m32 rtcconverter.c %LIBRARY% -I.\ -L.\ -c
c:\gcc-14.2.0-no-debug\bin\gcc -m32 daylight.c %LIBRARY% -I.\ -L.\ -c

c:\gcc-14.2.0-no-debug\bin\gcc -m32 main.c %OPTION% %LIBRARY% -I.\ -L.\ -o main dcf77.o dcf77_meteotime.o dcf77_meteotime_decrypt.o dcf77_meteotime_packet.o dcf77_meteotime_text.o dcf77_parser.o dcf77_phase.o dcf77_pon.o dcf77_simple.o rtc.o rtcconverter.o daylight.o
main.exe