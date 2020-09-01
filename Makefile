CXXFLAGS=-Wall -O3 -g -fno-strict-aliasing -fmax-errors=2
BINARIES=tetrisclock 

# Where our library resides. It is split between includes and the binary
# library in lib
RGB_INCDIR=include
RGB_LIBDIR=lib
LDFLAGS+=-L$(RGB_LIBDIR) -lrgbmatrix -lrt -lm -lpthread

all : $(BINARIES)


tetrisclock : tetrisclock.o TetrisMatrixDraw.o  $(RGB_LIBRARY)
	$(CXX) $(CXXFLAGS) tetrisclock.o TetrisMatrixDraw.o  -o $@ $(LDFLAGS)


%.o : %.cpp
	$(CXX) -I$(RGB_INCDIR) $(CXXFLAGS) -DADAFRUIT_RGBMATRIX_HAT -c -o $@ $<

clean:
	rm -f *.o $(OBJECTS) $(BINARIES)

