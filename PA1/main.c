#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "hashtab.h"

char* ChangeFilename(char*, int);
void OpTabInit(OpNode**);
void SymTabRegInit(SymNode**);
void PassOne(char*);
void PassTwo(char*);

int main(int argc, char *argv[]){
  bool pass1 = false, pass2 = false;

  switch(argc){
    case 2:
      pass1 = true;
      pass2 = true;
      break;

    case 4:
      if(strcmp(argv[2], "-pass") == 0){
        if(strcmp(argv[3], "1") == 0) pass1 = true;
        else if(strcmp(argv[3], "2") == 0) pass2 = true;
        else{
          printf("Invalid execution!!!\n");
          return 0;
        }
        break;
      }

    default:
      printf("Invalid execution!!!\n");
      return 0;

  }

  if(pass1 == true) PassOne(argv[1]);
  if(pass2 == true) PassTwo(argv[1]);

  return 0;
}

void PassOne(char *path){
  printf("passone\n");
  FILE *fpi = fopen(path, "r");
  FILE *fpo = fopen(ChangeFilename(path, 1), "w");
  OpNode *optab[OPTABLEN] = {NULL, }, *op_info;
  SymNode *symtab[SYMTABLEN] = {NULL,}, *sym_info;
  unsigned int locctr = 0, sadr = 0;
  bool xadr = false;
  char name[30], buffer[30], sym[MYCHARLEN], op[MYCHARLEN], ta[MYCHARLEN], *xp;

  OpTabInit(optab);

  fscanf(fpi, "%s %s %d\n", name, op, &sadr);
  fprintf(fpo, "%s %s %x\n", name, op, sadr);
  printf("name: %s\n", name);

  while(1){
    memset(op, 0x00, MYCHARLEN);
    fgets(buffer, sizeof(buffer), fpi);
    if(buffer[0] == '\t'){ //statement doesn't have SYMBOL
      strcpy(sym, "$");
      strcpy(op, strtok(buffer+1, "\t"));
      strcpy(ta, strtok(NULL, "\n"));
      printf("%s %s\n", op, ta);
    }else if(buffer[0] == '\n'){
      continue;
    }else{// statement has SYMBOL
      strcpy(sym, strtok(buffer, "\t"));
      strcpy(op, strtok(NULL, "\t"));
      strcpy(ta, strtok(NULL, "\n"));
      printf("%s %s %s\n", sym, op, ta);
      if(SymSeek(symtab, sym) == NULL){
        SymAdd(symtab, sym, 0, locctr);
      }else{
        printf("ERR!!! Symbol overlapped\n");
        return;
      }
    }
    fprintf(fpo, "%.4x ", locctr);
    if(op[0] == '+') locctr += 4;
    else if(strcmp(op, "WORD") == 0){
      locctr += 3;
    }else if(strcmp(op, "RESW") == 0){
      locctr += 3*atoi(ta);
    }else if(strcmp(op, "RESB") == 0){
      locctr += atoi(ta);
    }else if(strcmp(op, "BYTE") == 0){
      locctr += strlen(ta)-3;
    }else if(strcmp(op, "BASE") == 0){
      break;
    }else if(strcmp(op, "END") == 0){
      printf("opEND");
      break;
    }else{
      op_info = OpSeek(optab, op);
      locctr += op_info->format;
    }
    xp = strstr(ta, ", X");
    if(xp != 0){
      xadr = true;
      memset(xp, 0x00, 3);
    }
    fprintf(fpo, "%s %s %s %x\n", sym, op, ta, xadr);
    xadr = false;
  }
  printf("END OF FILE\n");
  fprintf(fpo, "%s %s %s\n", sym, op, ta);

  SymNode *temp;
  fprintf(fpo, "\nSYMTAB\n");
  for(int i = 0; i < SYMTABLEN; i++){
    temp = symtab[i];
    while(temp!=NULL){
      fprintf(fpo, "%s %d %x\n", temp->symbol, temp->is_value_type, temp->value);
      temp = temp->next;
    }
  }

  fprintf(fpo, "\nPRGLEN %x\n", locctr-sadr);
  SymPrint(symtab, SYMTABLEN);
  OpPrint(optab, OPTABLEN);
  fclose(fpi);
  fclose(fpo);
  return;
}

void PassTwo(char *path){
  printf("pass two\n");
  FILE *fpi = fopen(path, "r");
  FILE *fpo = fopen(ChangeFilename(path, 2), "w");
  OpNode *optab[OPTABLEN] = {NULL, }, *op_info;
  SymNode *symtab[SYMTABLEN] = {NULL,}, *sym_info;
  char name[30], buffer[30], sym[MYCHARLEN], op[MYCHARLEN], ta[MYCHARLEN], *p_op, *p_ta;
  int objcode, objlen = 0, predisp;
  char text_record[35] = {'T'};
  int is_value_type, value, sadr, prglen = 0;
  int locctr, xflag, eflag, nflag, iflag, pflag, bflag, pc, b = 0;

//initialize

  OpTabInit(optab);
  SymTabRegInit(symtab);

  do{
    fgets(buffer, sizeof(buffer), fpi);
  }while(memcmp(buffer, "SYMTAB", 6) != 0);

  do{
    memset(sym, 0x00, MYCHARLEN);
    fscanf(fpi, "%s %x %x", sym, &is_value_type, &value);
    SymAdd(symtab, sym, is_value_type, value);
  }while(memcmp(sym, "PRGLEN", 6) != 0);
  prglen = is_value_type;
  fseek(fpi, 0, SEEK_SET);

//pass2 start
//header record
  fscanf(fpi, "%s %s %d\n", name, op, &sadr);
  fprintf(fpo, "%.2x", 'H');
  for(int i = 0; i < 6; i++) fprintf(fpo, "%.2x", name[i]);
  fprintf(fpo, "%.6x%.6x", sadr, prglen);
  fprintf(fpo, "\n");

//text record
  printf("fp: %p\n", fpi);
  do{
    memset(sym, 0x00, MYCHARLEN);
    memset(op, 0x00, MYCHARLEN);
    memset(ta, 0x00, MYCHARLEN);
    objcode = 0;
    fscanf(fpi, "%x %s %s %s %x", &locctr, sym, op, ta, &xflag);
    p_op = op;
    p_ta = ta;
    printf("%x %s %s %s\n", locctr, sym, op, ta);
    pc = locctr;
    if(strcmp(op, "WORD") == 0){
      pc += 3;
      objlen = strlen(ta);
      objcode = atoi(ta);
      fprintf(fpo, "%.6x\n", objcode);
    }else if(strcmp(op, "RESW") == 0){
      pc += 3*atoi(ta);
    }else if(strcmp(op, "RESB") == 0){
      pc += atoi(ta);
    }else if(strcmp(op, "BYTE") == 0){
      pc += strlen(ta)-3;
      objlen = strlen(ta)-3;
      p_ta = ta+2;
      p_ta[objlen] = '\0';
      objcode = atoi(p_ta);
      fprintf(fpo, "%.6x\n", objcode);
    }else if(strcmp(op, "END") == 0){
      break;
    }else{
      if(op[0] == '+'){
        eflag = 1<<4;
        p_op = op + 1;
        pc += 1;
      }
      printf("search for mnemonic %s\n", p_op);
      op_info = OpSeek(optab, p_op);
      pc += op_info->format;
      if(op_info->format == 1){
        objcode = op_info->opcode;
        fprintf(fpo, "%.2x\n", objcode);
      }else if(op_info->format == 2){
        objcode = op_info->opcode << 8;
        fprintf(fpo, "%.4x\n", objcode);
        //WIP
      }else{
        objlen = 3;
        if(ta[0] == '#'){
          iflag = 1;
          p_ta = ta+1;
        }else if(ta[0] == '@'){
          nflag = 1<<1;
          p_ta = ta+1;
        }
        sym_info = SymSeek(symtab, p_ta);
        predisp = sym_info->value;
        if(eflag != 0){
          objlen += 1;
          objcode = (op_info->opcode|nflag|iflag) << 24;
          objcode |= predisp;
          objcode |= eflag << 16;
          objcode |= xflag << 23;
          fprintf(fpo, "%.8x\n", objcode);
        }else{
          if((-2046 <= predisp - pc) || (predisp - pc <= 2047)){
            pflag = 1<<5;
          }else if((0 <= predisp - b) || (predisp - b <= 4095)){
            bflag = 1<<6;
          }else{
            printf("relative addressing ERR\n");
            return;
          }
          objcode |= (op_info->opcode|nflag|iflag) << 16;
          objcode |= predisp;
          objcode |= bflag<<8;
          objcode |= pflag<<8;
          objcode |= xflag << 15;
          fprintf(fpo, "%.6x\n", objcode);
        }

      }

    }
    eflag = 0;
    nflag = 0;
    iflag = 0;
    bflag = 0;
    pflag = 0;
    eflag = 0;
  }while(memcmp(op, "END", 3) != 0);

//end record
  fprintf(fpo, "%.2x", 'E');
  fprintf(fpo, "%.6x\n", sadr);
//modification record

  fclose(fpi);
  fclose(fpo);
  return;
}

char* ChangeFilename(char* origin, int pass){
  char *filename;
  int l = strlen(origin);
  switch(pass){

    case 1:
      filename = (char*)calloc(l+10, 1);
      memcpy(filename, origin, l-3);
      strcpy(filename+l-3, "intermediate");
      filename[l+10] = '\0';
      break;

    case 2:
      filename = (char*)calloc(l-10, 1);
      memcpy(filename, origin, l-12);
      strcpy(filename+l-12, "o");
      filename[l-10] = '\0';
      break;

    default:
      printf("invalid usage\n");

  }

  return filename;
}

void OpTabInit(OpNode** head){
  char mnemonic[MYCHARLEN] = {0x00, };
  int format, opcode;
  FILE *fp_optab = fopen("optab.txt", "r");
  while(!feof(fp_optab)){
    fscanf(fp_optab, "%s %x %x", mnemonic, &format, &opcode);
    OpAdd(head, mnemonic, format, opcode);
    memset(mnemonic, 0x00, MYCHARLEN);
  }
  fclose(fp_optab);
}

void SymTabRegInit(SymNode** head){
  SymAdd(head, "A\0\0\0\0\0\0", 0, 0);
  SymAdd(head, "X\0\0\0\0\0\0", 0, 1);
  SymAdd(head, "L\0\0\0\0\0\0", 0, 2);
  SymAdd(head, "B\0\0\0\0\0\0", 0, 3);
  SymAdd(head, "S\0\0\0\0\0\0", 0, 4);
  SymAdd(head, "T\0\0\0\0\0\0", 0, 5);
  SymAdd(head, "F\0\0\0\0\0\0", 0, 6);
  SymAdd(head, "PC\0\0\0\0\0", 0, 8);
  SymAdd(head, "SW\0\0\0\0\0", 0, 9);
}
