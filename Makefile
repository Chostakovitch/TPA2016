# Inspiration taken from : https://www.oreilly.com/library/view/c-cookbook/0596007612/ch01s18.html
CLEANEXTS   = o so d
CXX = g++
LDFLAGS =
CPPFLAGS =

SOURCES     = I2C_TPA2016.cpp
TEST_DIR		= tests
TEST_SRC		= $(TEST_DIR)/catch.cpp $(TEST_DIR)/tpa.cpp
HEADERS 		= I2C_TPA2016.h
OUTPUTFILE  = libtpa2016.so
OUTPUTTEST	= $(TEST_DIR)/tpa_test
INSTALLPREFIX = /usr
LIBDIR  = lib
INCDIR = include

.PHONY: all install clean

all: $(OUTPUTFILE)

$(OUTPUTFILE): $(subst .cpp,.o,$(SOURCES))
	$(CXX) -shared -fPIC $(LDFLAGS) -o $@ $^

install:
	mkdir -p $(LIBDIR) $(INCDIR)
	install -m 644 -o root -g root $(OUTPUTFILE) $(INSTALLPREFIX)/$(LIBDIR)
	install -m 644 -o root -g root $(HEADERS) $(INSTALLPREFIX)/$(INCDIR)

# Launch tests to check default values of the amplifier
test_defaults: test
	$(OUTPUTTEST) *default*

# Launch tests to check features of the library
test_lib: tests
	$(OUTPUTTEST) ~*default*

test: $(OUTPUTFILE) $(OUTPUTTEST)

$(OUTPUTTEST): $(subst .cpp,.o,$(TEST_SRC))
	$(CXX) $(LDFLAGS) -ltpa2016 -o $@ $^

clean:
	for file in $(CLEANEXTS); do rm -f *.$$file; done

include $(subst .cpp,.d,$(SOURCES))

%.d: %.cpp
	$(CXX) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
rm -f $@.$$$$
