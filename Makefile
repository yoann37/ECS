sources = $(wildcard *.cpp)
objets= $(sources:.cpp=.o)
.PHONY: clean mrproper depend

exec: programme clean
	export LD_LIBRARY_PATH=/Users/yoann/Desktop/Prog/SFML-2.4.2-osx-clang/lib && ./programme 

programme: depend $(objets)
	g++ -std=c++11 -o $@ $(objets) -L/Users/yoann/Desktop/Prog/SFML-2.4.2-osx-clang/lib -lsfml-graphics -lsfml-window -lsfml-system

%.o: %.cpp
	g++ -std=c++11 -O2 -o $@ -c $< -I/Users/yoann/Desktop/Prog/SFML-2.4.2-osx-clang/include


clean:
	rm *.o *.bak

mrproper: clean
	rm programme

depend:
	makedepend $(sources)

# DO NOT DELETE
