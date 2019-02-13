#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    int addr;
    char dir[32];
    char* args[100];
    int args_count;
} line_t;

typedef struct
{
    int addr;
    char name[32];
} label_t;


int find_label_addr(char* name, label_t* labels, int label_count)
{
    int i;
    for (i = 0; i < label_count; i++)
    {
        if (strcmp(labels[i].name, name) == 0)
            return labels[i].addr;
    }
    return -1;
}

int main(int argc, const char * argv[]) {
    
    char* prog = "/Users/jhladik/Documents/Development/sasm/program.sasm";
    
    // open file
    FILE* fp = fopen(prog, "r");
    if (!fp) exit(EXIT_FAILURE);
    
    line_t* lines = (line_t*) calloc(65535, sizeof(line_t));
    int line_count = 0;
    
    label_t* labels = (label_t*) calloc(65535, sizeof(line_t));
    int label_count = 0;
    
    uint8_t program[65535];
    int pc = 0;
    
    // read line by line
    char line[1000];
    char* token;
    char* args[100];
    int args_count;
    int lc = 0;
    while(fgets(line, sizeof(line), fp))
    {
        
        // remove comments
        char* cp = strchr(line, ';');
        if (cp) *cp = '\0';
        
        // skip empty or comment lines
        if (line[0] == '\0' || (token = strtok(line, " ,\t\n\v\f")) == NULL)
            continue;
        
        // first token already parsed, use it
        if(token[strlen(token)-1] == ':')
        {
            // line has a label
            token[strlen(token)-1] = '\0';
            //printf("LABEL %s @ 0x%04x\n", token, lc);
            
            // TODO: add label to table
            labels[label_count].addr = lc;
            strcpy(labels[label_count].name, token);
            label_count++;
            
            token = strtok(NULL, " ,\t\n\v\f");
            if (!token) continue;
        }
        
        lines[line_count].addr = lc;
        strcpy(lines[line_count].dir, token);
        
        // parse directive
        if (strcmp(token, "sble") == 0)
        {
            lc += 6;
        }
        else if (strcmp(token, "db") == 0)
        {
            lc += 1;
        }
        else
        {
            printf("ERROR: UNKNOWN DIRECTIVE\n");
            exit(EXIT_FAILURE);
        }
        //printf("  DIRECTIVE %s ", token);
        
        // parse args
        args_count = 0;
        token = strtok(NULL, " ,\t\n\v\f");
        
        while (token)
        {
            char* arg = (char*) calloc(32, sizeof(char));
            strcpy(arg, token);
            lines[line_count].args[args_count] = arg;
            args[args_count++] = token;
            token = strtok(NULL, " ,\t\n\v\f");
        }
        
        lines[line_count].args_count = args_count;
        
        //printf("(%d)\n", args_count);
        
        int i;
        for (i = 0; i < args_count; i++)
        {
            
            //printf("    ARGUMENT %s\n", args[i]);
        }
        
        line_count++;
        
    }
    
    
    printf("listing:\n");
    
    
    int i;
    for (i = 0; i < line_count; i++)
    {
        
        if (strcmp(lines[i].dir, "sble") == 0)
        {
            printf("0x%04x\t%s\t", lines[i].addr, lines[i].dir);
            int j;
            for (j = 0; j < lines[i].args_count; j++)
            {
                int addr = find_label_addr(lines[i].args[j], labels, label_count);
                if (addr == -1)
                {
                    printf("ERROR: LABEL DOES NOT EXIST\n");
                    exit(EXIT_FAILURE);
                }
                
                program[pc] = addr & 0xFF;
                pc++;
                program[pc] = (addr >> 8) & 0xFF;
                pc++;
                
                printf("%s\t", lines[i].args[j]);
            }
            printf("\n");
        }
        else if (strcmp(lines[i].dir, "db") == 0)
        {
            // parse constants
            uint8_t c = (uint8_t) strtol(lines[i].args[0], NULL, 10);
            program[pc] = c;
            pc++;
            
            
            
            printf("0x%04x\t%s\t", lines[i].addr, lines[i].dir);
            int j;
            for (j = 0; j < lines[i].args_count; j++)
            {
                printf("%s\t", lines[i].args[j]);
            }
            printf("\n");
        }
    }
    
    printf("\nlables:\n");

    for (i = 0; i < label_count; i++)
    {
        printf("0x%04x\t%s\n", labels[i].addr, labels[i].name);
    }

    printf("\ntranslated:\n");
    for (i = 0; i < pc; i++)
    {
        printf("0x%04x: 0x%02x\n", i, program[i]);
    }
    
    free(lines);
    fclose(fp);
    exit(EXIT_SUCCESS);
}
