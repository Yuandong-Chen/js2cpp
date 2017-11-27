all:
	g++ -std=c++11 minicompiler/CodeGen.cpp minilexer/MiniLex.cpp minilexer/RegularExp.cpp minilexer/MacroHandler.cpp -o ./js2cpp 
test:
	g++ -std=c++11 minicompilerCodeGen.cpp -o ./a.out
clean:
	-rm ./js2cpp
	-rm ./*.out
	-rm ./*.cpp
	-rm ./out.cpp
	-rm ./.DS_Store
	-rm ./example/.DS_Store
	-rm ./minicompiler/.DS_Store
	-rm ./minilexer/.DS_Store
	-rm ./miniparser/.DS_Store
	-rm -rf ./a.out.*