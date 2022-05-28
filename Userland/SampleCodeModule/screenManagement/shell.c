#include "shell.h"
#include "stdinout.h"
#include <phylo.h>

typedef struct
{
    char line[SHELLW];
    int isCmd;
} shell_line;

typedef struct
{
    void (*shellf)();
    char *name;
    char *description;
} t_shellc;

int cmdIndex(char *buf);
void processCommands(char *buf, shell_line shellBuffer[SHELLH], int lines);
void loadCommand(void (*f)(), char *name, char *desc);
void copyOneLineUp(shell_line shellBuffer[SHELLH]);
void copyCommandDescriptor(char *buf, t_shellc cmd);
void copyLinesToShellOutput(char lines[][SHELLW], int qty);
void loop(char seconds);
void cat();
void wc();
void filter();



static char buffer1[32] = {0};
static char buffer2[32] = {0};
int buf1Size = 0;
int buf2Size = 0;
static char consoleMsg1[80] = {0};
static char consoleMsg2[80] = {0};
char *consoleMsg = consoleMsg1;
static uint8_t *const video = (uint8_t *)0xB8000;
static const uint32_t width = 80;
static const uint32_t height = 25;
static int cmdCounter = 0;
int currentShell = 0;
static int flag = 1;

static shell_line shellBuffer1[SHELLH];
static shell_line shellBuffer2[SHELLH];

static int buffer1Lines = 0;
static int buffer2Lines = 0;

static t_shellc shellCommands[10] = {{0, 0, 0}};

void printShell(char *buffer, shell_line shellBuffer[SHELLH])
{
    for (int i = 0; i < SHELLH; i++)
    {
        if (shellBuffer[i].line[0] == 0)
            putChar('\n');
        else
        {
            if (shellBuffer[i].isCmd)
                printColor(SHELL_MSG, SHELL_COLOR);
            print(shellBuffer[i].line);
            putChar('\n');
        }
    }
    print(consoleMsg);
    putChar('\n');

    printColor(SHELL_MSG, SHELL_COLOR);
    print(buffer);
}

void updateConsoleMsg(char *s)
{
    strcpy(consoleMsg, s);
}

void printDateTime()
{
    char buf[20] = {0};
    get_date(buf);
    buf[8] = buf[10] = ' ';
    buf[9] = '-';
    get_time(&buf[11]);
    updateConsoleMsg(buf);
}

void help()
{

    char lines[cmdCounter][SHELLW];

    for (int i = 0; i < cmdCounter; i++)
    {
        copyCommandDescriptor(lines[i], shellCommands[i]);
    }
    copyLinesToShellOutput(lines, cmdCounter);
}

void inforeg()
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
}

void divideByZero()
{
    int a = 5, b = 0, c;
    c = a / b;
    c++;
}

void invalidOpCode()
{
    __asm__("UD2"); // inline assembler. Usamos la instruccion UD2 que arroja la excepcion.
}

// Acepta valores decimales y tambien hexadecimales con el prefijo '0x'
void printmem(char *dirString)
{
    if (!dirString)
    {
        updateConsoleMsg("address argument needed");
        return;
    }
    char *splitted[2] = {0};
    uint8_t *ptr;
    split(dirString, 'x', splitted);
    if (splitted[1])
    {
        char *hexaAddress = splitted[1];
        ptr = (uint8_t *)(long)atoi_base(hexaAddress, 16);
    }
    else
    {
        ptr = (uint8_t *)(long)atoi_base(splitted[0], 10);
    }
    char output[2][SHELLW];
    char aux[10] = {"0x"};
    int idx = 0;
    output[0][0] = output[1][0] = 0;
    for (int i = 0; i < 32; i++)
    {
        if (i == 16)
            idx = 1;
        itoa(ptr[i], aux + 2, 16);
        int len = strcat(output[idx], aux);
        if (i != 15 && i != 31)
        {
            output[idx][len] = ' ';
            output[idx][len + 1] = 0;
        }
    }
    copyLinesToShellOutput(output, 2);
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
    shell_line *shell = currentShell == 0 ? shellBuffer1 : shellBuffer2;
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
    }
}

void setupShellCommands()
{
    loadCommand(&printDateTime, "datetime", "Displays the date and time");
    loadCommand(&help, "help", "Shows a list of available commands");
    loadCommand(&inforeg, "inforeg", "Shows the value of all registers");
    loadCommand(&printmem, "printmem", "Prints 32 bytes of memory from arg. address");
    loadCommand(&divideByZero, "exception0", "Executes rutine that generates \"division by zero\" exception");
    loadCommand(&invalidOpCode, "exception6", "Executes rutine that generates \"invalid op. code\" exception");
    loadCommand(&loop,"loop","prints a message with a delay inputed by user");
    loadCommand(&cat,"cat", "prints what its received");
    loadCommand(&wc,"wc", "counts the amount of lines inputed ");
    loadCommand(&filter,"filter", "prints what its received,excluding vocals");
    loadCommand(&phylo,"philosophers","philosopher problem");
    loadCommand(&memState,"mem","see memory status");
    loadCommand(&kill,"kill","kill a process");
    loadCommand(&nice,"nice","change a process priority");
    loadCommand(&changeState,"block","block a process");
    loadCommand(&getPipes,"pipe","see pipes status");
    loadCommand(&getAllProcesses,"ps","see processes status");

}

void cat(){
    char ascii;
    while((ascii=getCharSys())>0)
        print(&ascii);
    
    
}

void wc(){
    char buffer[100];
    int count;
    
    while(system_read(STDIN,buffer,100)>0)
        count++;
    char num[1];
    num[0]=intToChar(count);
    print(num);
}
void filter(){
    char buffer[100];
    while(system_read(STDIN,buffer,100)>0)
    {
        int i=0;
        while(buffer[i]){
            if (buffer[i]=='a'||buffer[i]=='e'||buffer[i]=='i'||buffer[i]=='o'||buffer[i]=='u'||buffer[i]=='A'||buffer[i]=='E'||buffer[i]=='I'||buffer[i]=='O'||buffer[i]=='U'){

            }else{putChar(buffer[i]);
            }
        }
    }
}
void loop(char seconds){
    int sec;
    char string[1];
    string[0]=intToChar(getPid);
    if(charToDigit(seconds)>=0)
    while(1){
        print("hola soy ");
        print(string);
        print("\n");
   //     sleep(sec);
    }
    else print("ERROR:No se inserto un numero");

}


void loadCommand(void (*f)(), char *name, char *desc)
{
    shellCommands[cmdCounter].shellf = f;
    shellCommands[cmdCounter].name = name;
    shellCommands[cmdCounter].description = desc;
    cmdCounter++;
}

void processCommands(char *buf, shell_line shellBuffer[SHELLH], int lines)
{
    for (int i = 1; i < SHELLH; i++)
    {
        strcpy(shellBuffer[i - 1].line, shellBuffer[i].line);
        shellBuffer[i - 1].isCmd = shellBuffer[i].isCmd;
    }

    strcpy(shellBuffer[SHELLH - 1].line, buf);
    shellBuffer[SHELLH - 1].isCmd = 1;
    char *splitted[8] = {0};
    split(buf, ' ', splitted);
    int idx = cmdIndex(splitted[0]);
    buf[0] = 0;
    if (idx < 0)
    {
        updateConsoleMsg("Comando invalido");
        return;
    }
    shellCommands[idx].shellf(splitted[1], splitted[2]);
}

void cleanBuffers()
{
    for (int i = 0; i < SHELLH; i++)
        shellBuffer1[i].line[0] = 0;
    for (int i = 0; i < SHELLH; i++)
        shellBuffer2[i].line[0] = 0;
    consoleMsg1[0] = consoleMsg2[0] = 0;
}

void manageConsole(shell_line bufferIn[SHELLH], char *buf, int *bufSize, char *consoleMsgC, int bufferLines)
{
    consoleMsg = consoleMsgC;
    console_clear();
    printShell(buf, bufferIn);
    if (readInput(buf, bufSize, SHELL_MSG, GREEN, 30))
    {
        return;
    }
    flag = 1;
    processCommands(buf, bufferIn, bufferLines);
    // console_clear();
    // printShell(bufferIn);
}

int theShell()
{
    cleanBuffers();
    setupShellCommands();
    // set_kb_target(&kb);
    console_clear();
    split_screen(2, 0);
    while (1)
    {
        set_screen(currentShell);
        switch (currentShell)
        {
        case 0:
            manageConsole(shellBuffer1, buffer1, &buf1Size, consoleMsg1, buffer1Lines);
            break;
        case 1:
            manageConsole(shellBuffer2, buffer2, &buf2Size, consoleMsg2, buffer2Lines);
            break;
        }
    }
}

int readInput(char *buffer, int *size, char *def, char color, int maxSize)
{
    int begin = 1, c;

    while ((*size) < (maxSize - 1) && ((c = getRealChar()) != '\n') && (c != UP_ARROW) && (c != DOWN_ARROW))
    {
        // if (!PRINTABLE(c))
        //     continue;
        if (begin)
        {
            clear_line();
            printColor(def, color);
            print(buffer);
            begin = 0;
        }

        if (c != '\b' && PRINTABLE(c))
        {
            putChar(c);
            buffer[(*size)++] = c;
        }
        else if (c == '\b' && (*size) > 0) // solo borro si escribi algo
        {
            putChar('\b');
            (*size)--;
        }
        c = 0;
    }
    if (c == UP_ARROW || c == DOWN_ARROW)
    {
        currentShell = currentShell == 0 ? 1 : 0;
        buffer[(*size)] = 0;
        return c - 500;
    }
    buffer[(*size)] = 0;
    putChar('\n');
    (*size) = 0;
    return 0;
}

// retorna -1 si el buffer no tiene ningun comando valido
int cmdIndex(char *buf)
{

    for (int i = 0; i < cmdCounter; i++)
    {
        if (strcmp(buf, shellCommands[i].name) == 0)
            return i;
    }
    return -1;
}


void sleep(int seconds)
{
    int secondsElapsed = _secondsElapsed();
    int finalTime = seconds + secondsElapsed;
    while (_secondsElapsed() <= finalTime)
        ;
}