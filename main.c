#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <dirent.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>

char* Read() {
    char *toRead = NULL;
    ssize_t bufferSize = 0;
    toRead = readline("?>");
    toRead[strcspn(toRead, "\n")] = 0;
    add_history(toRead);
    return toRead;
}

char** ParseNoPipes(char* command) {
    char *token = strtok(command, " ");
    token[strcspn(token, "\n")] = 0;
    char *commandWithoutArgs = (char *) malloc(strlen(token) + 1);
    strcpy(commandWithoutArgs, token);
    char **argumentList = malloc(100 * sizeof(char *));
    for (int i = 0; i < 100; i++)
        argumentList[i] = malloc(100 * sizeof(char));
    argumentList[0] = token;
    int k = 1;
    while (token != NULL) {
        token = strtok(NULL, " ");
        if (token != NULL)
            token[strcspn(token, "\n")] = 0;
        argumentList[k++] = token;
    }
    for(int i = 0; i < k-1; i++){
        if(strstr(argumentList[i],"--")){
            printf("Wrong argument format\n");
            return NULL;
        }
    }
    return argumentList;
}

char** ParseWithPipes(char* command){
    char* initialCommand = malloc(100*sizeof(char));
    strcpy(initialCommand,command);
    char* token = strtok(command, "|");
    char** commandList = malloc(100* sizeof(char*));
    for(int i = 0; i < 100; i++)
        commandList[i] = malloc(100 * sizeof(char));
    int k=0;
    while(token != NULL){
        token[strlen(token)-1] = '\0';
        commandList[k++] = token;
        token = strtok(NULL,"|");
        if(token)
            token++;
    }
    commandList[k-1][strlen(commandList[k-1])] = initialCommand[strlen(initialCommand)-1];
    for(int i = 0; i < k-1; i++){
        if(strstr(commandList[i],"--")){
            printf("Wrong argument format\n");
            return NULL;
        }
    }
    return commandList;
}

int lsCustom(int argc, char*argv[] ){
    DIR *directory;
    if((directory=opendir(".")) == NULL){
        perror("Couldn't open directory");
        return 1;
    }
    struct dirent *dir;
    if(!argv[1]){
        while((dir = readdir(directory))){
            if(dir->d_name[0] == '.')
                continue;
            printf("%s ", dir -> d_name);
        }
    }
    if(argv[1] && strlen(argv[1]) == 2){
        if(argv[1] && strcmp(argv[1],"-F")==0){
            while((dir = readdir(directory))){
                if(dir->d_name[0] == '.')
                    continue;
                struct stat Stat;
                if(stat(dir ->d_name,&Stat) < 0){
                    perror("Couldn't open file");
                    return 1;
                }
                printf("%s", dir -> d_name);
                if(S_ISDIR(Stat.st_mode)){
                    printf("/");
                    printf("  ");
                    continue;
                }
                if(Stat.st_mode & S_IXUSR)
                    printf("*");
                printf("  ");
            }
            printf("\n");
        }
        if(argv[1] && strcmp(argv[1],"-s")==0){
            while((dir = readdir(directory))){
                if(dir->d_name[0] == '.')
                    continue;
                struct stat Stat;
                if(stat(dir ->d_name,&Stat) < 0){
                    perror("Couldn't open file");
                    return 1;
                }
                int realblock = Stat.st_blocks/2;
                printf("%d ", realblock);
                printf("%s  ", dir -> d_name);
            }
            printf("\n");
        }
        if(argv[1] && strcmp(argv[1],"-a")==0){
            while((dir = readdir(directory))){
                printf("%s ", dir -> d_name);
            }
            printf("\n");
        }
        if(argv[1] && strcmp(argv[1],"-l")==0){
            while((dir = readdir(directory))){
                if(dir->d_name[0] == '.')
                    continue;
                struct stat Stat;
                char time[50];
                if(stat(dir ->d_name,&Stat) < 0){
                    perror("Couldn't open file");
                    return 1;
                }
                strftime(time,50,"%Y-%m-%d %H:%M:%S", localtime(&Stat.st_mtime));

                printf( (S_ISDIR(Stat.st_mode)) ? "d" : "-");
                printf( (Stat.st_mode & S_IRUSR) ? "r" : "-");
                printf( (Stat.st_mode & S_IWUSR) ? "w" : "-");
                printf( (Stat.st_mode & S_IXUSR) ? "x" : "-");
                printf( (Stat.st_mode & S_IRGRP) ? "r" : "-");
                printf( (Stat.st_mode & S_IWGRP) ? "w" : "-");
                printf( (Stat.st_mode & S_IXGRP) ? "x" : "-");
                printf( (Stat.st_mode & S_IROTH) ? "r" : "-");
                printf( (Stat.st_mode & S_IWOTH) ? "w" : "-");
                printf( (Stat.st_mode & S_IXOTH) ? "x" : "-");

                printf(" %lu ", Stat.st_nlink);

                printf(" mati mati ");

                printf(" %ld ", Stat.st_size);

                printf(" %s ", time);

                printf(" %s", dir -> d_name);

                printf("\n");
            }
        }
    }
    else{
        if(argv[1] && strlen(argv[1]) == 3){
            if(strcmp(argv[1],"-ls")==0 || strcmp(argv[1],"-sl")==0){
                while((dir = readdir(directory))){
                    if(dir->d_name[0] == '.')
                        continue;
                    struct stat Stat;
                    char time[50];
                    if(stat(dir ->d_name,&Stat) < 0){
                        perror("Couldn't open file");
                        return 1;
                    }
                    strftime(time,50,"%Y-%m-%d %H:%M:%S", localtime(&Stat.st_mtime));

                    int realblock = Stat.st_blocks/2;
                    printf("%d ", realblock);

                    printf( (S_ISDIR(Stat.st_mode)) ? "d" : "-");
                    printf( (Stat.st_mode & S_IRUSR) ? "r" : "-");
                    printf( (Stat.st_mode & S_IWUSR) ? "w" : "-");
                    printf( (Stat.st_mode & S_IXUSR) ? "x" : "-");
                    printf( (Stat.st_mode & S_IRGRP) ? "r" : "-");
                    printf( (Stat.st_mode & S_IWGRP) ? "w" : "-");
                    printf( (Stat.st_mode & S_IXGRP) ? "x" : "-");
                    printf( (Stat.st_mode & S_IROTH) ? "r" : "-");
                    printf( (Stat.st_mode & S_IWOTH) ? "w" : "-");
                    printf( (Stat.st_mode & S_IXOTH) ? "x" : "-");

                    printf(" %lu ", Stat.st_nlink);

                    printf(" mati mati ");

                    printf(" %ld ", Stat.st_size);

                    printf(" %s ", time);

                    printf(" %s", dir -> d_name);

                    printf("\n");
                }
            }
            if(strcmp(argv[1],"-la")==0 || strcmp(argv[1],"-al")==0){
                while((dir = readdir(directory))){
                    struct stat Stat;
                    char time[50];
                    if(stat(dir ->d_name,&Stat) < 0){
                        perror("Couldn't open file");
                        return 1;
                    }
                    strftime(time,50,"%Y-%m-%d %H:%M:%S", localtime(&Stat.st_mtime));

                    printf( (S_ISDIR(Stat.st_mode)) ? "d" : "-");
                    printf( (Stat.st_mode & S_IRUSR) ? "r" : "-");
                    printf( (Stat.st_mode & S_IWUSR) ? "w" : "-");
                    printf( (Stat.st_mode & S_IXUSR) ? "x" : "-");
                    printf( (Stat.st_mode & S_IRGRP) ? "r" : "-");
                    printf( (Stat.st_mode & S_IWGRP) ? "w" : "-");
                    printf( (Stat.st_mode & S_IXGRP) ? "x" : "-");
                    printf( (Stat.st_mode & S_IROTH) ? "r" : "-");
                    printf( (Stat.st_mode & S_IWOTH) ? "w" : "-");
                    printf( (Stat.st_mode & S_IXOTH) ? "x" : "-");

                    printf(" %lu ", Stat.st_nlink);

                    printf(" mati mati ");

                    printf(" %ld ", Stat.st_size);

                    printf(" %s ", time);

                    printf(" %s", dir -> d_name);

                    printf("\n");
                }
            }
            if(strcmp(argv[1],"-lF")==0 || strcmp(argv[1],"-Fl")==0){
                while((dir = readdir(directory))){
                    if(dir->d_name[0] == '.')
                        continue;
                    struct stat Stat;
                    char time[50];
                    if(stat(dir ->d_name,&Stat) < 0){
                        perror("Couldn't open file");
                        return 1;
                    }
                    strftime(time,50,"%Y-%m-%d %H:%M:%S", localtime(&Stat.st_mtime));

                    int realblock = Stat.st_blocks/2;
                    printf("%d ", realblock);

                    printf( (S_ISDIR(Stat.st_mode)) ? "d" : "-");
                    printf( (Stat.st_mode & S_IRUSR) ? "r" : "-");
                    printf( (Stat.st_mode & S_IWUSR) ? "w" : "-");
                    printf( (Stat.st_mode & S_IXUSR) ? "x" : "-");
                    printf( (Stat.st_mode & S_IRGRP) ? "r" : "-");
                    printf( (Stat.st_mode & S_IWGRP) ? "w" : "-");
                    printf( (Stat.st_mode & S_IXGRP) ? "x" : "-");
                    printf( (Stat.st_mode & S_IROTH) ? "r" : "-");
                    printf( (Stat.st_mode & S_IWOTH) ? "w" : "-");
                    printf( (Stat.st_mode & S_IXOTH) ? "x" : "-");

                    printf(" %lu ", Stat.st_nlink);

                    printf(" mati mati ");

                    printf(" %ld ", Stat.st_size);

                    printf(" %s ", time);

                    printf(" %s", dir -> d_name);
                    if(S_ISDIR(Stat.st_mode)){
                        printf("/");
                        printf("\n");
                        continue;
                    }
                    if(Stat.st_mode & S_IXUSR)
                        printf("*");

                    printf("\n");
                }
            }
            if(strcmp(argv[1],"-sa")==0 || strcmp(argv[1],"-as")==0){
                while((dir = readdir(directory))){
                    struct stat Stat;
                    if(stat(dir ->d_name,&Stat) < 0){
                        perror("Couldn't open file");
                        return 1;
                    }
                    int realblock = Stat.st_blocks/2;
                    printf("%d ", realblock);
                    printf("%s  ", dir -> d_name);
                }
                printf("\n");
            }
            if(strcmp(argv[1],"-sF")==0 || strcmp(argv[1],"-Fs")==0){
                while((dir = readdir(directory))){
                    if(dir->d_name[0] == '.')
                        continue;
                    struct stat Stat;
                    if(stat(dir ->d_name,&Stat) < 0){
                        perror("Couldn't open file");
                        return 1;
                    }
                    int realblock = Stat.st_blocks/2;
                    printf("%d ", realblock);
                    printf("%s", dir -> d_name);
                    if(S_ISDIR(Stat.st_mode)){
                        printf("/");
                        printf("  ");
                        continue;
                    }
                    if(Stat.st_mode & S_IXUSR)
                        printf("*");
                    printf("  ");
                }
                printf("\n");
            }
            if(strcmp(argv[1],"-aF")==0 || strcmp(argv[1],"-Fa")==0){
                struct stat Stat;
                while((dir = readdir(directory))){
                    printf("%s", dir -> d_name);
                    if(stat(dir ->d_name,&Stat) < 0){
                        perror("Couldn't open file");
                        return 1;
                    }
                    if(S_ISDIR(Stat.st_mode)){
                        printf("/");
                        printf("  ");
                        continue;
                    }
                    if(Stat.st_mode & S_IXUSR)
                        printf("*");
                    printf("  ");
                }
                printf("\n");
            }
        }
    }

    return 1;
}

int tacCustom(int argc, char* argv[]){
    if(!argv[1]){
        char* toRead = malloc(100);
        char** readStuff = malloc(100);
        for(int i = 0; i < 100; i++)
            readStuff[i] = malloc(100);
        int counter=0;
        while(fscanf(stdin,"%s",toRead) != EOF){
            strcpy(readStuff[counter],toRead);
            counter++;
        }
        for(int i = counter-1; i >= 0; i--)
            printf("%s\n",readStuff[i]);
        return 1;
    }
    FILE *filePointer = NULL;
    char* fileName = argv[1];
    if((filePointer = fopen(fileName, "r")) == NULL){
        perror("Couldn't open file");
        return 1;
    }
    char fileContent[128][128];
    int i = 0, nbOfLines = 0;
    if(!argv[2]){
        while(fgets(fileContent[i], 128, filePointer)){
            i++;
        }
        nbOfLines = i;
        for(i = nbOfLines-1; i >= 0; i--){
            printf("%s", fileContent[i]);
        }
        fclose(filePointer);
    }
    else{
        if(argv[2] && strcmp(argv[2],"-s") == 0){
            char* line;
            size_t len = 0;
            ssize_t read;
            while((getdelim(&line, &len, argv[3][0], filePointer) != -1)){
                strcpy(fileContent[i], line);
                i++;
            }
            nbOfLines = i;
            for(i = nbOfLines-1; i >= 0; i--){
                printf("%s", fileContent[i]);
            }
            fclose(filePointer);
        }
        if(argv[2] && strcmp(argv[2],"-b") == 0){
            char* line;
            size_t len = 0;
            if(argv[4]){
                while((getdelim(&line, &len, argv[4][0], filePointer) != -1)){
                    strcpy(fileContent[i], line);
                    i++;
                }
            }
            else{
                while(fgets(fileContent[i], 128, filePointer)){
                    i++;
                }
            }
            nbOfLines = i;
            for(int j = 0; j < nbOfLines; j++){
                if(fileContent[j][strlen(fileContent[j])-1] == ','){
                    fileContent[j][strlen(fileContent[j])-1] = '\0';
                    char comma[130] = ",";
                    strcat(comma,fileContent[j+1]);
                    strcpy(fileContent[j+1],comma);
                }
            }
            nbOfLines = i;
            for(i = nbOfLines-1; i >= 0; i--){
                printf("%s", fileContent[i]);
            }
            fclose(filePointer);
        }
    }
    return 1;
}

int dirnameCustom(int argc, char* argv[]){
    if(strchr(argv[1],'/')){
        for(int i = strlen(argv[1])-1; i >= 0 ; i--)
            if(argv[1][i] == '/' && i != strlen(argv[1])-1){
                argv[1][i] = '\0';
                break;
            }
        printf("%s",argv[1]);
        return 1;
    }
    printf(".");
    return 1;
}

int ExecuteNoPipes(char** command) {
    int argc = 0;
    for (int i = 0; i >= 0; i++) {
        if (command[i] == NULL) {
            break;
        }
        argc++;
    }
    int simpleRedirectionIndex = 0, doubleRedirectionIndex = 0, inputRedirectionIndex = 0;
    for (int i = 0; command[i] != NULL; i++) {
        if (strcmp(command[i], ">>") == 0) {
            doubleRedirectionIndex = i;
            break;
        }
        else{
            if (strcmp(command[i], ">") == 0) {
                simpleRedirectionIndex = i;
                break;
            }
            else{
                if (strcmp(command[i], "<") == 0) {
                    inputRedirectionIndex = i;
                    break;
                }
            }
        }
    }
    if(simpleRedirectionIndex!=0){
        char* file = malloc(100);
        strcpy(file,command[simpleRedirectionIndex+1]);
        int outputFile = open(file,O_WRONLY | O_CREAT | O_TRUNC, 0644);
        int standardOutput = dup(STDOUT_FILENO);
        dup2(outputFile,STDOUT_FILENO);
        pid_t pid = fork();
        if(pid == 0){
            if(simpleRedirectionIndex==1){
                execlp(command[0],command[0],NULL);
            }
            if(simpleRedirectionIndex==2){
                execlp(command[0],command[0],command[1],NULL);
            }
            if(simpleRedirectionIndex==3){
                execlp(command[0],command[0],command[1],command[2],NULL);
            }
            if(simpleRedirectionIndex==4){
                execlp(command[0],command[0],command[1],command[2],command[3],NULL);
            }
            if(simpleRedirectionIndex==5){
                execlp(command[0],command[0],command[1],command[2],command[3],command[4],NULL);
            }
        }
        else{
            wait(NULL);
            close(outputFile);
            dup2(standardOutput,STDOUT_FILENO);
            close(standardOutput);
            return 1;
        }
    }
    else{
        if(doubleRedirectionIndex!=0){
            char* file = malloc(100);
            strcpy(file,command[doubleRedirectionIndex+1]);
            int outputFile = open(file,O_WRONLY | O_APPEND, 0644);
            int standardOutput = dup(STDOUT_FILENO);
            dup2(outputFile,STDOUT_FILENO);
            pid_t pid = fork();
            if(pid == 0){
                if(doubleRedirectionIndex==1){
                    execlp(command[0],command[0],NULL);
                }
                if(doubleRedirectionIndex==2){
                    execlp(command[0],command[0],command[1],NULL);
                }
                if(doubleRedirectionIndex==3){
                    execlp(command[0],command[0],command[1],command[2],NULL);
                }
                if(doubleRedirectionIndex==4){
                    execlp(command[0],command[0],command[1],command[2],command[3],NULL);
                }
                if(doubleRedirectionIndex==5){
                    execlp(command[0],command[0],command[1],command[2],command[3],command[4],NULL);
                }
            }
            else{
                wait(NULL);
                close(outputFile);
                dup2(standardOutput,STDOUT_FILENO);
                close(standardOutput);
                return 1;
            }
        }
        else{
            if(inputRedirectionIndex!=0){
                char* file = malloc(100);
                strcpy(file,command[inputRedirectionIndex+1]);
                int inputFile = open(file,O_RDONLY,0644);
                int standardInput = dup(STDIN_FILENO);
                dup2(inputFile,STDIN_FILENO);
                pid_t pid = fork();
                if(pid == 0){
                    if(inputRedirectionIndex==1){
                        execlp(command[0],command[0],NULL);
                    }
                    if(inputRedirectionIndex==2){
                        execlp(command[0],command[0],command[1],NULL);
                    }
                    if(inputRedirectionIndex==3){
                        execlp(command[0],command[0],command[1],command[2],NULL);
                    }
                    if(inputRedirectionIndex==4){
                        execlp(command[0],command[0],command[1],command[2],command[3],NULL);
                    }
                    if(inputRedirectionIndex==5){
                        execlp(command[0],command[0],command[1],command[2],command[3],command[4],NULL);
                    }
                }
                else{
                    wait(NULL);
                    close(inputFile);
                    dup2(standardInput,STDIN_FILENO);
                    close(standardInput);
                    return 1;
                }
            }
        }
    }
    if (strcmp(command[0], "help") == 0){
        printf("The implemented commands are:\n");
        printf("The \"ls\" command. The implemented parameters are: -l, -s, -a, -F\n");
        printf("The \"tac\" command. The implemented parameters are: -b, -s\n");
        printf("The \"dirname\" command. No parameters implemented\n");
        return 1;
    }
    if (strcmp(command[0], "version") == 0){
        printf("This Shell Project was made by Matei Macsinga\n");
        return 1;
    }
    if (strcmp(command[0], "exit") == 0)
        return 0;
    if (strcmp(command[0], "ls") == 0) {
        lsCustom(argc, command);
        if (!command[1])
            printf("\n");
        return 1;
    }
    if (strcmp(command[0], "tac") == 0) {
        tacCustom(argc, command);
        return 1;
    }
    if (strcmp(command[0], "dirname") == 0) {
        dirnameCustom(argc, command);
        printf("\n");
        return 1;
    }
    pid_t pid = fork();
    if (pid == 0) {
        if (execvp(command[0], command) == -1) {
            perror("Command doesn't exist");
            exit(1);
        }
    } else {
        waitpid(pid, NULL, 0);
    }
    return 1;
}

int ExecuteWithPipes(char* fullCommand, char** commandList){
    if(strcmp(commandList[2],"")==0){
        char** firstCommand = ParseNoPipes(commandList[0]);
        char** secondCommand = ParseNoPipes(commandList[1]);
        pid_t pid1, pid2;
        int fd[2];
        if(pipe(fd) == -1){
            perror("Couldn't generate pipe");
            return 1;
        }

        pid1 = fork();
        if(pid1 < 0){
            perror("Couldn't create process");
            return 1;
        }
        if(pid1 == 0){
            dup2(fd[1], STDOUT_FILENO);
            close(fd[0]);
            close(fd[1]);
            int argc = 0;
            for(int i = 0; i >= 0; i++){
                if(firstCommand[i] == NULL){
                    break;
                }
                argc++;
            }
            if(strcmp(firstCommand[0],"ls")==0){
                lsCustom(argc,firstCommand);
                exit(1);
            }
            else{
                if(strcmp(firstCommand[0],"tac")==0){
                    tacCustom(argc,firstCommand);
                    exit(1);
                }
                else{
                    if(strcmp(firstCommand[0],"dirname")==0){
                        dirnameCustom(argc,firstCommand);
                        exit(1);
                    }
                    else{
                        execvp(firstCommand[0],firstCommand);
                    }
                }
            }
        }
        pid2 = fork();
        if(pid2 < 0){
            perror("Couldn't create process");
            return 1;
        }
        if(pid2 == 0){
            dup2(fd[0], STDIN_FILENO);
            close(fd[1]);
            close(fd[0]);
            int argc = 0;
            for(int i = 0; i >= 0; i++){
                if(secondCommand[i] == NULL){
                    break;
                }
                argc++;
            }
            for(int i = 0; secondCommand[i] != NULL; i++){
                if(strcmp(secondCommand[i],">")==0 || strcmp(secondCommand[i],">>")==0 || strcmp(secondCommand[i],"<")==0){
                    ExecuteNoPipes(secondCommand);
                    exit(1);
                }
            }
            if(strcmp(secondCommand[0],"ls")==0){
                lsCustom(argc,secondCommand);
                printf("\n");
                exit(1);
            }
            else{
                if(strcmp(secondCommand[0],"tac")==0){
                    tacCustom(argc,secondCommand);
                    exit(1);
                }
                else{
                    if(strcmp(secondCommand[0],"dirname")==0){
                        dirnameCustom(argc,secondCommand);
                        exit(1);
                    }
                    else{
                        execvp(secondCommand[0],secondCommand);
                    }
                }
            }
        }
        close(fd[1]);
        close(fd[0]);
        waitpid(pid1,NULL,0);
        waitpid(pid2,NULL,0);
        return 1;
    }
    else{
        int occurences = 0;
        char *substring=strchr(fullCommand,'|');
        while (substring!=NULL) {
            occurences++;
            substring=strchr(substring+1,'|');
        }

        int fd[occurences*2];

        for(int i = 0; i < occurences; i++){
            if(pipe(fd+i*2) == -1){
                perror("Couldn't generate pipe");
                return 1;
            }
        }

        int commandCounter = 0;
        int descriptorCounter = 0;
        pid_t pid;
        while(strcmp(commandList[commandCounter],"") != 0){
            pid = fork();
            if(pid < 0){
                perror("Couldn't create process");
                return 1;
            }
            else{
                if(pid == 0){
                    if(strcmp(commandList[commandCounter+1],"") != 0)
                        dup2(fd[descriptorCounter+1],STDOUT_FILENO);
                    if(descriptorCounter != 0)
                        dup2(fd[descriptorCounter-2], STDIN_FILENO);
                    for(int l = 0; l < occurences*2; l++)
                        close(fd[l]);
                    char** command = ParseNoPipes(commandList[commandCounter]);
                    for(int i = 0; command[i] != NULL; i++){
                        if(strcmp(command[i],">")==0 || strcmp(command[i],">>")==0 || strcmp(command[i],"<")==0){
                            ExecuteNoPipes(command);
                            exit(1);
                        }
                    }
                    execvp(command[0],command);
                }
            }
            commandCounter++;
            descriptorCounter+=2;
        }
        for(int l = 0; l < occurences*2; l++)
            close(fd[l]);
        for(int l=0; l < occurences; l++)
            wait(NULL);
    }
    return 1;
}

void shellLoop(){
    char* fullCommand;
    char** parsedCommand;
    int ok = 1;

    while(ok){
        fullCommand = Read();
        char* initialCommand = malloc(100);
        strcpy(initialCommand,fullCommand);
        if(strchr(fullCommand,'|') == NULL){
            parsedCommand = ParseNoPipes(fullCommand);
            if(parsedCommand)
                ok = ExecuteNoPipes(parsedCommand);
        }
        else{
            parsedCommand = ParseWithPipes(fullCommand);
            if(parsedCommand)
                ok = ExecuteWithPipes(initialCommand, parsedCommand);
        }
        free(fullCommand);
    }
}

int main() {
    shellLoop();
    return 0;
}
