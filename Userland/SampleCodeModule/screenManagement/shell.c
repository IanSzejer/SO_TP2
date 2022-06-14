#include "shell.h"
#include "stdinout.h"
#include "phylo.h"
#include "test.h"
#define SHELL_BUFFER_SIZE 128
#define ARG_AMOUNT 6
#define ARG_SIZE 21
#define MEM_ID 1
typedef struct
{
    char line[SHELLW];
    int isCmd;
} shell_line;

typedef struct
{
    void* (*shellf)(void*);
    char *name;
    char *description;
    int argAmount;
    int procContext;
} t_shellc;

int cmdIndex(char *buf,char args[ARG_AMOUNT][ARG_SIZE]);
void loadCommand(void *(*f)(void *), char *name, char *desc,int argAmount,int procContext);
void copyOneLineUp(shell_line shellBuffer[SHELLH]);
void copyCommandDescriptor(char *buf, t_shellc cmd);
void copyLinesToShellOutput(char lines[][SHELLW], int qty);
void loop(int argc,char argv[ARG_AMOUNT][ARG_SIZE]);
void cat(int argc,char argv[ARG_AMOUNT][ARG_SIZE]);
void wc(int argc,char argv[ARG_AMOUNT][ARG_SIZE]);
void filter(int argc,char argv[ARG_AMOUNT][ARG_SIZE]);
void memState(int argc,char argv[ARG_AMOUNT][ARG_SIZE]);
void callKill(int argc,char argv[ARG_AMOUNT][ARG_SIZE]);
void callNice(int argc,char argv[ARG_AMOUNT][ARG_SIZE]);
void changeState(int argc,char argv[ARG_AMOUNT][ARG_SIZE]);
void getPipes(int argc,char argv[ARG_AMOUNT][ARG_SIZE]);
void getAllProcesses(int argc,char argv[ARG_AMOUNT][ARG_SIZE]);
void pSharedMemory();
void rSharedMemory();

static int cmdCounter = 0;
int currentShell = 0;



static t_shellc shellCommands[10] = {{0, 0, 0}};



void help(int argc,char argv[ARG_AMOUNT][ARG_SIZE])
{

    char lines[20][SHELLW];

    for (int i = 0; i < cmdCounter; i++)
    {
        copyCommandDescriptor(lines[i], shellCommands[i]);
        print(lines[i]);
        print("\n");
    }
    copyLinesToShellOutput(lines, cmdCounter);
    exit();
}

void inforeg(int argc,char argv[ARG_AMOUNT][ARG_SIZE])
{

    int j = 0;
    char *regs[17] = {0};
    get_regs(regs);
    char lines[5][SHELLW];
    for (int i = 0; i < 5; i++)
    {
        lines[i][0] = 0;
        for (int k = 0; k < 4 && j < 17; k++)
        {
            int len = strcat(lines[i], regs[j++]);
            lines[i][len] = ' ';
            lines[i][len + 1] = 0;
        }
    }
    copyLinesToShellOutput(lines, 5);
    exit();
}

void divideByZero()
{
    int a = 5, b = 0, c;
    c = a / b;
    c++;
    exit();
}

void invalidOpCode()
{
    __asm__("UD2"); // inline assembler. Usamos la instruccion UD2 que arroja la excepcion.
    exit();
}


void copyOneLineUp(shell_line shellBuffer[SHELLH])
{
    for (int i = 0; i < SHELLH - 1; i++)
    {
        strcpy(shellBuffer[i].line, shellBuffer[i + 1].line);
        shellBuffer[i].isCmd = shellBuffer[i + 1].isCmd;
    }
}

void copyCommandDescriptor(char *buf, t_shellc cmd)
{
    int len = strlength(cmd.name);
    strcpy(&buf[0], cmd.name);
    strcpy(&buf[len], " - ");
    strcpy(&buf[len + 3], cmd.description);
}

void copyLinesToShellOutput(char lines[][SHELLW], int qty)
{
   /* shell_line *shell = currentShell == 0 ? shellBuffer1 : shellBuffer2;
    for (int i = 0; i < qty; i++)
    {
        copyOneLineUp(currentShell == 0 ? shellBuffer1 : shellBuffer2);
    }

    int idx = SHELLH - qty - 1;
    for (int i = 0; i < qty && i < SHELLH; i++)
    {
        strcpy(shell[idx].line, lines[i]);
        shell[idx].isCmd = 0;
        idx++;
    }*/
    
}

void setupShellCommands()
{
    loadCommand((void *(*)(void*))&help, "help", "Shows a list of available commands",0,BACKGROUND);
    loadCommand((void *(*)(void*))&inforeg, "inforeg", "Shows the value of all registers",0,BACKGROUND);
    loadCommand((void *(*)(void*))&loop,"loop","prints a message with a delay inputed by user",1,BACKGROUND);
    loadCommand((void *(*)(void*))&cat,"cat", "prints what its received",0,FOREGROUND);
    loadCommand((void *(*)(void*))&wc,"wc", "counts the amount of lines inputed ",0,FOREGROUND);
    loadCommand((void *(*)(void*))&filter,"filter", "prints what its received,excluding vocals",0,FOREGROUND);
    loadCommand((void *(*)(void*))&phylo,"phylo","philosopher problem",0,FOREGROUND);
    loadCommand((void *(*)(void*))&memState,"mem","see memory status",0,BACKGROUND);
    loadCommand((void *(*)(void*))&callKill,"kill","kill a process",1,BACKGROUND);
    loadCommand((void *(*)())&callNice,"nice","change a process priority",2,BACKGROUND);
    loadCommand((void *(*)(void*))&changeState,"block or unblock","block or unblock a process",2,BACKGROUND);
    loadCommand((void *(*)(void*))&getPipes,"pipe","see pipes status",0,BACKGROUND);
    loadCommand((void *(*)(void*))&getAllProcesses,"ps","see processes status",0,BACKGROUND);
    loadCommand((void *(*)(void*))&test_sync,"synctest","executes sync test",2,BACKGROUND);
    loadCommand((void *(*)(void*))&test_mm,"memtest","executes memory manager test",1,BACKGROUND);
    loadCommand((void *(*)(void*))&test_prio,"priotest","executes prio test",0,BACKGROUND);
    loadCommand((void *(*)(void*))&test_processes,"processtest","executes process test",1,BACKGROUND);
    loadCommand((void *(*)(void*))&pSharedMemory,"pShared","gets a shared memory block anr writes in it",0,BACKGROUND);
    loadCommand((void *(*)(void*))&rSharedMemory,"rShared","prints a shared memory block string in it",0,BACKGROUND);
}


void cat(int argc,char argv[ARG_AMOUNT][ARG_SIZE]){
    char buffer[100];
    while(scanf(buffer)>0)
    {
        int i=0;
        while(buffer[i]){
            putChar(buffer[i]);
            i++;
            
        }
        print("\n");
    }
    exit();
    
}

void wc(int argc,char argv[ARG_AMOUNT][ARG_SIZE]){
    char buffer[100];
    int count=0;
    
    while(system_read(STDIN,buffer,100)>0)
        count++;
    char num[3]={0};
    num[0]=intToChar(count%10);
    if (count>10){
        num[1]=num[0];
        num[0]=intToChar((count/10)%10);
    }

    print(num);
    exit();
}

void filter(int argc,char argv[ARG_AMOUNT][ARG_SIZE]){
    char buffer[100];
    while(system_read(STDIN,buffer,100)>0)
    {
        int i=0;
        while(buffer[i]){
            if (buffer[i]=='a'||buffer[i]=='e'||buffer[i]=='i'||buffer[i]=='o'||buffer[i]=='u'||buffer[i]=='A'||buffer[i]=='E'||buffer[i]=='I'||buffer[i]=='O'||buffer[i]=='U'){

            }else{putChar(buffer[i]);
            }
            i++;
        }
        print("\n");
    }
    exit();
}

void loop(int argc,char argv[ARG_AMOUNT][ARG_SIZE]){
    int numero;
    numero = atoi(argv[0]);
    char string[4];
    int i=numToStr(get_pid_sys(),string,10);
    string[i]='\0';
    if(numero>0){
        while(1){
            print("hola soy ");
            print(string);
            print("\n");
            sleep(numero);
        }
    }
    else print("ERROR:No se inserto un numero\n");
    exit();
}


void loadCommand(void *(*f)(void *), char *name, char *desc,int argAmount,int procContext)
{
    shellCommands[cmdCounter].shellf = f;
    shellCommands[cmdCounter].name = name;
    shellCommands[cmdCounter].description = desc;
    shellCommands[cmdCounter].argAmount= argAmount;
    shellCommands[cmdCounter].procContext= procContext;
    cmdCounter++;
}





// retorna -1 si el buffer no tiene ningun comando valido
int cmdIndex(char *buf,char args[ARG_AMOUNT][ARG_SIZE])
{
    int spaceIndex=0;
    int i=0;
    char command[SHELL_BUFFER_SIZE];
    while(*(buf+i)!='\0' && !spaceIndex){      //Me fijo si hay espacios que separan la funcion de sus argumentos
        if(*(buf+i)==' '){
            spaceIndex=1;
        }else{
            command[i]=*(buf+i);        //Copio el comando hasta el espacio
            i++;
        }
    }
    command[i]='\0';
    int argsRead=0;
    int ceroFound=0;
    if(*(buf+i)!='\0'){
        for(int j=0;j<6 && !ceroFound ;j++){          //Copio los args
            i++;
            if(*(buf+i)!='\0' && *(buf+i)!=' '){
                int k=0;
                while(k<21-1 && *(buf+i)!='\0' && *(buf+i)!=' '){
                    args[argsRead][k]=*(buf+i);
                    i++;
                    k++;
                }
                if(*(buf+i)=='\0')
                    ceroFound=1;
                args[argsRead][k]='\0';
                argsRead++;     //Aumento la cantidad de argumentos leidos

            }
        }
    }
    
    for (int i = 0; i < cmdCounter; i++)
    {
        if (strcmp(command, shellCommands[i].name) == 0){
            if(argsRead!=shellCommands[i].argAmount)
                return -2;
            return i;
        }
    }
    return -1;
}


void sleep(int seconds)
{
    long _secondsElapsed = secondsElapsed();
    int finalTime = seconds + _secondsElapsed;
    while (secondsElapsed() <= finalTime)
        ;
}

void pSharedMemory(){
    char* memPoint=(char*)getSharedMemoryBlock(MEM_ID);
    char phrase[]="New shared memory\n";
    strcpy(memPoint,phrase);
    exit();
}

void rSharedMemory(){
    char* memPoint=(char*)getSharedMemoryBlock(MEM_ID);
    print(memPoint);
    exit();
}

void memState(int argc,char argv[ARG_AMOUNT][ARG_SIZE]){   
    mem_state();    
    exit();
}


void callKill(int argc,char argv[ARG_AMOUNT][ARG_SIZE]){
    kill(atoi(argv[0]));
    exit();
}

void callNice(int argc,char argv[ARG_AMOUNT][ARG_SIZE]){
    int i = nice(atoi(argv[0]), atoi(argv[1]));
    if(i==-1){
        printColor("Error al cambiar la prioridad del proceso\n", RED);
    }
    exit();
}

void changeState(int argc,char argv[ARG_AMOUNT][ARG_SIZE]){
    change_state(atoi(argv[0]), atoi(argv[1]));
    exit();
}

void getPipes(int argc,char argv[ARG_AMOUNT][ARG_SIZE]){
    get_pipes();
    exit();
}

void getAllProcesses(int argc,char argv[ARG_AMOUNT][ARG_SIZE]){
    get_all_processes();
    exit();
}
   

int theShell(int argc,char argv[ARG_AMOUNT][ARG_SIZE]){
    setupShellCommands();
    print("Bienvenido! \n");
    print("Por favor ingrese su usuario: ");
    char username[SHELL_BUFFER_SIZE] = {0};
    scanf(username);
    print("Para mas informacion escriba 'help'.");
    int i=0;
    char choose[SHELL_BUFFER_SIZE] = {0};
    while(i==0){
        print("\n>>");
        scanf(choose);
        char args[ARG_AMOUNT][ARG_SIZE];
        int verify = cmdIndex(choose,args);
        while(verify<0){
                if(verify==-1){
                    print("Comando incorrecto");
                }else{
                    print("Cantidad de argumentos incorrectos");
                }
                print("Por favor vuelva a intentarlo \n");
                print("\n>>");
                scanf(choose);
                verify = cmdIndex(choose,args);
        }
        int params[2]={shellCommands[verify].argAmount,shellCommands[verify].procContext};
        new_process(shellCommands[verify].shellf,args,params,shellCommands[verify].name);       //DEvuelve el pid
    }
    return 1;
}