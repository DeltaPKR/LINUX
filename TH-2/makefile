CC = g++

all: parallel_scheduler.so demo-application

parallel_scheduler.o: parallel_scheduler.cpp parallel_scheduler.h
	$(CC) -fPIC -c $< -o $@

parallel_scheduler.so: parallel_scheduler.o
	$(CC) -shared $^ -o $@

demo-application: main.cpp parallel_scheduler.so
	$(CC) -fPIC main.cpp -L. -l:parallel_scheduler.so -Wl,-rpath=. -o $@

clean:
	rm -f *.o *.so demo-application

