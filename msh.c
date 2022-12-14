/*
  Name: Diego Vester
  ID: 1001329342
*/

// gcc msh.c -o msh -std=c99

// The MIT License (MIT)
// 
// Copyright (c) 2016 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 4     // Mav shell only supports four arguments 

#define MAX_PID_SIZE 20         // Mav shell is able to print up to 20 previous PIDs

#define MAX_HISTORY_SIZE 15     // Mav shell supports the history command, listing 15 commands

int run = 1;
int pid_count = 0;
int pid_array[MAX_PID_SIZE];

int command_count = 0;
char *command_array[MAX_HISTORY_SIZE];




/*****REQUIREMENT 12 ********/
      /*
      Your shell shall support the history command which will list the last 15 commands entered by the user.
      Typing !n, where n is a number between 0 and 14 
        will result in your shell re-running the nth command.
      If the nth command does not exist 
        then your shell will state "Command not in history".
      The output shall be a list of numbers 1 through n and their commands, each on a separate line, single spaced.
      If there are less than 15 commands in the history 
        only list the commands the user has entered up to that point
      */

// insertCommand() will insert the current command into an array of a size up to MAX_HISTORY_SIZE
// if the array no longer has space, the funtion will delete the oldest command
// and shift all commands in the array one lower index
// then insert the most recent command on the index of MAX_HISTORY_SIZE
// insertCommand() will return the size of the array 
int insertCommand(char *current_command)
{
  if(command_count < MAX_HISTORY_SIZE)
  {
    command_array[command_count] = current_command;
    command_count ++;
  }
  else
  {
    for(int i = 0; i < MAX_HISTORY_SIZE; i++)
    {
      command_array[i] = command_array[i+1];
    }
    command_array[MAX_HISTORY_SIZE-1] = current_command;
  }
  return command_count;
}

// insertPID will insert the current PID into an array of a size up to MAX_PID_SIZE
// if the array no longer has space, the function will delete the oldest PID
// and shift all PIDs in the array one lower index
// then insert the most recent PID on the index of MAX_PID_SIZE
// insertPID() will return the size of the array
int insertPID(int current_PID)
{
  if(pid_count < MAX_PID_SIZE)
  {
    pid_array[pid_count] = current_PID;
    pid_count ++;
  }
  else
  {
    for(int i = 0; i<MAX_PID_SIZE; i++)
    {
      pid_array[i] = pid_array[i+1];
    }
    
    pid_array[MAX_PID_SIZE-1] = current_PID;
  }
  return pid_count;
}

void printPIDs()
{
  printf("List of PIDs (%d)\n", pid_count);
  for(int i = 0; i < pid_count; i++)
  {
    printf("%d\n", pid_array[i]);
  }
}

int main()
{

  char * command_string = (char*) malloc( MAX_COMMAND_SIZE );
  
  while( run )
  {
    
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (command_string, MAX_COMMAND_SIZE, stdin) );

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;                                         
                                                           
    char *working_string  = strdup( command_string );                

    // we are going to move the working_string pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *head_ptr = working_string;

    // Tokenize the input strings with whitespace used as the delimiter
    while ( ( (argument_ptr = strsep(&working_string, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    // Now print the tokenized input as a debug check
    // \TODO Remove this code and replace with your shell functionality
    
    /*
    int token_index  = 0;
    for( token_index = 0; token_index < token_count; token_index ++ ) 
    {
      printf("token[%d] = %s\n", token_index, token[token_index] );  
    }
    */
    
    

    char *str_cd = "cd";
    char *str_quit = "quit";
    char *str_exit = "exit";
    char *str_listpids = "listpids";
    const char split[2] = "!";
    char *command_n;

    


    
    if(token[0] != NULL)
    {
      command_n = strtok(token[0], split);
      //printf("command_n = %s", command_n);

      int result_quit = strcmp(token[0], str_quit);
      int result_exit = strcmp(token[0], str_exit);
      if( result_quit == 0 || result_exit == 0)
      {
        exit(0);
      }
    }
    int child_pid;
    int parent_pid;
   
    pid_t pid = fork( );
    if( pid == 0 && token[0] != NULL )
    {
      
      
      child_pid = getpid();
      insertCommand(token[0]);
      insertPID(child_pid);
      
      // Notice you can add as many NULLs on the end as you want
      // replace arguments with token
      int ret = execvp( token[0], &token[0] );  
      
      if( ret == -1 )
      {
        int result_cd = strcmp(token[0], str_cd);
        int result_listpids = strcmp(token[0], str_listpids);
        if(result_cd == 0)
        {
          chdir(token[1]);
        } 
        else if(result_listpids == 0)
        {
          printPIDs();
        }
        else
        {
          printf("%s : Command not found. \n", token[0]);
        }
      }    
    }
    else 
    {
      //parent_pid = getpid();
      //insertPID(parent_pid);
      int status;
      wait( & status );
    }
    free( head_ptr );
  }
  return 0;
  // e2520ca2-76f3-90d6-0242ac120003
}

// REQUIREMENTS COMPLETE: 10/18

/*COMPLETE****REQUIREMENT 1 ********/
      // Your program will print out a prompt of msh> when it is ready to 
      // accept input. It must read a line of input and, if the command given is a supported shell 
      // command, it shall execute the command and display the output of the command

      
/*COMPLETE****REQUIREMENT 2 ********/
      // If the command is not supported
      // your shell shall print the invalid command 
      // follow by ": Command not found."


/*COMPLETE****REQUIREMENT 4 ********/
      // After each command completes, your program shall 
      // print the msh> prompt and accept another line of input

/*COMPLETE****REQUIREMENT 5 ********/
      // Your shell will exit with status zero 
      // if the command is "quit" or "exit"

/*COMPLETE****REQUIREMENT 6 ********/
      // If the user types a blank line, your shell will
      // quietly and with no other output
      // print another prompt and accept a new line of input

/*COMPLETE****REQUIREMENT 7 ********/
      // Your version of Mav shell shall support 
      // up to 10 command line parameters in addition to the command


/*COMPLETE****REQUIREMENT 8 ********/
    /*Your shell shall support and execute any command entered
      Any comand in 
           /bin
           /usr/bin/
           /user/local/bin/
           the current working directory
        is to be considered valid for testing
        Your shell shall search in the following PATH order:
        1. Current working directory,
        2. /usr/local/bin
        3. /usr/bin
        4. /bin
        Parameters may also be combined.
        For example, ps may be executed as:
        ps -aef
        ps -a -e -f
      */

     /*
/bin dir     
bash           fuser       mountpoint     sleep                           umount
cat            grep        mv             ss                              uname
chgrp          gunzip      nano           stty                            uncompress
chmod          gzexe       networkctl     su                              vdir
chown          gzip        nisdomainname  sync                            wdctl
cp             hostname    pidof          systemctl                       ypdomainname
dash           ip          ping           systemd                         zcat
date           journalctl  ping4          systemd-ask-password            zcmp
dd             kill        ping6          systemd-escape                  zdiff
df             ln          ps             systemd-inhibit                 zegrep
dir            login       pwd            systemd-machine-id-setup        zfgrep
dmesg          loginctl    rbash          systemd-notify                  zforce
dnsdomainname  ls          readlink       systemd-sysusers                zgrep
domainname     lsblk       rm             systemd-tmpfiles                zless
echo           mkdir       rmdir          systemd-tty-ask-password-agent  zmore
egrep          mknod       rnano          tar                             znew
false          mktemp      run-parts      tempfile
fgrep          more        sed            touch
findmnt        mount       sh             true
     */

/*
usr/bin dir
addpart                             id                          sha1sum
addr2line                           infocmp                     sha224sum
apt                                 infotocap                   sha256sum
apt-cache                           install                     sha384sum
apt-cdrom                           instmodsh                   sha512sum
apt-config                          ionice                      shasum
apt-get                             ipcmk                       shred
apt-key                             ipcrm                       shuf
apt-mark                            ipcs                        size
ar                                  ischroot                    skill
arch                                join                        slabtop
as                                  json_pp                     slogin
*/

/*

*/

/*COMPLETE****REQUIREMENT 9 ********/
      /*
      Mav shell shall be implemented using fork(), wait() and one of the exec family of functions
      Your mav shell shall not use system().
      Use of system() will result in a grade of 0.
      */

/*COMPLETE****REQUIREMENT 10 ********/
      /*
      Your shell shall support the cd command to change the directories.
      Your shell must handle cd ..
      */

/*COMPLETE****REQUIREMENT 11 ********/
      /*
      Your shell shall support the listpids command to list the PIDs of the last 20 processes spawned by your shell.
      If there have been less than 20 processes spawned
        then it shall print only those process PIDs
      */

/*****REQUIREMENT 12 ********/
      /*
      Your shell shall support the history command which will list the last 15 commands entered by the user.
      Typing !n, where n is a number between 0 and 14 
        will result in your shell re-running the nth command.
      If the nth command does not exist 
        then your shell will state "Command not in history".
      The output shall be a list of numbers 1 through n and their commands, each on a separate line, single spaced.
      If there are less than 15 commands in the history 
        only list the commands the user has entered up to that point
      */

  /*****REQUIREMENT 13 ********/
      /*
      [This requirement is removed.]
      */

  /*****REQUIREMENT 14 ********/
      /*
      Tabs or spaces shall be used to indent the code.
      Your code must use one or the other.
      All indentation must be consistent.
      */

  /*****REQUIREMENT 15 ********/
      /*
      No line of code shall exceed 100 characters.
      */

  /*****REQUIREMENT 16 ********/
      /*
      Each source code file shall have the following header filled out:
      /*
        Name: Student Name
        ID: 10000001
      */

  /*****REQUIREMENT 17 ********/
      /*
      All code must be well commented.
      This means descriptive comments that tell the intent of the code, not just what the code is executing.
      The following are poor comments
        
        // Set working_str equal to strdup return
        char *working_str = strdup( cmd_str );
        // Set working_root equal to working_str
        char *working_root = working_str;
      The following explains the intent:
        // Save a copy of the command line since strsep
        // will end up moving the pointer head
        char *working_str = strdup( cmd_str );
        // we are going to move the working_str pointer so
        // keep track of its original value so we can deallocate
        // the correct amount at the end
        char *working_root = working_str;
      When in doubt, over comment your code.
      */

  /*****REQUIREMENT 18 ********/
      /*
      Keep your curly brace placement consistent.
      If you place curly on a new line, always place curly braces on a new end.
      Don't mix end line brace placement with new line brace placement
      */


  /*****REQUIREMENT 19 ********/
      /*
      Each function should have a header that describes 
        its name
        any parameters expected
        any return values
        a description of what the function does
      */

  /*****REQUIREMENT 20 ********/
      /*
      Remove all extraneous debug output before submission.
      The only output shall be the output of the commmands entered or the shell prompt.
      */
      
  /*****REQUIREMENT ETC ********/
      /*
      compile code on VM with
        gcc msh.c -o msh -std=c99
      */

  /*****REQUIREMENT SUBMISSION ********/
      /*
      turn in msh.c
      */

  /***** HINTS ********/
      /*
      Read man pages
        fork
        exec
        exit
        print
        fgets
        strtok
        strsep
        strcmp
        wait
        pause
      Use fork and one of the exec family to execute the command and
      call wait to wait for the child to complete
      If the command is "cd"
        then use chdir() instead of exec
      
      chdir() must be called from the parent
      If you see garbage in any of your commands or parents,
        try using the functions memset() or bzero() to clear out your input string and token array
        before and/or after you are done using them.
      Verify you are NULL terminating your strings.
      There are examples that show how to use execl and execvp
      */
      
    
    
 
