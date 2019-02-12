#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LABELS 65535
#define MAX_INST 65535

typedef struct {
    int address;
    char name[32];
} label_t;


typedef struct {
    int address;
    char a[32];
    char b[32];
    char c[32];
} inst_t;

int find_label(char* label, label_t* labels, int label_count)
{
    int i;
    for (i = 0; i < label_count; i++)
    {
        if (strcmp(labels[i].name, label) == 0)
            return labels[i].address;
    }
    return -1;
}

int main(int argc, const char * argv[]) {
    
    char* prog = "/Users/jhladik/Documents/Development/sasm/program.sasm";
    
    // label table
    label_t* labels = (label_t*) malloc(MAX_LABELS*sizeof(label_t));
    int label_count = 0;
    
    // program table
    inst_t* insts = (inst_t*) malloc(MAX_INST*sizeof(inst_t));
    int inst_count = 0;
    
    uint8_t program[MAX_INST];
    int program_byte_count = 0;
    
    // open file
    FILE* fp = fopen(prog, "r");
    if (!fp) exit(EXIT_FAILURE);
    
    // read line by line
    char line[1000];
    while(fgets(line, sizeof(line), fp))
    {
        // remove new line character
        line[strlen(line)-1] = '\0';
        
        // skip empty lines or comment lines
        if (line[0] == ';' || line[0] == '\n' || line[0] == '\0')
            continue;
        
        // check for label
        if (line[strlen(line)-1] == ':')
        {
            // get rid off the colon
            line[strlen(line)-1] = '\0';
            
            labels[label_count].address = inst_count;
            strcpy(labels[label_count].name, line);
            label_count++;
            // TODO: PUT LABEL IN THE TABLE
            printf("LABEL: %s @ 0x%04X\n", line, inst_count);
        }
        else
        {
            inst_count++;
        }
    }
    
    printf("found %d labels\n", label_count);
    
    // second pass
    inst_count = 0;
    fseek(fp, 0, SEEK_SET);
    while(fgets(line, sizeof(line), fp))
    {
        // remove new line character
        line[strlen(line)-1] = '\0';
        
        if (line[0]              == ';'  ||
            line[0]              == '\n' ||
            line[0]              == '\0' ||
            line[strlen(line)-1] == ':')
        {
            continue;
        }
        else
        {
            // TODO: ASSEMBLE LINE
            printf("ASSEMBLING: %s @ 0x%04X\n", line, inst_count);
            char* token = strtok(line, " ,\t\n\v\f");
            
            if (strcmp(token, "sble") == 0)
            {
                int label_addr;
                
                // pointer a
                token = strtok(NULL, " ,\t\n\v\f");
                label_addr = 6*find_label(token, labels, label_count);
                program[program_byte_count] = label_addr & 0xFF;
                program[program_byte_count+1] = (label_addr >> 8) & 0xFF;
                program_byte_count += 2;
                
                // pointer b
                token = strtok(NULL, " ,\t\n\v\f");
                label_addr = 6*find_label(token, labels, label_count);
                program[program_byte_count] = label_addr & 0xFF;
                program[program_byte_count+1] = (label_addr >> 8) & 0xFF;
                program_byte_count += 2;
                
                // pointer c
                token = strtok(NULL, " ,\t\n\v\f");
                if (token)
                {
                    label_addr = 6*find_label(token, labels, label_count);
                    program[program_byte_count] = label_addr & 0xFF;
                    program[program_byte_count+1] = (label_addr >> 8) & 0xFF;
                    
                }
                else
                {
                    // c missing, use next instruction
                    //program[program_byte_count] = inst_count + 1;
                    program[program_byte_count] = 6*inst_count & 0xFF;
                    program[program_byte_count+1] = (6*inst_count >> 8) & 0xFF;
                }
                program_byte_count += 2;
                inst_count++;
            }
            else if (strcmp(token, "db") == 0)
            {
                // data byte
                token = strtok(NULL, " ,\t\n\v\f");
                uint8_t value = (uint8_t) strtol(token, NULL, 10);
                program[program_byte_count] = value;
                program_byte_count++;
                //inst_count++;
            }
            else
            {
                printf("error parsing token\n");
                exit(EXIT_FAILURE);
            }
        }
        
    }
    
    int i;
    for (i = 0; i < program_byte_count; i++)
    {
        printf("%04x: %02x\n", i, program[i]);
    }
    printf("\n");
    
    free(labels);
    free(insts);
    
    fclose(fp);
    exit(EXIT_SUCCESS);
}
