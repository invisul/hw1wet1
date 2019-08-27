#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "set.h"
#include "Party.h"
#include "list.h"
#include <assert.h>
typedef struct candidates_t {
    char* name;
    char* id;
    Gender gender;
}*Candidatep,Candidate;


struct party{
    char* name;
    char* code;
    List candidates;
};


Party partyCopy(Party party){
    Party new= malloc(sizeof(*new));
    if (!new) return NULL;
    new->name=malloc(sizeof(*new->name)*(strlen(party->name)+1));
    new->code=malloc(sizeof(*new->code)*(strlen(party->code)+1));
    new->candidates=listCopy(party->candidates);
    if (!new->candidates || !new->name || !new->code){
        free(new->name);
        free(new->code);
        listDestroy(new->candidates);
        free(new);
        return NULL;
    }
    strcpy(new->name,party->name);
    strcpy(new->code,party->code);
    return new;

}

static bool legalId(char* id){
    if (strlen(id) != 9){
        return false;
    }
    for (int i=0;i<9;i++){
        if (id[i] > '9' || id[i] < '0') {
            return false;
        }
    }
    return true;
}

static void*  candidateCopy(void* candidate) {
    Candidatep new= malloc(sizeof(*new));
    if(!new) return NULL;
    new->gender=((Candidatep)(candidate))->gender;
    new->name=malloc(sizeof(*new->name)*strlen(((Candidatep)(candidate))->name)+1);
    new->id=malloc(sizeof(*new->id)*10);
    if (!new->name || !new->id){
        free(new->name);
        free(new->id);
        return NULL;
    }
    strcpy(new->name,((Candidatep)(candidate))->name);
    strcpy(new->id,((Candidatep)(candidate))->id);
    return (void*)new;

}
static void destroyCandidate(void* candidate){
    if(!candidate) return;
    free(((Candidatep)(candidate))->id);
    free(((Candidatep)(candidate))->name);
    free(((Candidatep)(candidate)));
    return;
}
/*
static bool candidateCmp(char* id_1,char* id_2){
    return !strcpy(id_1,id_2);
}
*/
//UNNECCESSARY ITS FOR A TEST SHOULD BE REMVOED
bool partycmp(Party a, Party b){
    if (!haveCommonMembers(a,b)) return false;
    if (strcmp(a->name,b->name)!=0) return false;
    if (strcmp(a->code,b->code)!=0) return false;
    return true;
}


/*
char *inputString(FILE* fp){
//The size is extended by the input with the value of the provisional
    char *str;
    int ch;
    size_t len = 0,size=10;
    str = realloc(NULL, sizeof(char)*size);//size is start size
    if(!str)return str;
    while ((EOF!=(ch=fgetc(fp))   && ch == ' ')|| ch == '\n');
    if (ch == EOF ){
        free(str);
        return NULL;
    }
    str[len++]=ch;
    while(EOF!=(ch=fgetc(fp)) && ch != '\n' && ch != ' '){
        str[len++]=ch;
        if(len==size){
            str = realloc(str, sizeof(char)*(size+=size));
            if(!str)return str;
        }
    }
    str[len++]='\0';

    return realloc(str, sizeof(char)*len);
}
*/



Party createParty(char *party_data_file) {
    FILE *party_data = fopen(party_data_file, "r");
    if (!party_data) return NULL;
    char name[50], id[10], gender[50];
    if(fscanf(party_data,"%s %s", name, gender) !=2){
        fclose(party_data);
        return NULL;
    }
    Party new= malloc(sizeof(*new));
    if (!new) {
        fclose(party_data);
        return NULL;
    }
    new->name=malloc(sizeof(*new->name)*(strlen(name)+1));
    new->code=malloc(sizeof(*new->code)*(strlen(gender)+1));
    new->candidates=listCreate(candidateCopy,destroyCandidate);
    if (!new->name || !new->code || !new->candidates){
        free(new->candidates);
        fclose(party_data);
        free(new->code);
        free(new->name);
        free(new);
        return NULL;
    }
    strcpy(new->name,name);
    strcpy(new->code,gender);
    for(;;){
        if( fscanf(party_data,"%s %s %s",name,id,gender) != 3){
            break;
        }
        Candidate candidate;
        candidate.name=name;
        candidate.id=id;
        if(*gender == 'F') candidate.gender=FEMININE;
        else candidate.gender=MASCULINE;
        if(listInsertLast(new->candidates,&candidate) != LIST_SUCCESS){
            destroyParty(new);
            fclose(party_data);
            return NULL;
        }
    }
    fclose(party_data);
    return new;
}



/*
Party createParty(char *party_data_file){
    if (!party_data_file) return NULL;
    FILE* party_data= fopen(party_data_file,"r");
    if (!party_data) return NULL;
    Party new= malloc(sizeof(*new));
    if (!new) {
        fclose(party_data);
        return NULL;
    }
    new->name=inputString(party_data);
    new->code=inputString(party_data);
    new->candidates=listCreate(candidateCopy,destroyCandidate);
    if (!new->candidates || !new->name || !new->code){
        free(new->name);
        free(new->code);
        fclose(party_data);
        free(new->candidates);
        return NULL;
    }
    char* name;
    while((name = inputString(party_data))){
        char* id = inputString(party_data);
        char* gender = inputString(party_data);
        if (!name || !id || !gender ) {
            free(name);
            free(id);
            free(gender);
            free(new->name);
            free(new->code);
            fclose(party_data);
            free(new->candidates);
            return NULL;
        }
        Candidate member;
        member.name=name;
        member.id=id;
        if (*gender == 'F') member.gender=FEMININE;
        else member.gender=MASCULINE;
        free(gender);
        listInsertLast((void*)new->candidates,(void*)&member);
        free(name);
        free(id);
    }
    return new;
}
*/

void destroyParty(Party party) {
    if (!party) return;
    free(party->name);
    free(party->code);
    listDestroy(party->candidates);
    free(party);
    return;
}

/**
 * NEEDS TO ADD CHECK IF MALLOC WAS SUCCESSFUL
 * @param party
 * @param name
 * @param id
 * @param gender
 * @param position
 * @return
 */
PartyResult addPerson(Party party, char *name, char *id, Gender gender,
                      int position){
    if (!party || !name || !id || position<=0 ) return PARTY_FAIL;
    if (!legalId(id)) return PARTY_FAIL;
    if (isMember(party,id)) return PARTY_FAIL;
    Candidate new;
    new.name=name;
    new.gender=gender;
    new.id=id;
    if(listGetSize(party->candidates)<= position){
        if(listInsertLast(party->candidates,&new) != LIST_SUCCESS)
        return PARTY_FAIL;
    }
    LIST_FOREACH(Candidatep,tmp,party->candidates){
        position--;
        if(position==0)
        if(listInsertBeforeCurrent(party->candidates,&new) != LIST_SUCCESS)
            return PARTY_FAIL;
    }
    return PARTY_SUCCESS;
}


PartyResult deletePerson(Party party, char *id){
    if (!party || !id) return PARTY_FAIL;
    if (!legalId(id)) return PARTY_FAIL;
    LIST_FOREACH(Candidatep,tmp,party->candidates){
        if (strcmp(tmp->id,id)==0){
            listRemoveCurrent(party->candidates);
            return PARTY_SUCCESS;
        }
    }
    return PARTY_FAIL;
}

bool isMember(Party party, char *id){
    assert(party);
    assert(id);
    if (!legalId(id)) return false;
    LIST_FOREACH(Candidatep,tmp,party->candidates){
        if(strcmp(tmp->id,id)==0) return true;
    }
    return false;
}

/**
 *  NEEDS FIXING IS THE ARRAY SORTED OR DO WE NEED TO SORT
 *  AND IS THE POSITOIN ARRAY CONNECTED TO THE LIST ORDER
 * @return
 */
PartyResult joinParties(Party* original_party_1, Party* original_party_2,
                        Party* outcome_party, int position_party_2[],
                        int n /*length of position_party_2*/,
                        char *new_name, char *new_code){
    assert(original_party_1 && original_party_2 && outcome_party
           && position_party_2 && new_code && new_name);
    if (n<=0 ) return PARTY_FAIL;
    if (haveCommonMembers(*original_party_1,*original_party_2))
        return PARTY_FAIL;
    Party new = malloc(sizeof(*new));
    if(!new) return PARTY_FAIL;
    new->name=malloc(sizeof(*new->name)*strlen(new_name)+1);
    new->code=malloc(sizeof(*new->code)*strlen(new_code)+1);
    new->candidates=listCopy((*original_party_1)->candidates);
    if(!new->candidates || !new->name || !new->code){
        free(new->name);
        free(new->code);
        free(new->candidates);
        free(new);
        return PARTY_FAIL;
    }
    strcpy(new->name,new_name);
    strcpy(new->code,new_code);
    int i=0;
    LIST_FOREACH(Candidatep,tmp,(*original_party_2)->candidates){
        if (i>=n) i=listGetSize(new->candidates);
        else i=position_party_2[i];
        if(addPerson(new,tmp->name,tmp->id,tmp->gender,i) != PARTY_SUCCESS){
            destroyParty(new);
            return PARTY_FAIL;
        }
        i++;
    }
    destroyParty(*original_party_1);
    destroyParty(*original_party_2);
    *outcome_party=new;
    return PARTY_SUCCESS;
}


PartyResult displayParty(Party party, int from_position, int to_position){
    assert(party);
    int position=1;
    printf("%s\n",party->name);
    printf("%s\n",party->code);
    char gender;
    LIST_FOREACH(Candidatep,tmp,party->candidates){
        if (position >= from_position && position <= to_position){
            if (tmp->gender==FEMININE) gender='F';
            else gender = 'M';
            printf("%s %s %c\n",tmp->name, tmp->id, gender);
        }
        position++;
    }
    return PARTY_SUCCESS;
}



PartyResult saveParty(Party party, char *party_data_file){
    assert(party && party_data_file);
    FILE* file=fopen(party_data_file,"w");
    if (!file) return PARTY_FAIL;
    fprintf(file,"%s\n",party->name);
    fprintf(file,"%s\n",party->code);
    char gender;
    LIST_FOREACH(Candidatep,tmp,party->candidates){
        if (tmp->gender==FEMININE) gender='F';
        else gender = 'M';
        fprintf(file,"%s %s %c\n",tmp->name,tmp->id,gender);
    }
    fclose(file);
    return PARTY_SUCCESS;

}


bool haveCommonMembers(Party party1, Party party2){
    assert(party1 && party2);
    LIST_FOREACH(Candidatep,tmp,party1->candidates){
        LIST_FOREACH(Candidatep,tmp2,party2->candidates){
            if (strcmp(tmp->id,tmp2->id)==0) return true;
        }
    }
    return false;
}


PartyResult getPartyDetails(Party party, char **party_name,
                            char **party_code, int *party_size){
    assert(party && party_code && party_name && party_size);
    int size,*ptr;
    char* name,*code;
    size=listGetSize(party->candidates);
    /*
    if(size==0) return PARTY_FAIL;
    ptr=malloc(sizeof(*ptr));
    *ptr=size;
     */
    name=malloc(sizeof(*name)*(strlen(party->name)+1));
    code=malloc(sizeof(*code)*(strlen(party->code)+1));
    if (!name || !code){
        free(name);
        free(code);
        return PARTY_FAIL;
    }
    strcpy(name,party->name);
    strcpy(code,party->code);
    *party_code=code;
    *party_name=name;
    *party_size=size;
    return PARTY_SUCCESS;
}





//
// Created by itke on 8/24/2019.
//
/*
int main (){
   // FILE* ptr= fopen("kulanu.txt", "r");
  //  printf("%c",*ptr);
//    FILE* file = fopen("Kulanu.txt","r");
    Party  new = createParty("Kulanu.txt");
    printf("%s",new->name);
    LIST_FOREACH(Candidatep,tmp,new->candidates){
        printf("%s ",tmp->name);
    }

    printf("\n");
    deletePerson(new,"222241322");
    addPerson(new,"dank","123456789",FEMININE,4);
    LIST_FOREACH(Candidatep,tmp,new->candidates){
        printf("%s ",tmp->name);
    }
    if (isMember(new,"123456789")) printf("\n MEOW \n");

    displayParty(new,3,5);
    saveParty(new,"dankparty.txt");

    char *name,*code;
    int *size;
    //size=malloc((sizeof(int)));
    getPartyDetails(new,&name,&code,&size);

    printf("MEOW %s %s %d MEOW",name ,code,*size);
    free(name);
    free(code);
    free(size);



    Party  second = createParty("yarok.txt");
    getPartyDetails(second,&name,&code,&size);

    printf("\n \n \nMEOW %s %s %d MEOW",name ,code,*size);


    free(name);
    free(code);
    free(size);


    displayParty(second,1,10);
    int array[]={1,3,5,6};
    Party next;
    joinParties(&new,&second,&next,array,4,"taub","cs");
    displayParty(next,1,20);


   // free(name);
   // free(code);
   // free(size);
    destroyParty(next);
    return 0;
}
*/