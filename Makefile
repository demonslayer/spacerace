#  List of all programs you want to build
#EX=ex10 ex11 ex12 ex13 ex14 ex15 ex16 ex17 ex18 ex19 ex20 ex21 ex22 ex23 ex24 ex25 ex26 ex27 ex28 ex29 ex30 ex31 ex32 ex33 ex34 ex35 sdl20
EX=lighting, ex15, ex8, spacerace, justparticles, ex17

LIBS=-framework GLUT -framework OpenGL

#  Main target
all: $(EX)

#  Generic compile rules
.c.o:
	gcc -c -O -Wall $<

#  Generic compile and link
%: %.c CSCIx229.a
	gcc -Wall -O3 -o $@ $^ $(LIBS)

#  Delete unwanted files
clean:
	rm -f *.o
	rm -f lighting
	rm -f ex15
	rm -f ex8
	rm -f spacerace
	rm -f justparticles
	rm -f ex17

#  Create archive (include glWindowPos here if you need it)
CSCIx229.a:fatal.o loadtexbmp.o print.o project.o errcheck.o object.o
	ar -rcs CSCIx229.a $^

#  SDL version of ex20
sdl20: sdl20.o fatal.o loadtexbmp.o project.o errcheck.o printsdl.o
	gcc -Wall -O3 -o $@ $^ -lSDL -lSDL_mixer -lGLU -lGL -lm

#  Obligatory UNIX inside joke
love:
	@echo "not war?"
