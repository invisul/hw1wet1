#include <stdlib.h>
#include <string.h>
#include "PartySystem.h"
#include "Party.h"
#include "set.h"
#include "list.h"
#include <assert.h>
#include <stdio.h>


struct partySystem {
    Set parties;
   // Condition condition;
};

 //TODO we need to change that. isn't good
 /*
static void* partyCopy(void* party){
    if (!party) return NULL;
    if (saveParty((Party)party,"tmp.txt")==1){
    return NULL;
    }
    return createParty("tmp.txt");
}
  */
static void* partyCopy(void* party){
     return party;
}

static void partyNotDsetory(void* party){
    return;
}
static void partyDestroyUtil(void* party){
    destroyParty((Party)party);
    return;
}

/**
 *
 * @param party_a
 * @param party_b
 * @return true if equal in SOME manner
 */
static int partyCmp(void* party_a,void* party_b){
    //if (!party_a || !party_b) return false;
    assert(party_a && party_b);
    if (haveCommonMembers((Party)party_a,(Party)party_b))
        return 0;
    char* name_a,*name_b;
    PartyCode code_a,code_b;
    int size_a,size_b;
    int flag=1;
    getPartyDetails((Party)party_a,&name_a,&code_a,&size_a);
    getPartyDetails((Party)party_b,&name_b,&code_b,&size_b);
    if (strcmp(name_a,name_b)==0) flag=0;
    if (strcmp(code_a,code_b)==0) flag=0;
    free(name_a);
    free(name_b);
    free(code_a);
    free(code_b);
    return flag;
}

PartySystem createPartySystem(){
    PartySystem  new = malloc(sizeof(*new));
    if (!new) return NULL;
    new->parties=setCreate(partyCopy,partyDestroyUtil,partyCmp);
    if (!new->parties){
        free(new);
        return NULL;
    }
    return new;
}

//TODO we need to change that. isn't good
/*
PartySystemResult addParty(PartySystem system, char *party_data_file){
    if (!system || !party_data_file) return SYSTEM_FAIL;
    Party tmp= createParty(party_data_file);
    if (!tmp) return SYSTEM_FAIL;
    if (setAdd(system->parties,tmp)==SET_SUCCESS){
        destroyParty(tmp);
        return SYSTEM_SUCCESS;
    }
    destroyParty(tmp);
    return SYSTEM_FAIL;
}
*/
PartySystemResult addParty(PartySystem system, char *party_data_file){
    if (!system || !party_data_file) return SYSTEM_FAIL;
    Party tmp= createParty(party_data_file);
    if (!tmp) return SYSTEM_FAIL;
    tmp=NULL;
    return PARTY_SUCCESS;
}

PartySystemResult deleteParty(PartySystem system, PartyCode prt_code){
    if (!system || !prt_code) return SYSTEM_FAIL;
    PartyCode code;
    char* name;
    int size;
    SET_FOREACH(Party,tmp,system->parties){
        getPartyDetails(tmp,&name,&code,&size);
        if (strcmp(prt_code,code)==0){//אולי צריך להשוות גם בין השמות TODO
            free(name);
            free(code);
            setRemove(system->parties,tmp);
            return SYSTEM_SUCCESS;
        }
        free(name);
        free(code);

    }
    return SYSTEM_FAIL;
}



//TODO I CHANGING THAT
/*
PartySystemResult displayPartySystem(PartySystem party_system,
                                     bool compare(Party prt1, Party prt2)){
    if (!party_system || !compare) return SYSTEM_FAIL;
    SET_FOREACH(Party,tmp,party_system->parties){
        displayParty(tmp,1,5);
        printf("\n");
        for(int i=0;i<80;i++) printf("-"); //TODO why 79 and not 80?
        printf("\n");
    }
    return SYSTEM_SUCCESS;
}
 */


typedef int (*Condition)(Party ,Party );
Condition cond;
static int compareconverter(void* p1,void* p2){
    return cond((Party)p1,(Party)p2);
}
PartySystemResult displayPartySystem(PartySystem party_system,
                                     int compare(Party prt1, Party prt2)){
    cond=compare;
    if (!party_system || !compare) return SYSTEM_FAIL;
    Set tmp_set=setCreate(partyCopy,partyNotDsetory,compareconverter);
    SET_FOREACH(Party,tmp,party_system->parties) {
        setAdd(tmp_set,tmp);
    }
    char* name;
    char* code;
    int size;
    SET_FOREACH(Party,tmp,tmp_set) {
        getPartyDetails(tmp,&name,&code,&size);
        displayParty(tmp,1,size);
        printf("\n");
        for(int i=0;i<80;i++) printf("-"); //TODO why 79 and not 80?
        printf("\n");
        free(name);
        free(code);
    }

    return SYSTEM_SUCCESS;
}


void destroyPartySystem(PartySystem system){
    if (!system) return;
    setDestroy(system->parties);
    free(system);
    return;
}





///

bool ppsize(Party a,Party b){
    return true;
}

/*
int main (){
    PartySystem  sys= createPartySystem();
    addParty(sys,"kulanu.txt");
    addParty(sys,"yarok.txt");
    displayPartySystem(sys,ppsize);
    deleteParty(sys,"mrs");
    displayPartySystem(sys,ppsize);
    return 0;
}
 */
