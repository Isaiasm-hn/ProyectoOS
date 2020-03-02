#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstddef>
#include <string.h>
#include <vector>

using namespace std;

enum PipeRedirect {PIPE, PIPES, REDIRECT, WRITE, NEITHER};

PipeRedirect parse_command(int argc, char** argv, char** cmd1, char** cmd2) {

  PipeRedirect result = NEITHER;

  int split = -1;
  int counter = 0;

  for (int i=0; i<argc; i++) {
    if (strcmp(argv[i], "|") == 0) {
      result = PIPE;
      split = i;
      counter += 1;

    } else if (strcmp(argv[i], ">>") == 0) {
      result = REDIRECT;
      split = i;
    } else if (strcmp(argv[i], ">") == 0) {
      result = WRITE;
      split = i;
    }
  }


  if (result != NEITHER) {
    for (int i=0; i<split; i++)
      cmd1[i] = argv[i];

    int count = 0;
    for (int i=split+1; i<argc; i++) {
      cmd2[count] = argv[i];
      count++;
    }

    cmd1[split] = NULL;
    cmd2[count] = NULL;
  }

  return result;
}
void executeCommand(char command[]){
    int index = 1;
    char *args[30];
    char *token = strtok(command, " ");
    args[0] = strdup(token);

    while ((token = strtok(NULL, " ")) != NULL)
    {
        args[index] = strdup(token);
        ++index;
    }
    args[index] = NULL;

    execvp(args[0], args);
}
void pipe_cmd(char *comando) {
/*pid_t pid;
char* command1,* command2;
        for (size_t i = 0; comando[i] != '\0'; i++){
            if(comando[i]=='|'){
                command1 = strtok(comando, "|");
                command2 = strtok(NULL, "|");

                if(fork()==0){
                    int pid;
                    int status[2];
                    pipe(status);
                    if((pid=fork())<0){
                        cout<<"Error on command execution. Killing process"<<endl;
                        exit(1);
                    }
                    if(pid == 0){
                        close(1);
                        dup(status[1]);
                        close(status[0]);
                        close(status[1]);
                        executeCommand(command1);
                    }else{
                        close(0);
                        dup(status[0]);
                        close(status[0]);
                        close(status[1]);
                        executeCommand(command2);
                    }
                }
            }
        }
        */
}

int read_args(char **argv) {
  char *cstr;
  string arg;
  int argc = 0;

  while (cin >> arg) {
    cstr = new char[arg.size()+1];
    strcpy(cstr, arg.c_str());
    argv[argc] = cstr;

    argc++;

    if (cin.get() == '\n')
      break;
  }

  argv[argc] = NULL;

  return argc;
}

void redirect_cmd(char** cmd, char** file) {
  int fds[2];
  int count; 
  int fd; 
  char c;
  pid_t pid;  

  pipe(fds);

  if (fork() == 0) {
    fd = open(file[0], O_RDWR | O_APPEND | O_CREAT, 0666);

    if (fd < 0) {
      return;
    }

    dup2(fds[0], 0);

    close(fds[1]);

    while ((count = read(0, &c, 1)) > 0)
      write(fd, &c, 1); 

    execlp("echo", "echo", NULL);

  } else if ((pid = fork()) == 0) {
    dup2(fds[1], 1);

    close(fds[0]);

    execvp(cmd[0], cmd);
    perror("execvp failed");

  } else {
    waitpid(pid, NULL, 0);
    close(fds[0]);
    close(fds[1]);
  }
}

void write_cmd(char** cmd, char** file) {
  int fds[2]; 
  int count;  
  int fd;     
  char c;     
  pid_t pid;  

  pipe(fds);

  if (fork() == 0) {
    fd = open(file[0], O_WRONLY | O_TRUNC | O_CREAT, 0666);

    if (fd < 0) {
      return;
    }

    dup2(fds[0], 0);

    close(fds[1]);

    while ((count = read(0, &c, 1)) > 0)
      write(fd, &c, 1);

    execlp("echo", "echo", NULL);

  } else if ((pid = fork()) == 0) {
    dup2(fds[1], 1);

    close(fds[0]);

    execvp(cmd[0], cmd);
    perror("execvp failed");

  } else {
    waitpid(pid, NULL, 0);
    close(fds[0]);
    close(fds[1]);
  }
}

void run_cmd(int argc, char** argv) {
  pid_t pid;
  const char *amp;
  amp = "&";
  bool found_amp = false;

  if (strcmp(argv[argc-1], amp) == 0)
    found_amp = true;

  pid = fork();

  if (pid < 0)
    perror("Error (pid < 0)");

  else if (pid == 0) {
    if (found_amp) {
      argv[argc-1] = NULL;
      argc--;
    }

    execvp(argv[0], argv);
    perror("execvp error");

  } else if (!found_amp)
    waitpid(pid, NULL, 0);
}

int main() {
  char *argv[50], *cmd1[50], *cmd2[50];
  char* comando;
  PipeRedirect pipe_redirect;
  int argc;
  while (true) {
    cout << "> ";
    argc = read_args(argv);
    pipe_redirect = parse_command(argc, argv, cmd1, cmd2);
    if (pipe_redirect == PIPE)
      pipe_cmd(comando);
    else if (pipe_redirect == REDIRECT) 
      redirect_cmd(cmd1, cmd2);
    else if (pipe_redirect == WRITE)
      write_cmd(cmd1,cmd2);
    else
      run_cmd(argc, argv);           
    for (int i=0; i<argc; i++)
      argv[i] = NULL;
  }

  return 0;
}                                                                                                                                                                                                                                                                                                                                                                                                                                                           