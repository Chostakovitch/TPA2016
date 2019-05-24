## Dependencies
* [Catch2](https://github.com/catchorg/Catch2) for testing
* [Bela platform](bela.io/) or [BeagleBone Black](https://beagleboard.org/black) **with some adaptation** for running

This lib assume that you know how to open an I2C adapter on Bela. More info to come.

## Compilation

Just use `build_project.sh` Bela script or open the project in web IDE.

## Launch tests

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
