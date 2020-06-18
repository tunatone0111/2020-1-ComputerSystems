#include "hashtab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int HashFunc(char string[], int tablen){
  int hashnum = 0;
  // printf("hash(%s)\t", string);
  for(int i = 0; i < MYCHARLEN; i++)
    hashnum += (int)string[i];
  hashnum %= tablen;
  if(hashnum<0) hashnum *= -1;
  // printf("hashnum: %d\n", hashnum);
  return hashnum;
}

void OpAdd(OpNode** head, char mnemonic[], int format, int opcode){
  OpNode *newnode = (OpNode*)calloc(1, sizeof(OpNode));
  memcpy(newnode, mnemonic, 7);
  newnode->format = format;
  newnode->opcode = opcode;
  newnode->next = NULL;

  int hashnum = HashFunc(mnemonic, OPTABLEN);
  OpNode *temp;
  if(head[hashnum] == NULL){
    head[hashnum] = newnode;
  }else{
    temp = head[hashnum];
    while(temp->next != NULL)
      temp = temp->next;
    temp->next = newnode;
  }
}

void SymAdd(SymNode** head, char symbol[], int is_value_type, int value){
  SymNode *newnode = (SymNode*)calloc(1, sizeof(SymNode));
  memcpy(newnode, symbol, MYCHARLEN);
  newnode->is_value_type = is_value_type;
  newnode->value = value;
  newnode->next = NULL;

  int hashnum = HashFunc(symbol, SYMTABLEN);
  SymNode *temp;
  if(head[hashnum] == NULL){
    head[hashnum] = newnode;
  }else{
    temp = head[hashnum];
    while(temp->next != NULL)
      temp = temp->next;
    temp->next = newnode;
  }
}

OpNode* OpSeek(OpNode** head, char mnemonic[]){
  OpNode *temp = head[HashFunc(mnemonic, OPTABLEN)];
  if(temp == NULL){
    printf("ERR!!! optab empty\n");
    return NULL;
  }
  while(strcmp(temp->mnemonic, mnemonic) != 0){
    if(temp->next == NULL){
      printf("ERR!!! opcode doesn't exist\n");
      return NULL;
    }
    temp = temp->next;
  }
  return temp;
}

SymNode* SymSeek(SymNode** head, char symbol[]){
  SymNode *temp = head[HashFunc(symbol, SYMTABLEN)];
  if(temp == NULL) return NULL;
  while(strcmp(temp->symbol, symbol) != 0){
    if(temp->next == NULL) return NULL;
    temp = temp->next;
  }
  return temp;
}

void OpPrint(OpNode** head, int n){
  OpNode *temp;
  for(int i = 0; i < n; i++){
    temp = head[i];
    while(temp != NULL){
      printf("%d\tkey: %s, value: %x  \t", i, temp->mnemonic, temp->opcode);
      temp = temp->next;
    }
    printf("\n");
  }
}

void SymPrint(SymNode** head, int n){
  SymNode *temp;
  for(int i = 0; i < n; i++){
    temp = head[i];
    while(temp != NULL){
      printf("%d\tkey: %s  \tvalue: %x  \t", i, temp->symbol, temp->value);
      temp = temp->next;
    }
    printf("\n");
  }
}
