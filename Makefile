

run: src/main.cpp
	clang++ src/main.cpp -o nice.exe

clean: 
	rm *.exe 
