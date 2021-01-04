#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <setjmp.h>

#define filesize 15

char cwd[100];
int pid;
int dup_op;
int last_read;
int ind = -1;
static sigjmp_buf env;
int killp = 0;
int mult = 0;
unsigned char isChild =0;

void run_cd(char* cmd);
int check_semicolon(char* string);
char** string_tokenize(char* string, char* args[10]);
void run_redir(char* cmdToRun, unsigned char);
void run_command(char* command);
void check_space(char * string);
void temp_run_command(char * command,int read, int write,unsigned char isPiped);
int countPipes(char* pipeCmd);
void run_pipe(char* cmd);
void handle_sigint(int signum);

void handle_sigint(int signum){//Signal Handler
 if(isChild){
   printf("\n");

    // int i;
    // for(i=0;i<ind;i++){
     //kill(pid,SIGTERM);
    // }
    kill(pid,SIGKILL);
    if(mult){
     killp = 1;
    }
    else{
     killp = 0;
    }
    
 }else{
   printf("\n");
 }
}

void run_redir(char* redirCommand, unsigned char isPiped){ //executes '>' command
  char * stdoutCmd = (char*) malloc(filesize * sizeof(char));
  char * fileName = (char*) malloc(filesize * sizeof(char));
  memset(fileName,'\0',strlen(fileName));
  memset(stdoutCmd,'\0',strlen(stdoutCmd));
  int isDoubleR = 0;
  int x = 0;int j =0;
   //get string 1 for pre '> or >>'
   while(redirCommand[x] != '>'){
         
       stdoutCmd[x] = redirCommand[x];
       x++;

   }
   if(redirCommand[x+1] == '>'){
     
     isDoubleR = 1;
     x+=2;

   } else{
    
      x++;
   }
    
   if(isDoubleR){
   
     char * token = strtok(redirCommand,">>");
     while ( token !=NULL){
         token = strtok(NULL,">> \n");
         if(token == NULL) break;
         else{
           strncpy(fileName,token,strlen(token));
         }
     }
   }else{
     char * token = strtok(redirCommand,">");
     while ( token !=NULL){
         token = strtok(NULL,"> \n");
         if(token == NULL) break;
         else{
           strncpy(fileName,token,strlen(token));
         }
     }

    }
   
  int y = dup(1);
  int in_dup;
  close(1);
  int fd;
   if(!isDoubleR){
      fd = open(fileName,O_TRUNC| O_RDWR | O_CREAT,0777);
     if(fd < 0){
       perror(fileName);
       return;
     }
 
   }else if (isDoubleR){
     fd = open(fileName,O_RDWR | O_CREAT |O_APPEND,0777);
     if(fd < 0){
       perror(fileName);
       return;
     }

   }
  check_space(stdoutCmd);
  if(isPiped){
    in_dup = dup(STDIN_FILENO);
    dup2(last_read,STDIN_FILENO);
  }
  if(strncmp(stdoutCmd, "cd", 2) == 0){
    close(fd);
    run_cd(stdoutCmd);
    close(fd);
    dup2(y,1);
    close(y);
    if(isPiped){
      dup2(in_dup,STDIN_FILENO);
      close(in_dup);
    }
    return;
  }
  else{
    //close(fd);
    run_command(stdoutCmd);
    close(fd);
    dup2(y,1);
    close(y);
     if(isPiped){
       dup2(in_dup,STDIN_FILENO);
       close(in_dup);
     }
    return;
 }

}

int countPipes(char* pipeCmd){

  int i;
  int count =0;
  for(i=0;i < strlen(pipeCmd);i++){

    if(pipeCmd[i] == '|'){

      count++;

    }  
    
  }  

  return count;
}

void run_pipe(char* cmd){

  int num_pipes = countPipes(cmd);  
  int totalCommands = num_pipes + 1;
  int currentPipeCount = 0;
  unsigned char isFirst = 0;
  unsigned char isLast = 0;
  unsigned char isPiped = 1;
  int i;
  int read = -1; int write = -1;
  char * token = (char*) malloc((strlen(cmd)+10)*sizeof(char));
   memset(token,'\0',strlen(cmd));
  
  
  int j =0;
  
  for(i =0; i < strlen(cmd);i++){
    while(cmd[i] != '|'){
      if(cmd[i] == '\n' || cmd[i] == '\0'){
         isLast = 1;
         break; 
      }
      
      token[j] = cmd[i];
      j++;i++;
    
    }
    if(cmd[i] == '|'){ 
      currentPipeCount++;
    }
    if(currentPipeCount == 1 && !isLast){
         read = -1;
         write = 1;
    }else if(isLast){
       if(strchr(token, '>') != NULL){
           
          run_redir(token,isPiped);
          return;
       }
      read = 1; write = -1;
    }else{
      read =1; write =1;
    }
    temp_run_command(token,read,write,isPiped);
    
   memset(token,'\0',strlen(token)); 
   j = 0;
  }
    


}
void run_cd(char* cmd){//executes cd command

 char* token = strtok(cmd, " \n");
 token = strtok(NULL," \n");

 chdir(token);
}

void temp_run_command(char * command,int read, int write, unsigned char isPiped){
 char* args[10];
 char* command1 = (char*)malloc(strlen(command)*sizeof(char));
 strncpy(command1, command, strlen(command));
 int x = 0; int fd[2]; 
  pipe(fd);
 for(x = 0; x<10;x++){
   args[x] = (char*)malloc(500*sizeof(char));
 }

 string_tokenize(command1,args);
 
 isChild = 1;
 
 if(killp){
 // kill(pid, SIGKILL);
  return;
 }
 
 pid = fork();
 
 
 if(pid == 0){// ls |

 signal(SIGINT, handle_sigint);
     
   if( read == -1 && write != -1){ //process is gonna write to the pipe
     //close(fd[0]);
     dup2(fd[1],STDOUT_FILENO);
     
   }else if(read != -1 && write == -1){ //process is gonna read from the pipe
     dup2(last_read,STDIN_FILENO);
   }else if(read != -1 && write != -1){
     dup2(last_read,0);
     dup2(fd[1],1);
 
   }  

   if (execvp(args[0], args) == -1) {
      perror("Error");
      return;
   }
  
 }

  wait(NULL);
 //if last process
 if(read == 1 && write == -1){
   close(fd[0]);
 }  
 

 close(fd[1]); //need to close write end always

 
 last_read = fd[0];


}  
void run_command(char* command){
 char* args[10];
 char* command1 = (char*)malloc(strlen(command)*sizeof(char));
 strncpy(command1, command, strlen(command));
 int x = 0;
 for(x = 0; x<10;x++){
   args[x] = (char*)malloc(500*sizeof(char));
 }


 string_tokenize(command1,args);
 isChild = 1;
 
 if(killp){
   //kill(pid, SIGKILL);
  return;
 }
 
 pid = fork();
 
 if(pid == 0){

   signal(SIGINT,handle_sigint);
    
   if (execvp(args[0], args) == -1) {
      perror("Error");
   }
   
 }
 
 wait(NULL);
 
}

int check_semicolon(char* string){
    int i;
    int len = strlen(string);
    for(i=0;i<len;i++){
        if(string[i] == ';'){
            return 1;
        }
    }

    return 0; 
}

char** string_tokenize(char* com, char* args[10]){

    char* com1 = (char*)malloc(strlen(com)*sizeof(char));
    strncpy(com1, com, strlen(com));
    char* token1  = strtok(com1, " ;\n");
    args[0] = token1; 

    int i = 1;

    while(token1!= NULL){
     token1 = strtok(NULL, " ;\n");
     if(token1!=NULL){
      args[i] = token1; 
      i++;
     }
    
    }
    args[i] = (char*)NULL;

    return args; 
}

void check_space(char* string){
    
 int index;
 index = 0;

 while(string[index] == ' ' || string[index] == '\t' || string[index] == '\n')
    {
        index++;
    }

 if(index!=0){
     int i = 0,j = 1;
     while(string[i+index] != '\0'){
         string[i] = string[i+index];
         i++;
     }
     string[i] = '\0';
    
 }

}

int main(int argc, char** argv){

char cmd[100];
char string[100];
char* arguments[10];

 int x = 0;
 for(x = 0; x<10;x++){
   arguments[x] = (char*)malloc(500*sizeof(char));
 }
  
signal(SIGINT,handle_sigint); 


while(1){

  ind = 0;
int mult_cmd;
//if(sigsetjmp(env,1) == 7){
//  exit(0);
//  printf("In jump\n");
//  continue;
//}
killp = 0;
printf("%s$ ", getcwd(cwd, 100));
fgets(cmd,99,stdin);

int cont = strncmp("exit", cmd, 4);

if(!cont){
 exit(0);
}

strncpy(string,cmd, 100);

mult_cmd = check_semicolon(string);
char* commands[20]; 

if(mult_cmd){
    mult = 1;
    int i ;
    for(i=0;i<20;i++){
        commands[i] = (char*)malloc(500*sizeof(char));
    }
    
    i=0;
    char* t = strtok(string, ";");
    commands[0] = t;
    i++;
    while(t!= NULL){
        t = strtok(NULL, ";");
        if(t!= NULL){
        commands[i] = t;
        i++;
        }
    }
    commands[i] = (char*)NULL; 
    i = 0;
    while(commands[i] != NULL){

        
         char* to_run = (char*)malloc(strlen(commands[i])*sizeof(char)); 
         strncpy(to_run,commands[i],strlen(commands[i]));

         check_space(to_run);

          if(strncmp(to_run, "cd", 2) == 0){
            run_cd(to_run);
            i++;
            isChild = 0;
            continue;
          }
          else if((strncmp(to_run,"pwd", 3) == 0) && (strchr(to_run,'>')==NULL)){
            printf("%s\n",getcwd(cwd,100));
            i++;
            continue;
          }else if(strchr(to_run,'|') != NULL){
            ind++;
                 run_pipe(to_run);
                 i++;
   	         isChild = 0;
                 continue;

           }else if((strchr(to_run,'>') != NULL)){
            ind++;
            run_redir(to_run,0);
            i++;
            isChild = 0;
            continue;
           }

         run_command(to_run);
         isChild = 0;
         i++;

    }
    continue;
}

check_space(string);

if(strncmp(string, "cd", 2) == 0){
    run_cd(string);
    isChild = 0;
    continue;
}
else if((strncmp(string,"pwd", 3) == 0) && (strchr(string,'>')==NULL)){
    printf("%s\n",getcwd(cwd,100));
    continue;
}else if((strchr(string,'>') != NULL) && (strchr(string,'|') == NULL)){
    run_redir(string,0);
    isChild = 0;
    continue;
}else if(strchr(string,'|') != NULL){

    run_pipe(string);
    isChild = 0;
    continue;
} 

strncpy(string,cmd, 100);
run_command(string);
isChild = 0;

 }


return 0;
}
