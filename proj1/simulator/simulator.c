#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

typedef struct stateStruct {
  int pc;
  int mem[NUMMEMORY];
  int reg[NUMREGS];
  int numMemory;
} stateType;

void printState(stateType *);
inline void iarg(int bit, int* arg );
inline void rarg(int bit, int* arg );


int
main(int argc, char *argv[])
{
  FILE *filePtr;
  if (argc != 2) {
    printf("error: usage: %s <machine-code file>\n", argv[0]);
    exit(1);
  }

  filePtr = fopen(argv[1], "r");
  if (filePtr == NULL) {
    printf("error: can't open file %s", argv[1]);
    perror("fopen");
    exit(1);
  }

  stateType state;
  char line[MAXLINELENGTH];
  unsigned int i;

  for(i=0; i< NUMREGS; i++)
    {
      state.reg[i]=0;
    }
  /* read in the entire machine-code file into memory */
  for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
       state.numMemory++) {
    if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
      printf("error in reading address %d\n", state.numMemory);
      exit(1);
    }
    printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
  }
  printf("\n");

  state.pc = 0;
  unsigned int total =0;
  int op_args[3];
  int rA, rB, offset;

  for(i=1; i!=0; i++)
    {
      total++;
      printState(&state);
  switch (state.mem[state.pc]>>22)
    {
      //ADD  
    case 0:
    rarg(state.mem[state.pc], op_args);
    rA=state.reg[ op_args[0]];
    rB=state.reg[ op_args[1]];
    state.reg[ op_args[2] ] = rA+rB;
    break;
    //NAND
    case 1:
    rarg(state.mem[state.pc], op_args);
    rA=state.reg[ op_args[0]];
    rB=state.reg[ op_args[1]];
    state.reg[ op_args[2] ]= ~(rA&rB);
    break;
    //LW
    case 2:
      iarg(state.mem[state.pc], op_args);
      offset = op_args[2]+state.reg[ op_args[0] ];
      if(offset<0||offset >= state.numMemory )
	{
	  fprintf( stderr, "ERROR: Invaild memory address: %d (out of range).\n", offset);
	  offset;
	}
      state.reg[ op_args[1] ] = state.mem[ offset ];
      break;
      //SW
    case 3:
      iarg(state.mem[state.pc], op_args);
      offset=op_args[2]+state.reg[ op_args[0] ];
      if(offset <0 || offset >= state.numMemory)
	{
	  fprintf(stderr,"ERROR: Invaild memory address: %d (out of range).\n", offset);
	  exit(1);
	}
      state.mem[ offset ]= state.reg[ op_args[1] ];
      break;
      //BEQ
    case 4:
    iarg(state.mem[state.pc], op_args);
    rA=state.reg[ op_args[0]];
    rB=state.reg[ op_args[1]];
    if(rA==rB)
      {
	state.pc=state.pc+op_args[2];
      }
  break;
 case 5:  //CMOV
   rarg(state.mem[state.pc], op_args);
   rA=state.reg[ op_args[0] ];
   rB=state.reg[ op_args[1] ];

   if(rB!=0)
     {
       state.reg[ op_args[2] ]=rA;
     }
   break;
   //HALT
 case 6:
   i=-1;
   break;
 case 7: // NOOP
   break;

 default:
   fprintf(stderr, "ERROR: Memory at location %i illegible.\n", state.pc);
   exit(1);
   break;
    }
  state.pc++;
  if(state.pc>state.numMemory|| state.pc<0)
    {
      fprintf(stderr, "ERROR: PC is out of range [0, %d). PC value: %d\n", state.numMemory, state.pc);
      exit(1);
    }
    }
  printf("end state\n");
  printf("machine halted\n");
  printf("total of %i instructions executed\n", total);
  printf("final state of machine:\n");
  printState(&state);
  return(0);
}

void
printState(stateType *statePtr)
{
  int i;
  printf("\n@@@\nstate:\n");
  printf("\tpc %d\n", statePtr->pc);
  printf("\tmemory:\n");
  for (i=0; i<statePtr->numMemory; i++) {
    printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
  }
  printf("\tregisters:\n");
  for (i=0; i<NUMREGS; i++) {
    printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
  }
  printf("end state\n");
}


inline void iarg(int bits, int* arg)
{
  arg[0] = ( (bits & (7<<19) )>> 19 );
  arg[1] = ( (bits & (7<<16) )>> 16 );
  arg[2] = ( bits & (0xFFFF) );
  if(arg[2] & (1<<15) )
    {
      arg[2] = arg[2] - (1<<16);
    }
  if( arg[0]<0 || arg[1]<0 )
    {
      fprintf(stderr,"ERROR: Negative regative value encountered.\n");
      exit(1);
    }
  else if( arg[0] >= NUMREGS || arg[1] >= NUMREGS )
    {
      fprintf( stderr,"ERROR: Register value greater than %d encountered.\n",NUMREGS);
      exit(1);
    }
}

inline void rarg(int bits, int* arg)
{
  arg[0] = ( (bits & (7<<19) )>> 19 );
  arg[1] = ( (bits& (7<<16)  )>> 16 );
  arg[2] = ( bits& 7);
  
  if( arg[0]<0||arg[1]<0||arg[2]<0)
    {
      fprintf(stderr, "ERROR: NEgative register value encountered.\n");
      exit(1);
    }
  else if(arg[0] >= NUMREGS || arg[1] >= NUMREGS || arg[2]>= NUMREGS)
    {
      fprintf( stderr, "ERROR: Register value greater than %d encountered.\n", NUMREGS);
      exit(1);
    }
}
