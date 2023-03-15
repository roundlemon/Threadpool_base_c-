main : main.o TaskQueue.o ThreadPool.o
	g++ -o main main.o TaskQueue.o ThreadPool.o -lpthread

main.o : main.cpp
	g++ -g -c  main.cpp

TaskQueue.o : TaskQueue.cpp TaskQueue.h
	g++ -g -c TaskQueue.cpp

ThreadPool.o :ThreadPool.cpp ThreadPool.h
	g++ -g -c ThreadPool.cpp

clean :
	rm -f main main.o TaskQueue.o ThreadPool.o
