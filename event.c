#include "event.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <assert.h>

static char* stringDuplicate(const char* str);


struct Event_t{
    char* event_name;
    int event_id;
    Date event_date;
    PriorityQueue event_staff;
};


Event createEvent(char* name, int id, Date date){
    if(!name || !date || id < 0){
        return NULL;
    }
    Event new_event = malloc(sizeof(*new_event));
    if(!new_event){
        return NULL;
    }
    new_event->event_name = stringDuplicate(name);
    new_event->event_id = id;
    new_event->event_date = dateCopy(date);
    new_event->event_staff = pqCreate(copyMemberGeneric, freeMemberGeneric, equalMembersGeneric,
                                      copyIntGeneric, freeIntGeneric, compareIntsGeneric);
    if(!new_event->event_staff){
        freeEvent(new_event);
        return NULL;
    }
    return new_event;
}
 

Event createEventWithoutPQ(char* name, int id, Date date){
    if(!name || !date || id < 0){
        return NULL;
    }
    Event new_event = malloc(sizeof(*new_event));
    if(!new_event){
        return NULL;
    }
    new_event->event_name = stringDuplicate(name);
    new_event->event_id = id;
    new_event->event_date = dateCopy(date);
    return new_event;
}


Event copyEvent(Event event){
    if(!event){
        return NULL;
    }
    Event new_event = createEventWithoutPQ(event->event_name, event->event_id,event->event_date);
    if(!new_event){
        return NULL;
    }
    new_event->event_staff = pqCopy(event->event_staff);
    if(!new_event->event_staff){
        freeEvent(new_event);
        return NULL;
    }
    return new_event;
}


void freeEvent(Event event){
    free(event->event_name);
    dateDestroy(event->event_date);
    pqDestroy(event->event_staff);
    free(event);
}


void freeEventWithoutPQ(Event event){
    free(event->event_name);
    dateDestroy(event->event_date);
    free(event);
}


bool equalEvents(Event event1, Event event2){
    if(!event1 || !event2){
        return NULL;
    }
    return (dateCompare(event1->event_date, event2->event_date) == 0 && strcmp(event1->event_name, event2->event_name) == 0);
}


void addMember(Event event, Member member){
    //assert(!event || !member);
    if(event && member){
        PQElementPriority  tmpId = getIdGeneric(member);
        pqInsert(event->event_staff, member, tmpId);
        freeIntGeneric(tmpId);
    }
}


void removeMember(Event event, Member member){
    //assert(!event || !member);
    if(event && member){
        removeFromMemberEventsInChargeOf(member);
        pqRemoveElement(event->event_staff, member);
    }
}


PQElementPriority copyIntGeneric(PQElementPriority n) {
    if (!n) {
        return NULL;
    }
    int *copy = malloc(sizeof(*copy));
    if (!copy) {
        return NULL;
    }
    *copy = *(int *) n;
    return copy;
}


void freeIntGeneric(PQElementPriority n) {
    free(n);
}


int compareIntsGeneric(PQElementPriority n1, PQElementPriority n2) {
    return (*(int *) n2 - *(int *) n1);
}


PQElement copyEventGeneric(PQElement event){
    if (!event) {
        return NULL;
    }
    Event copy = copyEvent((Event)event);;
    if (!copy) {
        return NULL;
    }
    return copy;
}


void freeEventGeneric(PQElement event){
    freeEvent((Event) event);
}


bool equalEventGeneric(PQElement event1, PQElement event2){
    return equalEvents((Event) event1, (Event) event2);
}

int getEventId(Event event){
    if(!event){
        return -1;
    }
    return event->event_id;
}

char* getEventName(Event event){
    if(!event){
        return NULL;
    }
    return event->event_name;
}


Date getEventDate(Event event){
    if(!event){
        return NULL;
    }
    return event->event_date;
}


PriorityQueue getEventStaff(Event event){
     if(!event){
        return NULL;
    }
    return (event->event_staff);

}

/*
void setNewDate(Event event, Date new_date){
     //assert(!event || !new_date);
     if(event && new_date){
        event->event_date = dateCopy(new_date);
     }
}
*/

void setEventStaff(Event event, PriorityQueue new_staff){
     //assert(!event || !new_staff);
     if(event && new_staff){
        event->event_staff = pqCopy(new_staff);
     }
}


static char* stringDuplicate(const char* str) {
	char* copy = malloc(strlen(str)+1);
	return copy ? strcpy(copy, str) : NULL;
}