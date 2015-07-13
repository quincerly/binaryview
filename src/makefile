COMPILE = gcc -c -g
LINK = gcc -g

CFLAGS = 

INCLUDEDIR = -I/usr/include -I/usr/include/readline -I/usr/X11R6/include -I/usr/local/include -I${LIBBINARY}/include 
LIBDIR = -L/usr/local/lib -L/usr/lib -L/usr/X11R6/lib -L/usr/X11/lib -L${LIBBINARY}/lib

GTK2CFLAGS = `pkg-config --cflags gtk+-2.0`
GTK2LIBS = `pkg-config --libs gtk+-2.0`
LIBS = -lglut -lGLU -lGL -lXmu -lXext -lX11 -lm -ltiff -lbinary

BINDIR = ../bin/
OBJS = binaryanim.o GLlobe.o GLwind.o GLjet.o writetiff.o GLps.o
GTK2OBJS = binaryanimgtk2.o GLlobe.o GLwind.o GLjet.o writetiff.o GLps.o

# Link objects
both :	${OBJS}
	rm -f ${BINDIR}/binaryanim
	${LINK} ${CFLAGS} ${LIBDIR} -o ${BINDIR}/binaryanim ${OBJS} ${LIBS}

gtk2 :	${GTK2OBJS}
	rm -f ${BINDIR}/binaryanim
	${LINK} ${CFLAGS} ${LIBDIR} -o ${BINDIR}/binaryanim ${GTK2OBJS} ${LIBS} ${GTK2LIBS} 

# Make separate object files
binaryanim.o : binaryanim.c drawbinary.c GLlobe.c GLdisc.c GLstream.c getpar.c
	${COMPILE} ${CFLAGS} ${INCLUDEDIR} -o binaryanim.o binaryanim.c
binaryanimgtk2.o : binaryanim.c drawbinary.c GLlobe.c GLdisc.c GLstream.c getpar.c
	${COMPILE} ${CFLAGS} ${GTK2CFLAGS} ${INCLUDEDIR} -o binaryanimgtk2.o binaryanim.c -D_USEGTK2
GLlobe.o : GLlobe.c
	${COMPILE} ${CFLAGS} ${INCLUDEDIR} -o GLlobe.o GLlobe.c
GLwind.o : GLwind.c
	${COMPILE} ${CFLAGS} ${INCLUDEDIR} -o GLwind.o GLwind.c
GLjet.o : GLjet.c
	${COMPILE} ${CFLAGS} ${INCLUDEDIR} -o GLjet.o GLjet.c
GLps.o : GLps.c
	${COMPILE} ${CFLAGS} ${INCLUDEDIR} -o GLps.o GLps.c
writetiff.o : writetiff.c
	${COMPILE} ${CFLAGS} ${INCLUDEDIR} -o writetiff.o writetiff.c


clean:
	rm -f *.o
	rm -f *~
	rm -f ${BINDIR}/binaryanim
