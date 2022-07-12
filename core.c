#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <math.h>
#include <string.h>


#define REG_NUM 8

int imValue=0;
int dRegister=0;
int soperand=0;
int foperand=0;
int opcode=0;
int imValueFlag=0;
int BCC=0;
int BCCFlag=0;
int carry=0;
int verbose=0; 

int pc=0;/*program counter*/
long size;


uint64_t regs[REG_NUM];




enum {
    AND=0,  /*dest = op1 and op2*/
    ORR=1,  /*dest = op1 or op2*/
    EOR=2,  /*dest = op1 xor op2*/
    ADD=3,  /*dest = op1 + op2*/
    ADC=4,  /*dest = op1 + op2 + carry*/
    CMP=5,  
    SUB=6,  /*dest = op1 - op2*/
    SBC=7,  /*dest = op1 - op2 + carry - 1*/
    MOV=8,  /*dest = op2*/
    LSH=9,  /*dest = op1 << op2*/
    RSH=10  /*dest = op1 >> op2*/

};

enum {
    B=8,  /*Unconditional branch*/
    BEQ=9,  /*Branch if equal*/
    BNE=10,  /*Branch if not equal*/
    BLE=11,  /*Branch if lower or equal*/
    BGE=12,  /*Branch if greater or equal*/
    BL=13,  /*Branch if lower*/
    BG=14  /*Branch if greater*/
};

uint64_t hex2dec (char * tab) {
    uint64_t res;
    sscanf(tab, "%lX", &res);
    return res;
}

uint32_t littleEndianToBigEndian(uint32_t num){
    uint32_t b0,b1,b2,b3;
    uint32_t res;

    b0 = (num & 0x000000ff) << 24u;
    b1 = (num & 0x0000ff00) << 8u;
    b2 = (num & 0x00ff0000) >> 8u;
    b3 = (num & 0xff000000) >> 24u;

    return res = b0 | b1 | b2 | b3;

}


uint32_t* readfile(char* fichier) {

    FILE *bin = fopen(fichier, "rb");
    uint32_t* buffer;
    int i;

    /*On récupère la longueur du fichier*/
    
    fseek(bin, 0, SEEK_END);
    size = ftell(bin);
    rewind(bin);
    
    /*On peut maintenant lire le fichier*/
    buffer = (uint32_t*) malloc(size);
    fread(buffer, size, 1, bin);
    size = size/4;

    for (i=0; i<size; i++) {

        buffer[i] = littleEndianToBigEndian(buffer[i]);

    } 
    fclose(bin);
    return buffer;

}


void initRegisters (char* fichier){
    FILE *f = fopen(fichier, "r");
    char line[256];
    char val[8];
    uint64_t hex;
    int reg;
    int i;

    while(fgets(line, sizeof(line), f)){
        i=0;
        reg=line[1]-'0'; /*On recupere le numero de registre*/

        if (reg < 0 || reg >= REG_NUM) {
            printf("Numero de registre invalide \n");
            exit(EXIT_FAILURE);
        }

        /*On recupere la valeur hexa dans un tableau*/
        while ((line[i+3]!='\0')) {
            val[i]=line[i+3];
            i++;
        }

        hex = hex2dec(val); /*conversion en entier*/
        regs[reg]=hex;
    }
    fclose(f);
}





 uint32_t fetch(uint32_t* prog) {

    int offset;
    int signe;
    int temp;
    temp = pc;
    BCC = prog[pc] >> 28;

    if ((BCC == BEQ && (BCCFlag & 0x20)) || (BCC == BNE && (BCCFlag & 0x10)) || (BCC == BLE && (BCCFlag & 0x8)) || (BCC == BGE && (BCCFlag & 0x4)) || (BCC == BL && (BCCFlag & 0x2)) || (BCC == BG && (BCCFlag & 0x1)) || (BCC == B)) 
    {
        offset = prog[pc] & 0x7FFFFFF; /*offset*/
        signe = prog[pc]  & 0x8000000; /*bit 27 (offset positif ou negatif)*/
            
        if (signe) {
            pc = pc - offset;
        }
        else {
            pc = pc + offset;
        }
        
    }
    else
    {
        pc = pc+1;  
    }

    return prog[temp];   
}


void decode(uint32_t instr) {

    
    BCC = instr >> 28 ; /*BCC*/
    imValueFlag = (instr>> 24) & 0xF; /*imValueFlag*/
    opcode = (instr >> 20) & 0xF; /*opcode*/
    foperand  = (instr >> 16) & 0xF; /*first operand*/
    soperand  = (instr >> 12) & 0xF; /*soperand*/
    dRegister = (instr >> 8) & 0xF; /*destination register*/
    imValue = instr & 0xFF; /*imValue*/

}





void execute () {

    if (imValueFlag) {
        switch (opcode) {
            case AND:
                regs[dRegister] = regs[foperand] & imValue;
                if (verbose) 
                printf("AND r%d r%d %d \n", dRegister, foperand, imValue);
                break;

            case ORR:
                regs[dRegister] = regs[foperand] | imValue;
                if (verbose) 
                printf("ORR r%d r%d %d \n", dRegister, foperand, imValue);
                break;

            case EOR:
                regs[dRegister] = regs[foperand] ^ imValue;
                if (verbose) 
                printf("EOR r%d r%d %d \n", dRegister, foperand, imValue);
                break;

            case ADD:
                regs[dRegister] = regs[foperand] + imValue;
                if (regs[foperand] > (0xFFFFFFFFFFFFFFFF - imValue)) {
                    carry = 1;
                }
                if (verbose) 
                printf("ADD r%d r%d %d \n", dRegister, foperand, imValue);
                break;

            case ADC:
                regs[dRegister] = regs[foperand] + imValue + carry;
                if (regs[foperand] > (0xFFFFFFFFFFFFFFFF - imValue - carry)) {
                    carry = 1;
                }
                else {
                    carry = 0;
                }
                if (verbose) 
                printf("ADC r%d r%d %d \n", dRegister, foperand, imValue);
                break;

            case SUB:
                regs[dRegister] = regs[foperand] - imValue;
                if (regs[foperand] < imValue ) {
                    carry=1;
                }
                if (verbose) 
                printf("SUB r%d r%d %d \n", dRegister, foperand, imValue);
                break;

            case SBC:
                regs[dRegister] = regs[foperand] - imValue + carry - 1;
                if (regs[foperand] < (imValue - carry + 1)) {
                    carry=1;
                }
                else {
                    carry=0;
                }
                if (verbose) 
                printf("SBC r%d r%d r%d \n", dRegister, foperand, imValue);
                break;

            case MOV:
                regs[dRegister] = imValue;
                if (verbose) 
                printf("MOV r%d %d \n", dRegister, imValue);
                break;

            case LSH:
                regs[dRegister] = regs[foperand] << imValue;
                if (verbose) 
                printf("LSH r%d r%d %d \n", dRegister, foperand, imValue);
                break;

            case RSH:
                regs[dRegister] = regs[foperand] >> imValue;
                if (verbose) 
                printf("RSH r%d r%d %d \n", dRegister, foperand, imValue);
                break;

            case CMP:
                BCCFlag = (regs[foperand] == imValue) << 1; /*BEQ =*/
                BCCFlag = (BCCFlag + (regs[foperand] != imValue)) << 1; /*BNE !=*/
                BCCFlag = (BCCFlag + (regs[foperand] <= imValue)) << 1; /*BLE <=*/
                BCCFlag = (BCCFlag + (regs[foperand] >= imValue)) << 1; /*BGE >=*/
                BCCFlag = (BCCFlag + (regs[foperand] < imValue)) << 1; /*BL <*/
                BCCFlag = BCCFlag + (regs[foperand] > imValue); /*BG >*/
                if (verbose) 
                printf("CMP r%d %d \n", foperand, imValue);
                break;
        }

    }
    else {
        switch (opcode) {
            case AND:
                regs[dRegister] = regs[foperand] & regs[soperand];
                if (verbose) 
                printf("AND r%d r%d r%d \n", dRegister, foperand, soperand);
                break;

            case ORR:
                regs[dRegister] = regs[foperand] | regs[soperand];
                if (verbose) 
                printf("ORR r%d r%d r%d \n", dRegister, foperand, soperand);
                break;

            case EOR:
                regs[dRegister] = regs[foperand] ^ regs[soperand];
                if (verbose) 
                printf("EOR r%d r%d r%d \n", dRegister, foperand, soperand);
                break;

            case ADD:
                regs[dRegister] = regs[foperand] + regs[soperand];
                if (regs[foperand] > (0xFFFFFFFFFFFFFFFF - regs[soperand])) {
                    carry = 1;
                }
                if (verbose) 
                printf("ADD r%d r%d r%d \n", dRegister, foperand, soperand);
                break;

            case ADC:
                regs[dRegister] = regs[foperand] + regs[soperand] + carry;
                if (regs[foperand] > (0xFFFFFFFFFFFFFFFF - regs[soperand] - carry)) {
                    carry = 1;
                }
                else {
                    carry = 0;
                }
                if (verbose) 
                printf("ADC r%d r%d r%d \n", dRegister, foperand, soperand);
                break;

            case SUB:
                regs[dRegister] = regs[foperand] - regs[soperand];
                if (regs[foperand] < regs[soperand]) {
                    carry=1;
                }
                if (verbose) 
                printf("SUB r%d r%d r%d \n", dRegister, foperand, soperand);
                break;

            case SBC:
                regs[dRegister] = regs[foperand] - regs[soperand] + carry - 1;
                if (regs[foperand] < (regs[soperand] - carry + 1)) {
                    carry=1;
                }
                else {
                    carry=0;
                }
                if (verbose) 
                printf("SBC r%d r%d r%d \n", dRegister, foperand, soperand);
                break;

            case MOV:
                regs[dRegister] = regs[soperand];
                if (verbose) 
                printf("MOV r%d r%d \n", dRegister, soperand);
                break;

            case LSH:
                regs[dRegister] = regs[foperand] << regs[soperand];
                if (verbose) 
                printf("LSH r%d r%d r%d \n", dRegister, foperand, soperand);
                break;

            case RSH:
                regs[dRegister] = regs[foperand] >> regs[soperand];
                if (verbose) 
                printf("RSH r%d r%d r%d \n", dRegister, foperand, soperand);
                break;

            case CMP:
                BCCFlag = (regs[foperand] == regs[soperand]) << 1; /*BEQ*/
                BCCFlag = (BCCFlag + (regs[foperand] != regs[soperand])) << 1; /*BNE*/
                BCCFlag = (BCCFlag + (regs[foperand] <= regs[soperand])) << 1; /*BLE*/
                BCCFlag = (BCCFlag + (regs[foperand] >= regs[soperand])) << 1; /*BGE*/
                BCCFlag = (BCCFlag + (regs[foperand] < regs[soperand])) << 1; /*BL*/
                BCCFlag = BCCFlag + (regs[foperand] > regs[soperand]); /*BG*/
                if (verbose) 
                printf("CMP r%d r%d \n", foperand, soperand);
                break;
        }

    }
    
}


void printRegisters() {
    int i;
    for (i=0; i<REG_NUM; i++ ){
        printf("Registre %d : %016lX \n", i, regs[i]);
    }

}




int main(int argc, char *argv[]){

    uint32_t instruction;
    uint32_t* prog;

    if (argc > 2) {
        
        if ((argc == 4) && ((argv[3][0] == '0') || (argv[3][0] == '1'))) {
            verbose = argv[3][0] - '0';
        }

        
        prog = readfile(argv[1]);
        initRegisters(argv[2]);

        printRegisters();
        while (pc<size) {
            printf("\n %d \n", pc);
            instruction = fetch(prog);
            printf("instruction = %08x \n",instruction);
            if(BCC != B && BCC != BEQ && BCC != BNE && BCC != BLE && BCC != BGE && BCC != BL && BCC != BG) {
                decode(instruction);
                execute();
                printRegisters();
            }
        }
    }
    else {
        printf("Il faut au moins 2 arguments");
    }
    
    return 0;
}