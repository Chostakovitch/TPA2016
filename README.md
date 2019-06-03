## Dependencies
* [Catch2](https://github.com/catchorg/Catch2) for testing
* [Bela platform](bela.io/) (or [BeagleBone Black](https://beagleboard.org/black) **with some adaptation** for running - will not work as is, especially because of I2C.h dependency)

This lib assume that you know how to open an I2C adapter on Bela. More info to come.

## Compilation

Just use `build_project.sh` Bela script or open the project in web IDE.

## Usage

This library is made to be very simple to use, *e.g.* if you want to set attack time, just call `setAttackTime(value in ms/6dB)`. Internally, there is a conversion table between the register value and the ms/6dB value, but the library takes care of that. You can nevertheless find those conversion tables in the [TPA2016D2 datasheet](http://www.adafruit.com/datasheets/TPA2016D2.pdf).

If you come from the [original library for Arduino](https://github.com/adafruit/Adafruit-TPA2016-Library), be careful as your code probably need some adaptation. This library assume that you already did the conversion.

## Launch tests

\# todo write about separation between test of the amp and test of the code

Make sure to compile the main project with `-fPIC` flag. This can be done by passing `CPPFLAGS=-fPIC` to the Makefile in IDE settings.

```bash
$ cd tests
```

1. Compile main catch file, but do not link. In this file there is no tests, but this helps to [speed up future compilation](https://github.com/catchorg/Catch2/blob/master/docs/slow-compiles.md#top).

```bash
$ g++ catch.cpp -c
```

2. Compile tests using shared objects :
```bash
# /root/Bela/include is the include path used by the Bela platform
$ g++ -I/root/Bela/include catch.o ../build/I2C_TPA2016.o tpa.cpp -o tpa_tests
```

3. Launch tests :
```bash
$ ./tpa_tests
```
