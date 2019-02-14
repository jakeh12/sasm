#include <stdio.h>
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
    
    char* path = "/Users/jhladik/Documents/Development/sasm/program.sasm";
    
    // open file
    FILE* fp = fopen(path, "r");
    if (!fp) exit(EXIT_FAILURE);
    
    // figure out the size of the file
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    // load file in the memory and close file
    char *file = malloc(fsize + 1);
    fread(file, fsize, 1, fp);
    fclose(fp);
    
    // null terminate the file
    file[fsize] = '\0';
    
    line_t* lines = (line_t*) calloc(65535, sizeof(line_t));
    int line_count = 0;
    
    label_t* labels = (label_t*) calloc(65535, sizeof(line_t));
    int label_count = 0;
    
    int lc = 0;
    
    // ********************************
    // start process file
    // ********************************
    // process file line by line
    char* line = file;
    while (line)
    {
        // chop line with a null terminator
        char* next_line = strchr(line, '\n');
        if (next_line) *next_line = '\0';
        
        
        // ********************************
        // start process line
        // ********************************
        
        // remove comments
        char* cp = strchr(line, ';');
        if (cp) *cp = '\0';
        
        // skip empty or comment lines
        char* token;
        if (line[0] == '\0' || (token = strtok(line, " ,\t\n\v\f")) == NULL)
            goto skip_line;
        
        // check if the token is a label
        if(token[strlen(token)-1] == ':')
        {
            // remove colon and save it
            token[strlen(token)-1] = '\0';
            labels[label_count].addr = lc;
            labels[label_count].name = token;
            label_count++;
            
            // continue to next line
            token = strtok(NULL, " ,\t\n\v\f");
            if (!token)
                goto skip_line;
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
        
        // parse arguments
        int args_count = 0;
        token = strtok(NULL, " ,\t\n\v\f");
        while (token)
        {
            lines[line_count].args[args_count] = token;
            args_count++;
            token = strtok(NULL, " ,\t\n\v\f");
        }
        
        line_count++;
        // ********************************
        // end process line
        // ********************************
        
        
        // update line pointer
        skip_line: line = next_line ? next_line + 1 : NULL;
    }
    // ********************************
    // end process file
    // ********************************
    
    // ********************************
    // start assemble program
    // ********************************
    uint8_t program[65535];
    int pc = 0;
    int i;
    for (i = 0; i < line_count; i++)
    {
        if (strcmp(lines[i].dir, "sble") == 0)
        {
            // a address
            int addr = find_label_addr(lines[i].args[0], labels, label_count);
            if (addr == -1)
            {
                printf("ERROR: LABEL DOES NOT EXIST\n");
                exit(EXIT_FAILURE);
            }
            program[pc] = addr & 0xFF;
            pc++;
            program[pc] = (addr >> 8) & 0xFF;
            pc++;
            
            // b address
            addr = find_label_addr(lines[i].args[1], labels, label_count);
            if (addr == -1)
            {
                printf("ERROR: LABEL DOES NOT EXIST\n");
                exit(EXIT_FAILURE);
            }
            program[pc] = addr & 0xFF;
            pc++;
            program[pc] = (addr >> 8) & 0xFF;
            pc++;
            
            // c address
            if (lines[i].args[2])
            {
                addr = find_label_addr(lines[i].args[2], labels, label_count);
                if (addr == -1)
                {
                    printf("ERROR: LABEL DOES NOT EXIST\n");
                    exit(EXIT_FAILURE);
                }
                program[pc] = addr & 0xFF;
                pc++;
                program[pc] = (addr >> 8) & 0xFF;
                pc++;
            }
            else
            {
                program[pc] = (pc + 2) & 0xFF;
                pc++;
                program[pc] = ((pc + 2) >> 8) & 0xFF;
                pc++;
            }
        }
        else if (strcmp(lines[i].dir, "db") == 0)
        {
            // parse constants
            uint8_t c = (uint8_t) strtol(lines[i].args[0], NULL, 10);
            program[pc] = c;
            pc++;
        }
    }
    
    // ********************************
    // end assemble program
    // ********************************
    
    // print program
    for (i = 0; i < pc; i++)
    {
        printf("0x%04x:\t0x%02x\n", i, program[i]);
    }
    
    free(labels);
    free(lines);
    free(file);
    exit(EXIT_SUCCESS);
}
