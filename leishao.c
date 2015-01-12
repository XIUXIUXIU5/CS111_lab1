//
//  main.c
//  test111_lab1a
//
//  Created by ShaoLei on 1/10/15.
//  Copyright (c) 2015 ShaoLei. All rights reserved.
//

#include "command.h"
#include "command-internals.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "alloc.h"

//#include <error.h>

/* FIXME: You may need to add #include directives, macro definitions,
 static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
 complete the incomplete type declaration in command.h.  */

typedef struct command_buffer{
    char ** head;
    struct command_buffer* next;
    
}command_buffer, *command_buffer_t;

int isComComand(char* a)
{
    if (isEqual(a, "if")|| isEqual(a, "fi")||isEqual(a, "then")||isEqual(a, "while") ||isEqual(a, "do") ||isEqual(a, "done") || isEqual(a, "until") || isEqual(a, "else")) {
        return 1;
    }
    else
        return 0;
}

int isWhiteSpace(char a)
{
    if ((a == ' ')|| (a == '\t')) {
        return 1;
    }
    else
        return 0;
}


int isEqual(char* a, char* b)
{
    return strcmp(a,b)?0:1;
}

int isWord (char c)
{
    if (isalpha(c) || isdigit(c) ||
        c == '!' ||c == '%' ||c == '+' ||c == ',' || c == '-' ||
        c == '.' ||c == '/' ||c == ':' ||c == '@' ||c == '^' || c == '_')
        return 1;
    else
        return 0;
}

int isSpecialToken(char c)
{
    if (c == ';' || c == '|'|| c == '('|| c == ')' || c == '<'|| c == '>' ) {
        return 1;
    }
    else
        return 0;
}

int isStartComComand(char *a)
{
    if (isEqual(a, "if")|| isEqual(a, "while") || isEqual(a, "until")) {
        return 1;
    }
    else
        return 0;

}

//TODO: implement # for comment
//      implement error checking

command_buffer_t make_command_buffer (char* buffer, size_t size){
    //for malloc
    size_t buffer_size = 256*sizeof(char *);
    size_t buffer_word_size = 36*sizeof(char);
    
    //for head index
    int row = 0;
    int col = 0;
    
    //number of compound command
    size_t num_if = 0;
    size_t num_fi = 0;
    size_t num_while = 0;
    size_t num_done = 0;
    size_t num_until = 0;
    size_t num_do = 0;
    size_t num_then = 0;
    size_t num_else = 0;
    
    //number of special token
    int leftParan = 0;
  //  size_t rightParan = 0;
  //  size_t lessThan = 0;
    //size_t greaterThan = 0;
    
    
    //alloc buffer
    command_buffer_t root = checked_malloc(sizeof(command_buffer));
    command_buffer_t curr = root;
    root->head = (char**) malloc (buffer_size);
    int i;
    for (i = 0; i < buffer_size; i++) {
        root->head[i] = (char*)malloc(buffer_word_size);
    }
    
    //buffer index
    int count = 0;
    
    //record the last newline or whiteSpace
    //int lastIsWhiteSpace = 0;
   // int lastIsNewLine = 0;
    
    
    //create
    while (count != size) {
        
        //if command_buffer start with '#'
        if (buffer[count] == '#') {
            if (row == 0 && col == 0) {
                while (buffer[count] != '\n') {
                    if (count != size ) {
                        count++;
                    }
                    else{
                        count--;
                        break;
                    }
                }
                count++;
            }
        }
        
        //read next word if it is a word
        else if (isWord(buffer[count])) {
            while (count != size && isWord(buffer[count])) {
                
                curr->head[row][col] = buffer[count];
                col++;
                count++;
                if (col == buffer_word_size) {
                    curr->head[row] = checked_grow_alloc(curr->head[row], &buffer_word_size);
                }
                
            }
            curr->head[row][col] = '\0';
            
            //start a new row
            row++;
            col = 0;
            
            //check the row size in case out of bound
            if (row == buffer_size){
                i = buffer_size;
                curr->head = checked_grow_alloc(curr->head, &buffer_size);
                for (; i < buffer_size; i++) {
                    root->head[i] = (char *)malloc(buffer_word_size);
                }
            }
            
            //replace the ; with \n
            if (isComComand(curr->head[row-1]) &&( (row-2 >= 0)||((row-3 >= 0)&& !isEqual(curr->head[row-3], "done"))  )&& isEqual(curr->head[row-2],";")) {
                curr->head[row-2][0] = '\n';
            }

            
            //check if the last word is if/fi/while/until/done
            if ((isEqual(curr->head[row-1],"if") && row == 1 )|| (isEqual(curr->head[row-1], "if") &&  ((curr->head[row-2][0]== '\n') || isComComand(curr->head[row-2]))))
                num_if++;
            
            else if ((isEqual(curr->head[row-1],"then") && row == 1 )|| (isEqual(curr->head[row-1], "then") &&  ((curr->head[row-2][0]== '\n') || isComComand(curr->head[row-2])))){
                num_then++;
                if (num_then > num_if) {
                    fprintf(stderr,"then can't come before if");
                }
                if (isEqual(curr->head[row-2], "if")) {
                    fprintf(stderr, "if statement is empty");
                }
            }
            
            //fixme: there is condition that else come before then but still smaller than then
            else if ((isEqual(curr->head[row-1],"else") && row == 1 )|| (isEqual(curr->head[row-1], "else") &&  ((curr->head[row-2][0]== '\n') || isComComand(curr->head[row-2])))) {               num_else++;
                if (num_else > num_then) {
                    fprintf(stderr,"else can't come before then");
                }
                if (isEqual(curr->head[row-2], "then")) {
                    fprintf(stderr, "then statement is empty");
                }
            }
            
            else if ((isEqual(curr->head[row-1],"fi") && row == 1 )|| (isEqual(curr->head[row-1], "fi") &&  ((curr->head[row-2][0]== '\n') || isComComand(curr->head[row-2])))){
                num_fi++;
                if (num_fi > num_then) {
                    fprintf(stderr, "fi can't come before then");
                }
                if (isEqual(curr->head[row-2], "else") || isEqual(curr->head[row-2], "then")) {
                    fprintf(stderr, "then/else statement can't be empty");
                }
            }
            
            else if ((isEqual(curr->head[row-1],"while") && row == 1 )|| (isEqual(curr->head[row-1], "while") &&  ((curr->head[row-2][0]== '\n') || isComComand(curr->head[row-2]) )))
                num_while++;
            
            else if ((isEqual(curr->head[row-1],"until") && row == 1 )|| (isEqual(curr->head[row-1], "until") &&  ((curr->head[row-2][0]== '\n') || isComComand(curr->head[row-2]))))
                num_until++;
            
            else if ((isEqual(curr->head[row-1],"do") && row == 1 )|| (isEqual(curr->head[row-1], "do") &&  ((curr->head[row-2][0]== '\n') || isComComand(curr->head[row-2]))))
            {    num_do++;
                if (num_do > num_while + num_until) {
                    fprintf(stderr, "do can't come before while or until");
                }
                if (isEqual(curr->head[row-2], "while")||isEqual(curr->head[row-2], "until")) {
                    fprintf(stderr, "while/until statement is empty");
                }
            }
            
            
            else if ((isEqual(curr->head[row-1],"done") && row == 1 )|| (isEqual(curr->head[row-1], "done") &&  ((curr->head[row-2][0]== '\n') || isComComand(curr->head[row-2])))){
                {    num_done++;
                    if (num_done > num_do) {
                        fprintf(stderr, "done can't come before do");
                    }
                    if (isEqual(curr->head[row-2], "do")) {
                        fprintf(stderr, "do statement can't be empty");
                    }
                }
                
           //fixme: next can't be compound token other than if while unitl
                if (!isSpecialToken(buffer[count])) {
                    //put a ; after down
                    if (row != buffer_size -1) {
                        curr->head[row][0] = ';';
                        curr->head[row][1] = '\0';
                        row++;
                    }
                    else
                    {
                        if (row == buffer_size){
                            i = buffer_size;
                            curr->head = checked_grow_alloc(curr->head, &buffer_size);
                            for (; i < buffer_size; i++) {
                                root->head[i] = (char *)malloc(buffer_word_size);
                            }
                        }
                        curr->head[row][0] = ';';
                        curr->head[row][1] = '\0';
                        row++;
                    }

                }
            }
            
            
        }
        
        
        //todo: error checking and corner cases
        //record the special token in an indivial row
        else if (isSpecialToken(buffer[count])) {
            
            //record the value
            
            curr->head[row][0] = buffer[count];
            
            if (buffer[count] == '(') {
                leftParan++;
            }
            
            else if (buffer[count] == ')') {
                leftParan--;
                if (isEqual(curr->head[row-1], "(")) {
                    fprintf(stderr, "subshell can't be empty");
                }
                if (leftParan < 0) {
                    fprintf(stderr, "no sufficient left paranthesis");
                }
            }
            
            else if (buffer[count] == ';'){
                if (row == 0) {
                    fprintf(stderr, "no valid command before ;");
                }
                
                //Question? can ; follow a new line??
                else if (isEqual(curr->head[row-1], ";")|| curr->head[row-1][0] == '\n' ) {
                    fprintf(stderr, "no valid command before ;");
                }
            }
            
            else if (buffer[count] == '>' || buffer[count] == '<')
            {
                if (row == 0) {
                    fprintf(stderr, "no valid command before > or <");
                }
                
                // fixme: can )( followed by > ?
                else if (isEqual(curr->head[row-1], ">") ||isEqual(curr->head[row-1], "<")||isEqual(curr->head[row-1], "\n"))
                {
                    fprintf(stderr, "invalid commad before > or <");
                }
            }
            
            else if (buffer[count] == '|'){
                
                //todo: implement the error checking for |
                if (row == 0) {
                    fprintf(stderr, "no valid command before |");
                }
                
                //Question? can | follow a new line?? and can there be more than one | ??
                else if (isEqual(curr->head[row-1], "|") || curr->head[row-1][0] == '\n' ) {
                    fprintf(stderr, "no valid command before |");
                }
                
                
                
            }
            
            row++;
            count++;
            
            if (buffer[count] == '(' || buffer[count] == ')') {
                if ((row - 2 != 0) && curr->head[row-2][0] == ';') {
                    curr->head[row-2][0] = '\n';
                }
            }
           
            //check the row size in case out of bound
            if (row == buffer_size) {
                if (row == buffer_size){
                    i = buffer_size;
                    curr->head = checked_grow_alloc(curr->head, &buffer_size);
                    for (; i < buffer_size; i++) {
                        root->head[i] = (char *)malloc(buffer_word_size);
                    }
                }
            }
        }
        
        else if (buffer[count] == '\n'){
            if (count < size -1) {
                
                //create a new command_buffer
                if (buffer[count+1] == '\n' && (num_if == num_fi) && (num_done == num_until+num_while)) {
                    
                    //replace the last new line with '\0'
                    if (curr->head[row-1][0] == '\n' || curr->head[row-1][0] == ';')
                        curr->head[row-1][0] = '\0';
                    
                    else
                        curr->head[row][0] = '\0';
                    
                    curr->next = checked_malloc(sizeof(command_buffer));
                    curr->next->head = (char**) malloc (buffer_size);
                    for (i = 0; i < buffer_size; i++) {
                        curr->next->head[i] = (char*)malloc(buffer_word_size);
                    }
                    curr = curr->next;
                    curr->next = NULL;
                    row = 0;
                    col = 0;
                    count++;
                    
                    num_while = num_until = num_then = num_if = num_fi = num_else = num_done = num_do = 0;
                   // count++;
                }
                
                //get rid of the comment
                else if (buffer[count+1] == '#')
                {
                    count++;
                    while (count != size && buffer[count] != '\n') {
                        count++;
                    }
                   // count++;
                }
                
                //todo: implement error case:
                else if(buffer[count + 1] == '>' || buffer[count + 1] == '<')
                    fprintf(stderr, "newline before > or < is not allowed");
                    
                else{
                    curr->head[row][0] = '\n';
                    curr->head[row][1] = '\0';
                   // count++;
                    row++;
                }
            }
            count++;
            
        }
            
        else if(isWhiteSpace(buffer[count])){
            count++;
            if (count < size -1 && (buffer[count+1] == '#')){
                while (count != size && buffer[count] != '\n') {
                    count++;
                }
            }
            //else
               // lastIsWhiteSpace = 1;
        }
        
        
        else{
            fprintf(stderr, "unknown char");
            count++;
        }
    }
    
    //check if the num of compound command is right
    if (!((num_if == num_fi) && (num_fi == num_then) && (num_then >= num_else) && (num_done == num_do) &&(num_do == num_until + num_while)))
    {
        fprintf(stderr, "incorrect number of compound command");
    }
    
    if (leftParan != 0) {
        fprintf(stderr, "incorrent number of paranthesis");
    }
    
    
    return root;
}



void print(command_buffer_t root)
{
    printf("\n");
    command_buffer_t curr;
    int i = 0,j = 0;
    curr = root;
    
    if (curr->head != NULL) {
        do {
            for (i = 0; (curr->head[i][0] != '\0'); i++) {
                for (j = 0; (curr->head[i][j] != '\0'); j++) {
                    printf("%c",curr->head[i][j]);
                }
                printf("\n");
            }
            printf("\n next buffer \n");
            curr = curr->next;
        } while (curr->next != NULL);
    }
    
}


static int
get_next_byte (void *stream)
{
    return getc (stream);
}


    command_stream_t
    make_command_stream (int (*get_next_byte) (void *),
                         void *get_next_byte_argument)
    {
        /* FIXME: Replace this with your implementation.  You may need to
         add auxiliary functions and otherwise modify the source code.
         You can also use external functions defined in the GNU C Library.  */
        size_t buffer_size = 1024;
        size_t count = 0;
        char *buffer = (char*) checked_malloc (buffer_size);
        int c = get_next_byte(get_next_byte_argument);
        while (c != EOF)
        {
            buffer[count] = c;
            count++;
            c = get_next_byte(get_next_byte_argument);
            if (count == buffer_size)
                buffer = checked_grow_alloc (buffer, &buffer_size);
        }
        
        command_buffer_t root = make_command_buffer(buffer, count);
        
        //this is just for testing
        print(root);
      //  error (1, 0, "command reading not yet implemented");
        return 0;
    }
    
    command_t
    read_command_stream (command_stream_t s)
    {
        /* FIXME: Replace this with your implementation too.  */
       // error (1, 0, "command reading not yet implemented");
        return 0;
    }

int main(){
    
    FILE *script_stream = fopen ("/Users/shaolei/Desktop/test111_lab1a/test111_lab1a/test.txt", "r");
    command_stream_t command_stream =
    make_command_stream (get_next_byte, script_stream);

}
