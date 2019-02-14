#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    int addr;
    char* dir;
    char* args[3];
}
line_t;

typedef struct
{
    int addr;
    char* name;
}
label_t;


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
    int arg_count = 0;
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
            labels[label_count].addr = lc;
            labels[label_count].name = token;
            label_count++;
            token = strtok(NULL, " ,\t\n\v\f");
            if (!token) continue;
        }
        
        lines[line_count].addr = lc;
        lines[line_count].dir = token;
        
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
        
        // parse args
        arg_count = 0;
        token = strtok(NULL, " ,\t\n\v\f");
        
        while (token)
        {
            lines[line_count].args[arg_count] = token;
            arg_count++;
            token = strtok(NULL, " ,\t\n\v\f");
        }
        line_count++;
    }
    
    int i;
    for (i = 0; i < line_count; i++)
    {
        
        if (strcmp(lines[i].dir, "sble") == 0)
        {
            int a_addr = find_label_addr(lines[i].args[0], labels, label_count);
            if (a_addr == -1)
            {
                printf("ERROR: LABEL DOES NOT EXIST\n");
                exit(EXIT_FAILURE);
            }
            program[pc] = a_addr & 0xFF;
            pc++;
            program[pc] = (a_addr >> 8) & 0xFF;
            pc++;
            
            int b_addr = find_label_addr(lines[i].args[1], labels, label_count);
            if (b_addr == -1)
            {
                printf("ERROR: LABEL DOES NOT EXIST\n");
                exit(EXIT_FAILURE);
            }
            program[pc] = b_addr & 0xFF;
            pc++;
            program[pc] = (b_addr >> 8) & 0xFF;
            pc++;
            
            int c_addr;
            if (lines[i].args[2])
            {
                c_addr = find_label_addr(lines[i].args[0], labels, label_count);
                if (c_addr == -1)
                {
                    printf("ERROR: LABEL DOES NOT EXIST\n");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                c_addr = pc + 2;
            }
            program[pc] = c_addr & 0xFF;
            pc++;
            program[pc] = (c_addr >> 8) & 0xFF;
            pc++;
        }
        else if (strcmp(lines[i].dir, "db") == 0)
        {
            // parse constants
            program[pc] = (uint8_t) strtol(lines[i].args[0], NULL, 10);;
            pc++;
        }
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
