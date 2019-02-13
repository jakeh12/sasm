#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, const char * argv[]) {
    
    char* prog = "/Users/jhladik/Documents/Development/sasm/program.sasm";
    

    
    // open file
    FILE* fp = fopen(prog, "r");
    if (!fp) exit(EXIT_FAILURE);
    
    // read line by line
    char line[1000];
    char* token;
    char* args[100];
    int args_count;
    while(fgets(line, sizeof(line), fp))
    {
        
        // remove new line character
        //line[strlen(line)-1] = '\0';
        
        // remove comments
        char* cp = strchr(line, ';');
        if (cp) *cp = '\0';
        
        // skip empty or comment lines
        if (line[0] == '\0' || line[0] == ';' || (token = strtok(line, " ,\t\n\v\f")) == NULL)
            continue;
        
        // first token already parsed, use it
        if(token[strlen(token)-1] == ':')
        {
            // line has a label
            token[strlen(token)-1] = '\0';
            printf("LABEL %s\n", token);
            
            // TODO: add label to table
            
            token = strtok(NULL, " ,\t\n\v\f");
            if (!token) continue;
        }
        
        // parse directive
        printf("  DIRECTIVE %s ", token);
        
        // parse args
        args_count = 0;
        token = strtok(NULL, " ,\t\n\v\f");
        
        while (token)
        {
            args[args_count++] = token;
            token = strtok(NULL, " ,\t\n\v\f");
        }
        
        printf("(%d)\n", args_count);
        
        int i;
        for (i = 0; i < args_count; i++)
        {
            printf("    ARGUMENT %s\n", args[i]);
        }
        
    }
    
    fclose(fp);
    exit(EXIT_SUCCESS);
}
