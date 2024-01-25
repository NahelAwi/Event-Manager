#ifndef EVENT_H
#define EVENT_H

#include "date.h"
#include "member.h"
#include "priority_queue.h"
#include <stdbool.h>

typedef struct Event_t *Event;

Event createEvent(char* name, int id, Date date);

Event createEventWithoutPQ(char* name, int id, Date date);

Event copyEvent(Event event);

void freeEvent(Event event);

void freeEventWithoutPQ(Event event);

bool equalEvents(Event event1, Event event2);

void addMember(Event event, Member member);

void removeMember(Event event, Member member);

PQElement copyEventGeneric(PQElement event);

void freeEventGeneric(PQElement event);

bool equalEventGeneric(PQElement event1, PQElement event2);

PQElementPriority copyIntGeneric(PQElementPriority n);

void freeIntGeneric(PQElementPriority n);

int compareIntsGeneric(PQElementPriority n1, PQElementPriority n2);

int getEventId(Event event);

char* getEventName(Event event);

Date getEventDate(Event event);

PriorityQueue getEventStaff(Event event);

void setNewDate(Event event, Date new_date);

void setEventStaff(Event event, PriorityQueue new_staff);

#endif