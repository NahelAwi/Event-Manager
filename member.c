#include "member.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <assert.h>

static char* stringDuplicate(const char* str);


struct Member_t{
    char* name;
    int id;
    int events_in_charge_of;
};


Member createMember(char* name, int id){
    if(!name){
        return NULL;
    }
    Member new_member = malloc(sizeof(*new_member));
    if(!new_member){
        return NULL;
    }
    new_member->name = stringDuplicate(name);
    new_member->id = id;
    new_member->events_in_charge_of = 0;
    return new_member;
}


PQElementPriority getIdGeneric(Member member){
    if(!member){
        return NULL;
    }
    int *copy = malloc(sizeof(*copy));
    if (!copy) {
        return NULL;
    }
    *copy = member->id;
    return copy;
}


int getIdInt(Member member){
    //assert(!member);
    if(member){
        return member->id;
    }
    return -1;
}

char* getname(Member member){
    if(!member){
        return NULL;
    }
    return member->name;
}


int getEventsInChargeOf(Member member){
    //assert(!member);
    if(member){
        return member->events_in_charge_of;
    }
    return 0;
}


Member copyMember(Member member){
    if(!member){
        return NULL;
    } 
    Member new_member = createMember(member->name, member->id);
    if(!new_member){
        return NULL;
    }
    new_member->events_in_charge_of = member->events_in_charge_of;
    return new_member;
}


void freeMember(Member member){
    free(member->name);
    free(member);
}


bool equalMembers(Member member1, Member member2){
    if(!member1 || !member2){
        return NULL;
    }
    return(member1->id == member2->id);
}


PQElement copyMemberGeneric(PQElement member) {
    if (!member) {
        return NULL;
    }
    Member copy = copyMember((Member) member);
    if (!copy) {
        return NULL;
    }
    return copy;
}


void freeMemberGeneric(PQElement member) {
    freeMember((Member)member);
}


bool equalMembersGeneric(PQElement member1, PQElement member2) {
    return equalMembers((Member) member1, (Member) member2);
}


void addToMemberEventsInChargeOf(Member member){
    //assert(!member);
    if(member){
        member->events_in_charge_of += 1;
    }
}


void removeFromMemberEventsInChargeOf(Member member){
    //assert(!member);
    if(member){
        member->events_in_charge_of -= 1;
    }
}


static char* stringDuplicate(const char* str) {
	char* copy = malloc(strlen(str)+1);
	return copy ? strcpy(copy, str) : NULL;
}