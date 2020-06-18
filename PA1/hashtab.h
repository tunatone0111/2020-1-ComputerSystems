#ifndef HASHTAB
#define HASHTAB

#define OPTABLEN 60
#define SYMTABLEN 20
#define MYCHARLEN 20

typedef struct op_node_struct{
  char mnemonic[MYCHARLEN];
  int format;
  int opcode;
  struct op_node_struct *next;
} OpNode;

typedef struct sym_node_struct{
  char symbol[MYCHARLEN];
  int is_value_type;
  int value;
  struct sym_node_struct *next;
} SymNode;

void OpAdd(OpNode**, char*, int, int);
void SymAdd(SymNode**, char*, int, int);

OpNode* OpSeek(OpNode**, char*);
SymNode* SymSeek(SymNode**, char*);

void OpPrint(OpNode**, int);
void SymPrint(SymNode**, int);

#endif
