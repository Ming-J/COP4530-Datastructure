//Work on it with classmate Justin Block
//Look at codes from Sam github account:sskiswani
//and codes from Matthew  Husted  github account: matthusted
//Both of their code gave me a better understanding of what to do on this project
//
// To do X*Y
// if Y in biniary is 01010101
// basical principle X*(01010101)
// will be  X*(2^6+2^4+2^2+2^0)
// add the value to it's self is basically shifting the bit left one
// to keep a multiplier of X for 2x shift left 1
// 4x shift left 2, 
// 8x shift left 3 
// To find wether Y is 0 or 1 in a particular bit, you will have to use AND
// Two NAND after another is an AND Gate.
// So you just AND Y with 01 010 0100 01000 to figure out wether Y in a particular bit is zero or one 
// If the bit is one you will add the accounding multiplier of X to the product.
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define MAXLINELENGTH 1000
#define MAXNUMBEROFLINE 50

typedef struct labelnodes
{
  char label[1000];
  int address;
  struct labelnodes *next;
}labelnode;

typedef struct instructnodes
{
  int address;
  char opcode[1000];
  char arg0[1000];
  char arg1[1000];
  char arg2[1000];
}instructnode;

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
void addlabelnode(labelnode* root, char *label, int addressnumber);
int isVaildLabel(char* label);
void writeMachineCode(FILE* outFilePtr, labelnode* root, instructnode instruct[], int counter);
int convert(labelnode* root, instructnode cinstruct);

int rInstruction(int op, char* regA, char* regB, char* destReg);
int iInstruct(int op, char* regA, char* regB, int offset);
int checkBits(int bits, int max);
int getLabelAddress(labelnode* root, char* label);
int
main(int argc, char *argv[])
{
  char *inFileString, *outFileString;
  FILE *inFilePtr, *outFilePtr;
  char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
    arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

  if (argc != 3) {
    printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
	   argv[0]);
    exit(1);
  }

  inFileString = argv[1];
  outFileString = argv[2];

  inFilePtr = fopen(inFileString, "r");
  if (inFilePtr == NULL) {
    printf("error in opening %s\n", inFileString);
    exit(1);
  }
  outFilePtr = fopen(outFileString, "w");
  if (outFilePtr == NULL) {
    printf("error in opening %s\n", outFileString);
    exit(1);
  }

  /* here is an example for how to use readAndParse to read a line from
     inFilePtr */
  int addresscounter=0;
  labelnode *label_list;
  label_list = malloc( sizeof(labelnode) );
  label_list->next = 0;
  
  instructnode instruct[MAXNUMBEROFLINE];

  while ( readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ) {
    if(label[0]!='\0')
    {
    	addlabelnode(label_list, label, addresscounter);
	//printf("%s\n", label);
    }
    addresscounter=addresscounter+1;
  }
  rewind(inFilePtr);
  addresscounter=0;
  while ( readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) )
    {
    instruct[addresscounter].address=addresscounter;
    strcpy(instruct[addresscounter].opcode, opcode);
    strcpy(instruct[addresscounter].arg0, arg0);
    strcpy(instruct[addresscounter].arg1,arg1);
    strcpy(instruct[addresscounter].arg2,arg2);
    //    fprintf(outFilePtr, "%s %s %s %s %d %d\n", label,opcode, arg0, arg1, arg2, addresscounter);
    addresscounter=addresscounter+1;
    if(addresscounter > MAXNUMBEROFLINE)
      {
	fprintf(outFilePtr, "ERROR: Maximum program length exceeded. Maximum: %i \n"
		, MAXNUMBEROFLINE);
	exit(1);
      }
    /* reached end of file */
  }
  
  writeMachineCode(outFilePtr, label_list, instruct, addresscounter);
  
  /* this is how to rewind the file ptr so that you start reading from the
     beginning of the file */
  rewind(inFilePtr);

  /* after doing a readAndParse, you may want to do the following to test the
     opcode */
  //if (!strcmp(opcode, "add")) {
    /* do whatever you need to do for opcode "add" */
  //}

  return(0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int
readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
	     char *arg1, char *arg2)
{
  char line[MAXLINELENGTH];
  char *ptr = line;

  /* delete prior values */
  label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

  /* read the line from the assembly-language file */
  if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
    /* reached end of file */
    return(0);
  }

  /* check for line too long (by looking for a \n)() */
  if (strchr(line, '\n') == NULL) {
    /* line too long */
    printf("error: line too long\n");
    exit(1);
  }

  /* is there a label? */
  ptr = line;
  if (sscanf(ptr, "%[^\t\n ]", label)) {
    /* successfully read label; advance pointer over the label */
    ptr += strlen(label);
  }

  /*
   * Parse the rest of the line.  Would be nice to have real regular
   * expressions, but scanf will suffice.
   */
  sscanf(ptr, "%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]",
	 opcode, arg0, arg1, arg2);
  return(1);
}

int
isNumber(char *string)
{
  /* return 1 if string is a number */
  int i;
  return( (sscanf(string, "%d", &i)) == 1);
}

/*
 * Check to see if the label is a vail label
 * A vaild label contain maximum of 6 character, can consist letters and number
 * Must start with a letter
 * @parm label   string to check if it's a vaild label name
 * @return 0 if it's a invlid label, 1 if it is a vaild label 
*/
int isVaildLabel( char* label)
{
  if( strlen(label) > 6)
    {
      return 0;
    }
  else if( isalpha(label[0])==0)
    {
      return 0;
    }
  else
    {
      int i;
      for( i=0; i< strlen(label); i++)
	{
	  if(isalnum(label[i])==0)
	    {
	      return 0;
	    }
	  
	}
    }  
  return 1;
}

/*
 * Add a vaild label to label linked list
 * @param root root of the linked list
 * @param label the lable name 
 * @param addressnumber  the number of the address
*/


void addlabelnode(labelnode* root, char* label, int addressnumber)
{
  if( isVaildLabel(label) == 0)
    {
      fprintf(stderr, "ERROR: %s is not a vaild label. (See line %d)\n", label, addressnumber);
      exit(1);
    }

  labelnode* node = root;
  
  while( node->next!=0)
    {
     node = node->next;
    if(strcmp( node->label, label)==0)
  {
  
     fprintf(stderr, "ERROR: Dulicate label, %s, found.(see line  %d)\n", label, addressnumber);
    exit(1);
  }
  }
  
  node->next = malloc( sizeof(labelnode) );
  node = node->next;
  strcpy(node->label, label );
  node->address=addressnumber;
}

void writeMachineCode(FILE* outFilePtr, labelnode* root, instructnode instruct[],int counter )
{
  int i;
  for(i=0;i<counter;i++)
    {
      fprintf(outFilePtr, "%i\n", convert(root, instruct[i]));
    }
}


int convert(labelnode* root, instructnode cinstruct)
{
  if( strcmp ("add", cinstruct.opcode) ==0)
    {
      return rInstruction((0<<22), cinstruct.arg0, cinstruct.arg1, cinstruct.arg2 );
    }
  else if(strcmp("nand", cinstruct.opcode)==0)
    {
      return rInstruction((1<<22), cinstruct.arg0, cinstruct.arg1, cinstruct.arg2 );
    }
  else if(strcmp("lw",cinstruct.opcode)==0)
    {
      if(isalpha(cinstruct.arg2[0]) !=0&&isVaildLabel(cinstruct.arg2)!=1)
	{
	  fprintf(stderr, "ERROR: %s is not a vaild label. (See line %i)\n", cinstruct.arg2, cinstruct.address);
	  exit(1);
	}
      int offset;
	if(isVaildLabel(cinstruct.arg2))
	  {
	    offset=getLabelAddress(root, cinstruct.arg2);
	  }
	else
	  {
	    offset=checkBits( atoi(cinstruct.arg2), 16);
	  }
	return iInstruction((2<<22), cinstruct.arg0, cinstruct.arg1, offset);
    }
  else if(strcmp("sw", cinstruct.opcode)==0)
    {
      if(isalpha(cinstruct.arg2[0])!=0&&isVaildLabel(cinstruct.arg2)!=1)
	{
	  fprintf(stderr, "ERROR: %s is not a vaild label. (See line %i)\n", cinstruct.arg2, cinstruct.address);
	  exit(1);
	}
      int offset;
      if(isVaildLabel(cinstruct.arg2))
	{
	  offset=getLabelAddress(root, cinstruct.arg2);
	}
      else
	{
	  offset=checkBits(atoi(cinstruct.arg2), 16);
	}
      return iInstruction((3<<22), cinstruct.arg0, cinstruct.arg1, offset);
    }
  else if(strcmp("beq", cinstruct.opcode)==0)
    {
      if(isalpha(cinstruct.arg2[0])!=0&&isVaildLabel(cinstruct.arg2)!=1)
        {
          fprintf(stderr, "ERROR: %s is not a vaild label. (See line %i)\n", cinstruct.arg2, cinstruct.address);
          exit(1);
        }
      int offset;
      if(isVaildLabel(cinstruct.arg2))
        {
          offset=getLabelAddress(root, cinstruct.arg2)-cinstruct.address;
        }
      else
        {
          offset=checkBits(atoi(cinstruct.arg2), 16);
        }
      if(isVaildLabel(cinstruct.arg2)&&getLabelAddress(root, cinstruct.arg2)>cinstruct.address)
	{
	  --offset;
	}
      return iInstruction((4<<22),cinstruct.arg0, cinstruct.arg1, offset);
    }
  else if(strcmp("cmov", cinstruct.opcode)==0)
    {
      return rInstruction((5<<22), cinstruct.arg0, cinstruct.arg1, cinstruct.arg2);
    }
  else if(strcmp("halt", cinstruct.opcode)==0)
    {
      return (6<<22);
    }
  else if(strcmp("noop", cinstruct.opcode)==0)
    {
      return (7<<22);
    }
  else if(strcmp(".fill", cinstruct.opcode)==0)
    {
      if(isVaildLabel(cinstruct.arg0))
	{
	  //	  fprintf(stderr,"I am going to die: %s\n", cinstruct.arg1);
	  return getLabelAddress(root, cinstruct.arg0);
	}
      else
	{
	  return atoi(cinstruct.arg0);
	}
    }
  else
    {
      fprintf(stderr, "ERROR: Unrecognized opcode %s at address %i\n", cinstruct.opcode, cinstruct.address);
    }
}
/*
 *convert the parameter to bitstring for rtype instruction
 *@param op       bitshifted opcode
 *@param regA     registerA
 *@param regB     registerB
 *@param destReg   destination register
 */
int rInstruction(int op, char* regA, char* regB, char* destReg )
{
  int rA = checkBits( atoi(regA), 3);
  int rB = checkBits( atoi(regB), 3);
  int dR= checkBits( atoi(destReg), 3);
  return (op|(rA<<19)|(rB<<16)|dR);
}

int iInstruction(int op, char* regA, char* regB, int offset)
{
  int rA=checkBits(atoi(regA), 3);
  int rB=checkBits(atoi(regB), 3);
  checkBits(offset, 16);
  if(offset<0)
    {
      offset=0xFFFF+offset;
    }
  return  ( op | (rA<<19) | (rB<<16) | offset);
}

int getLabelAddress(labelnode* root, char* label)
{
  if(isVaildLabel(label)==0)
    {
      fprintf(stderr, "Error: %s is not a vaild label.\n", label);
      exit(1);
    }
  labelnode* node=root;
  while(node->next!=0)
    {
      node=node->next;
      if(strcmp(node->label, label)==0)
	{
	  return node->address;
	}
    }
  fprintf(stderr, "ERROR: Undeclared label %s. \n", label);
  exit(1);
}

/*
 *Chek to see if a number will be fit in a certain number of bit
 *@parm bits      number to check
 *@param max      the amount of bit the number are trying to fit
 */
int checkBits(int bits, int max)
{
  if( max ==3 && bits >7)
    {
      fprintf(stderr, "ERROR: value %d does nto fit in 3-bits (expected register value.)", bits);
      exit(1);
    }else if(max==16 &&(bits < -32768||bits>32767))
    {
      fprintf(stderr, "ERROR: Value %d does not fit in 16-bits (sxpected offset field)\n", bits);
      exit(1);
    }
    else{
      return bits;
    }
}
