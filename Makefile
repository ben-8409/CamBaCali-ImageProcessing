CC=c++
CXXFLAGS=-std=c++11 -Wall -I. -I ./thirdparty/include -I ~/.local/include -I /usr/include/Poco


OPENCV_CFLAGS=$(shell pkg-config --cflags opencv)
OPENCV_LIBS=-L $(shell pkg-config --libs opencv)

POCO_LIBS=-L ${HOME}/.local/lib -lPocoFoundation -lPocoNet
#POCO_LIBS=-L /usr/lib -lPocoFoundation -lPocoNet

VPATHS=../

modules=colordetector.o jsongenerator.o screenregistration.o serveraccess.o utils.o
objects=main.o clmasks.o $(modules)

all: calibtool clmasks

calibtool: main.o $(modules)
	$(CC) $(OPENCV_LIBS) $(POCO_LIBS) main.o $(modules) -o $@

clmasks: clmasks.o $(objects)
	$(CC) $(OPENCV_LIBS) $(POCO_LIBS) clmasks.o $(modules) -o $@

$(objects): %.o: %.cpp
	$(CC) -c $(CXXFLAGS) $(OPENCV_CFLAGS) $< -o $@

.PHONY:clean
clean:
	rm *.o calibtool clmasks
