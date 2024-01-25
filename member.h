#ifndef MEMBER_H
#define MEMBER_H

#include "priority_queue.h"
#include <stdbool.h>

typedef struct Member_t *Member;

Member createMember(char* name, int id);

Member copyMember(Member member);

void freeMember(Member member);

bool equalMembers(Member member1, Member member2);

PQElementPriority getIdGeneric(Member member);

int getIdInt(Member member);

char* getname(Member member);

int getEventsInChargeOf(Member member);

PQElement copyMemberGeneric(PQElement member);

void freeMemberGeneric(PQElement member);

bool equalMembersGeneric(PQElement member1, PQElement member2);

void addToMemberEventsInChargeOf(Member Member);

void removeFromMemberEventsInChargeOf(Member Member);

#endif