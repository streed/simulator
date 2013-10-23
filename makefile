CXX = g++
opts = -g -I./unittest-cpp/UnitTest++/src -L./unitest-cpp/UnitTest++/ -Wno-write-strings
libs = -l:./unittest-cpp/UnitTest++/libUnitTest.so

program.o:
	$(CXX) $(opts) -c src/program.cpp -o program.o

memorymanager.o:
	$(CXX) $(opts) -c src/memorymanager.cpp -o memorymanager.o

multiprogram.o: program.o memorymanager.o processgroup.o
	$(CXX) $(opts) -c src/memorymanager.cpp src/program.cpp src/multiprogram.cpp

processgroup.o: multiprogram.o forkshell.o
	$(CXX) $(opts) -c src/multiprogram.cpp src/forkshell.cpp src/processgroup.cpp

process.o: program.o
	$(CXX) $(opts) -c src/process.cpp -o process.o

shell.o: program.o
	$(CXX) $(opts) -c src/shell.cpp -o shell.o

forkshell.o: shell.o process.o
	$(CXX) $(opts) -c src/forkshell.cpp -o forkshell.o

unittest: 
	cd unittest-cpp/UnitTest++;\
	make all;

test_program.o:  unittest program.o shell.o memorymanager.o processgroup.o
	$(CXX) $(opts) src/program.cpp src/shell.cpp src/memorymanager.cpp\
		src/processgroup.cpp src/multiprogram.cpp src/test_program.cpp -o test_program.o $(libs)

test: test_program.o program.o shell.o memorymanager.o processgroup.o multiprogram.o
	echo "Creating ./test which will run all of the automated tests.";\
	$(CXX) $(opts) src/program.cpp src/shell.cpp src/memorymanager.cpp\
			src/processgroup.cpp src/multiprogram.cpp src/test_program.cpp -o test $(libs)

fork: processgroup.o forkshell.o
	echo "Creating ./fork which will run with the seperate processes and pipes.";\
		$(CXX) $(opts) src/shell.cpp src/program.cpp src/multiprogram.cpp\
				src/memorymanager.cpp src/processgroup.cpp src/forkshell.cpp src/main_fork.cpp -o fork

sim: program.o shell.o
	echo "Creating ./sim which when run can run the assembled programs.";\
		$(CXX) $(opts) src/program.cpp src/shell.cpp src/main.cpp -o sim

clean:
	rm *.o;rm test;rm sim;rm fork
