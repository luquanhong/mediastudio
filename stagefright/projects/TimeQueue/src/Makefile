


all:
	g++ -o TimedEventQueue.o -c TimedEventQueue.cpp
	g++ -o TimedQueueDemo.o TimedQueueDemo.cpp -c
	g++ -o demo	TimedEventQueue.o TimedQueueDemo.o -lpthread
	

	
clean:
	rm -rf *.o demo