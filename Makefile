CC = gcc
OBJS1 = date.o member.o event.o event_manager.o priority_queue.o event_manager_example_tests.o
OBJS2 = priority_queue.o pq_example_tests.o
EXEC1 = event_manager
EXEC2 = priority_queue
DEBUG = # now empty, assign -g for debug
CFLAGS = -std=c99 -Wall -Werror -pedantic-errors $(DEBUG) 

$(EXEC2) : $(OBJS2)
	$(CC) $(CFLAGS) $(OBJS2) -o $@

$(EXEC1) : $(OBJS1)
	$(CC) $(CFLAGS) $(OBJS1) -o $@


    
date.o : date.c date.h
member.o : member.c member.h priority_queue.h
event.o : event.c event.h priority_queue.h member.h date.h
event_manager.o : event_manager.c event_manager.h event.h priority_queue.h member.h date.h 
priority_queue.o : priority_queue.c priority_queue.h 
event_manager_example_tests.o : event_manager_example_tests.c test_utilities.h
pq_example_tests.o : pq_example_tests.c test_utilities.h



clean:
	rm -f $(OBJS1) $(EXEC1)
	rm -f $(OBJS2) $(EXEC2)