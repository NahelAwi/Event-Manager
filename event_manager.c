#include "event_manager.h"
#include "event.h"
#include "priority_queue.h"
#include "member.h"
#include "date.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
//#include <assert.h>

static PQElementPriority dateCopyGeneric(PQElementPriority date);
static void dateDestroyGeneric(PQElementPriority date);
static int dateCompareGeneric(PQElementPriority date1, PQElementPriority date2);
static Event findEventById(PriorityQueue events, int event_id);
static Member findMemberById(PriorityQueue members, int member_id);

struct EventManager_t{
    PriorityQueue events;
    PriorityQueue members;
    Date activation_date;

};


EventManager createEventManager(Date date){
    if (date == NULL){
        return NULL;
    }
    EventManager em = malloc(sizeof(*em));
    if (em == NULL){
        return NULL;
    }
    em->events = pqCreate(copyEventGeneric, freeEventGeneric, equalEventGeneric,
                          dateCopyGeneric, dateDestroyGeneric, dateCompareGeneric);
    
    em->members = pqCreate(copyMemberGeneric, freeMemberGeneric, equalMembersGeneric,
                           copyIntGeneric, freeIntGeneric, compareIntsGeneric);

    em->activation_date = dateCopy(date);
    if (em->activation_date == NULL || em->members == NULL || em->events == NULL){
        destroyEventManager(em);
        return NULL;
    }
    return em;
}


void destroyEventManager(EventManager em){
    if(em){
        pqDestroy(em->events);
        pqDestroy(em->members);
        dateDestroy(em->activation_date);
        free(em);
    }
}


EventManagerResult emAddEventByDate(EventManager em, char* event_name, Date date, int event_id){
    if(!em || !event_name || !date){
        return EM_NULL_ARGUMENT;
    }
    if(dateCompare(date, em->activation_date) < 0){
        return EM_INVALID_DATE;
    }
    if(event_id < 0){
        return EM_INVALID_EVENT_ID;
    }
    Event new_event = createEvent(event_name, event_id, date);
    if(new_event == NULL){
        return EM_OUT_OF_MEMORY;
    }
    if(pqContains(em->events, new_event)){
        freeEvent(new_event);
        return EM_EVENT_ALREADY_EXISTS;
    }
    Event iterator = findEventById(em->events, event_id);
    if(iterator){
        freeEvent(new_event);
        return EM_EVENT_ID_ALREADY_EXISTS;
    }
    if(pqInsert(em->events, new_event, date) == PQ_OUT_OF_MEMORY){
        freeEvent(new_event);
        return EM_OUT_OF_MEMORY;
    }
    freeEvent(new_event);
    return EM_SUCCESS;
}


EventManagerResult emAddEventByDiff(EventManager em, char* event_name, int days, int event_id){
    if(!em || !event_name){
        return EM_NULL_ARGUMENT;
    }
    if(days < 0){
        return EM_INVALID_DATE;
    }
    if(event_id < 0){
        return EM_INVALID_EVENT_ID;
    }
    
    Event iterator = findEventById(em->events, event_id);
    if(iterator){
        return EM_EVENT_ID_ALREADY_EXISTS;
    }

    Date current_date = dateCopy(em->activation_date);
    if(current_date == NULL){
        return EM_OUT_OF_MEMORY;
    }
    for(int i = 0 ; i < days ; i++){
        dateTick(current_date);
    }

    Event new_event = createEvent(event_name, event_id, current_date);
    dateDestroy(current_date);
    if(new_event == NULL){
        return EM_OUT_OF_MEMORY;
    }
    if(pqContains(em->events, new_event)){
        freeEvent(new_event);
        return EM_EVENT_ALREADY_EXISTS;
    }
    if(pqInsert(em->events, new_event, getEventDate(new_event)) == PQ_OUT_OF_MEMORY){
        freeEvent(new_event);
        return EM_OUT_OF_MEMORY;
    }
    freeEvent(new_event);
    return EM_SUCCESS;

}


EventManagerResult emRemoveEvent(EventManager em, int event_id){
    if(!em){
        return EM_NULL_ARGUMENT;
    }
    if(event_id < 0){
        return EM_INVALID_EVENT_ID;
    }

   Event event_iterator = findEventById(em->events, event_id);
    if(event_iterator){
        PQ_FOREACH(Member, member_iterator1, getEventStaff(event_iterator)){
            Member member_iterator2 = findMemberById(em->members, getIdInt(member_iterator1));
            if(member_iterator2){
                removeFromMemberEventsInChargeOf(member_iterator2);
            }
        }
        pqRemoveElement(em->events, event_iterator);
        return EM_SUCCESS;
    }
    return EM_EVENT_NOT_EXISTS;
}




EventManagerResult emChangeEventDate(EventManager em, int event_id, Date new_date){
    if(!em || !new_date){
        return EM_NULL_ARGUMENT;
    }
    if(dateCompare(new_date, em->activation_date) < 0){
        return EM_INVALID_DATE;
    }
    if(event_id < 0){
        return EM_INVALID_EVENT_ID;
    }
    Event event_iterator = findEventById(em->events, event_id);
    if(event_iterator){
        Event new_event = createEventWithoutPQ(getEventName(event_iterator), event_id, new_date);
        if(!new_event){
            return EM_OUT_OF_MEMORY;
        }
        if(pqContains(em->events, new_event)){
            freeEventWithoutPQ(new_event);
            return EM_EVENT_ALREADY_EXISTS;
        }
        setEventStaff(new_event, getEventStaff(event_iterator));
        pqRemoveElement(em->events, event_iterator);
        if(pqInsert(em->events, new_event, new_date) == PQ_OUT_OF_MEMORY){
            freeEvent(new_event);
            return EM_OUT_OF_MEMORY;
        }
        freeEvent(new_event);
        return EM_SUCCESS;
    }
    return EM_EVENT_ID_NOT_EXISTS;
}


EventManagerResult emAddMember(EventManager em, char* member_name, int member_id){
    if(!em || !member_name){
        return EM_NULL_ARGUMENT;
    } 
    if(member_id < 0){
        return EM_INVALID_MEMBER_ID;
    }
    
    Member tmpMember = findMemberById(em->members,member_id);
    if(tmpMember){
        return EM_MEMBER_ID_ALREADY_EXISTS;
    }
    Member new_memeber = createMember(member_name, member_id);
    if(new_memeber == NULL){
        return EM_OUT_OF_MEMORY;
    }
    PQElementPriority tmpId = getIdGeneric(new_memeber);
    if(pqInsert(em->members, new_memeber, tmpId) == PQ_OUT_OF_MEMORY){
        freeIntGeneric(tmpId);
        freeMember(new_memeber);
        return EM_OUT_OF_MEMORY;
    }
    freeIntGeneric(tmpId);
    freeMember(new_memeber);
    return EM_SUCCESS;
}


EventManagerResult emAddMemberToEvent(EventManager em, int member_id, int event_id){
    if(!em){
        return EM_NULL_ARGUMENT;
    }
    if(event_id < 0){
        return EM_INVALID_EVENT_ID;
    }
    if(member_id < 0){
        return EM_INVALID_MEMBER_ID;
    }
    Event event_iterator = findEventById(em->events, event_id);
    if(event_iterator){
        Member member_iterator = findMemberById(em->members,member_id);
        if(member_iterator){
            if(pqContains(getEventStaff(event_iterator), member_iterator)){
                return EM_EVENT_AND_MEMBER_ALREADY_LINKED;
            }
            addToMemberEventsInChargeOf(member_iterator);
            addMember(event_iterator, member_iterator);
            return EM_SUCCESS;
        }
            return EM_MEMBER_ID_NOT_EXISTS;
    }
    return EM_EVENT_ID_NOT_EXISTS;
}


EventManagerResult emRemoveMemberFromEvent (EventManager em, int member_id, int event_id){
    if(!em){
        return EM_NULL_ARGUMENT;
    }
    if(event_id < 0){
        return EM_INVALID_EVENT_ID;
    }
    if(member_id < 0){
        return EM_INVALID_MEMBER_ID;
    }
    Event event_iterator = findEventById(em->events, event_id);
    if(event_iterator){
        Member Member_iterator = findMemberById(em->members, member_id);
        if(Member_iterator){
            if(!pqContains(getEventStaff(event_iterator), Member_iterator)){
                return EM_EVENT_AND_MEMBER_NOT_LINKED;
            }
            removeMember(event_iterator, Member_iterator);
            return EM_SUCCESS;
        }
        return EM_MEMBER_ID_NOT_EXISTS;
    }   
    return EM_EVENT_ID_NOT_EXISTS;
}


EventManagerResult emTick(EventManager em, int days){
    if(!em){
        return EM_NULL_ARGUMENT;
    }
    if(days <= 0){
        return EM_INVALID_DATE;
    }
    for(int i = 0; i < days; i++){
        dateTick(em->activation_date);
    }
    while( dateCompare( getEventDate(pqGetFirst(em->events)), em->activation_date) < 0){
            emRemoveEvent(em, getEventId(pqGetFirst(em->events)));
    }
    return EM_SUCCESS;
    
}


int emGetEventsAmount(EventManager em){
    if(!em){
        return -1;
    }
    return pqGetSize(em->events);
}


char* emGetNextEvent(EventManager em){
    if(!em){
        return NULL;
    }

    return getEventName(pqGetFirst(em->events));
}


void emPrintAllEvents(EventManager em, const char* file_name){
    int day,month,year;
    FILE* stream = fopen(file_name, "w");
    PQ_FOREACH(Event, event_iterator, em->events){

        dateGet(getEventDate(event_iterator), &day, &month, &year);
        fprintf(stream,"%s,%d.%d.%d",getEventName(event_iterator),day,month,year);

        PQ_FOREACH(Member, member_iterator,getEventStaff(event_iterator)){
            fprintf(stream,",%s",getname(member_iterator));
        }
        
        fprintf(stream,"\n");
    }
    fclose(stream);
}



void emPrintAllResponsibleMembers(EventManager em, const char* file_name){
    FILE* stream = fopen(file_name, "w");
    int numOfEvents = pqGetSize(em->events) + 1;
    int *inOrder = malloc(sizeof(int) * numOfEvents);
    if(inOrder == NULL){
        return;
    }
    for(int i = 0; i < numOfEvents; i++ ){
        inOrder[i] = 0;
    }
    PQ_FOREACH(Member, firstIterator, em->members){
        inOrder[getEventsInChargeOf(firstIterator)] += 1;
    }
    for(int i = numOfEvents - 1 ; i > 0 ; i-- ){
        if(inOrder[i] > 0){
            PQ_FOREACH(Member, secondIterator, em->members){
                if(getEventsInChargeOf(secondIterator) == i){
                    fprintf(stream,"%s,%d\n", getname(secondIterator), i);
                }
            }
        }
    }
    free(inOrder);
    fclose(stream);
}


/////////////////////////////////////////////DATE GENERIC
static PQElementPriority dateCopyGeneric(PQElementPriority date){
    if (!date) {
        return NULL;
    }
    int day, month, year;
    dateGet((Date) date, &day, &month, &year);
    Date copy = dateCreate(day, month, year);
    if (!copy) {
        return NULL;
    }
    return copy;
}


static void dateDestroyGeneric(PQElementPriority date){
    dateDestroy((Date) date);
}


static int dateCompareGeneric(PQElementPriority date1, PQElementPriority date2){
    return dateCompare((Date) date2, (Date) date1);
}
//////////////////////////////////////////

static Event findEventById(PriorityQueue events, int event_id){
    PQ_FOREACH(Event, iterator, events){
        if(getEventId(iterator) == event_id)
            return iterator;
    }
    return NULL;
}

static Member findMemberById(PriorityQueue members, int member_id){
    PQ_FOREACH(Member, iterator, members){
        if(getIdInt(iterator) == member_id)
            return iterator;
    }
    return NULL;
}