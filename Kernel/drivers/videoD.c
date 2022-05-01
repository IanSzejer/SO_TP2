#include <videoD.h>


#define VIDEO_PTR ((uint8_t*)0xB8000-2) // apunta el lugar anterior a la primera posicion.
#define VALID_SCR_QTY(x) ((x) == 1 || (x) == 2 || (x) == 4 )
#define WIDTH 80
#define HEIGHT 25
#define H_WIDTH1 39
#define H_WIDTH2 40
#define H_HEIGHT 12
#define DEFAULT_COLOR 7

typedef struct {
	int position;
	uint8_t * ptr;
} VideoPTR;


static uint8_t * video = VIDEO_PTR;
static VideoPTR twoScreensAux[2] = {{0, 0}};
static VideoPTR fourScreensAux[4] = {{0, 0}};
static uint8_t * currentVideo = VIDEO_PTR;
static uint32_t width = 80;
static uint32_t height = 25;
static uint32_t screens = 1;
static uint32_t screen = 0;
static uint32_t shift = 0;
static uint32_t position = 0;
static uint32_t counterMax = 0;

#define NOT_INITIALIZED ((uint8_t*)1)

void ncInitVideoConsole() {

	ncSplitConsole(1,0);
}


int ncSplitConsole(int screenQty, int screenNum) {
	position = 0;
	twoScreensAux[0].ptr = twoScreensAux[1].ptr = fourScreensAux[0].ptr = fourScreensAux[1].ptr = fourScreensAux[2].ptr = fourScreensAux[3].ptr = (uint8_t *) NOT_INITIALIZED;

	if (!VALID_SCR_QTY(screenQty) || screenNum >= screenQty )
		return -1;
		screens = screenQty;
		screen = screenNum;
	if (screenQty == 1) {
		shift = 0;
		width = WIDTH;
		height = HEIGHT;
		video = VIDEO_PTR;
		currentVideo = video;
		counterMax = width*height;
		return 1;
	}
	else if (screenQty == 2) {
		shift = 0;
		width = WIDTH;
		height = H_HEIGHT;
		video = screenNum == 0 ? VIDEO_PTR : VIDEO_PTR + WIDTH*2*(H_HEIGHT+1);
		currentVideo = video;
	}
	else if (screenQty == 4) {
		width = (screenNum == 0 || screenNum == 2) ? H_WIDTH1 : H_WIDTH2;
		height = H_HEIGHT;
		shift = (screenNum == 0 || screenNum == 2) ? (H_WIDTH2+1)*2 : (H_WIDTH1+1)*2;
		video = (screenNum == 0 || screenNum == 1) ? VIDEO_PTR : VIDEO_PTR + WIDTH*2*(H_HEIGHT+1);
		video += (screenNum == 1 || screenNum == 3) ? shift : 0;
		currentVideo = video;
	}
	uint8_t * ptr;
	if (screenQty == 4) {
		ptr = VIDEO_PTR + 2 + (H_WIDTH1)*2;
		for(int i=0 ; i<HEIGHT ; i++) {
			(*ptr) = '|';
			ptr+=WIDTH*2;
		}
	}
	ptr = VIDEO_PTR + 2 + WIDTH*2*H_HEIGHT;
	for(int i=0 ; i<80 ; i++) {
		(*ptr) = '-';
		ptr+=2;
	}
	counterMax = width*height;
	return 1;
}

int advancePtr() {
	if (position >= width*height)
		return 0;

	if (position>1 && position % width == 0) {
		currentVideo += shift;
	}
	position++;
	currentVideo += 2;
	return 1;
}

// currentVideo apunta a la posicion ultima posicion escrita
// position apunta a la posicion del puntero dentro de la pantalla. Si es cero es porque no se escribio nada.

int retreatPtr() {
	if (position < 1)
		return 0;
	if (position == 1) {
		currentVideo = video;
		position = 0;
		return 1;
	}	

	position--;

	if (position % width == 0) {
		currentVideo -= shift;
	}
	
	currentVideo -= 2;
	return 1;
}

int ncChangeScreen(int screenNum) {
	if (screenNum >= screens)
		return -1;
	if (screenNum == screen)
		return 1;
	
	if (screens == 1) {
		return 1;
	}

	else if (screens == 2) {
		uint8_t * aux = currentVideo;
		int auxPos = position;
		shift = 0;
		width = WIDTH;
		height = H_HEIGHT;
		video = screenNum == 0 ? VIDEO_PTR : VIDEO_PTR + WIDTH*2*(H_HEIGHT+1);
		
		if (twoScreensAux[screenNum].ptr != NOT_INITIALIZED) {
			currentVideo = twoScreensAux[screenNum].ptr;
			position = twoScreensAux[screenNum].position;
		}
		else {
			currentVideo = video;
			position = 0;
		}
		twoScreensAux[screen].ptr = aux;
		twoScreensAux[screen].position = auxPos;
	}
	else if (screens == 4) {
		uint8_t * aux = currentVideo;
		int auxPos = position;
		width = (screenNum == 0 || screenNum == 2) ? H_WIDTH1 : H_WIDTH2;
		height = H_HEIGHT;
		shift = (screenNum == 0 || screenNum == 2) ? (H_WIDTH2+1)*2 : (H_WIDTH1+1)*2;
		video = (screenNum == 0 || screenNum == 1) ? VIDEO_PTR : VIDEO_PTR + WIDTH*2*(H_HEIGHT+1);
		video += (screenNum == 1 || screenNum == 3) ? shift : 0;
		if (fourScreensAux[screenNum].ptr != NOT_INITIALIZED) {
			currentVideo = fourScreensAux[screenNum].ptr;
			position = fourScreensAux[screenNum].position;
		}
		else {
			currentVideo = video;
			position = 0;
		}
		fourScreensAux[screen].ptr = aux;
		fourScreensAux[screen].position = auxPos;
	}
	screen = screenNum;
	return 1;
}

void ncPrint(const char * string)
{
	int i;

	for (i = 0; string[i] != 0; i++) {
		ncPrintChar(string[i],DEFAULT_COLOR);
	}
		
}

void ncPrintColor(const char * string, char color)
{
	int i;

	for (i = 0; string[i] != 0; i++) {
		ncPrintChar(string[i],color);
	}
		
}

void ncPrintChar(char character, char color)
{	if (!advancePtr())
		return;

	*currentVideo = character;
	*(currentVideo+1) = color;
}


void ncNewline()
{
	if (position >= counterMax)
		return;
	do
	{
		ncPrintChar(' ',DEFAULT_COLOR);
	}
	while((position) % (width) != 0);
}

void ncDelete()
{
	*currentVideo = ' ';

	retreatPtr();
	
	
}

void ncClearLine() {
	while (position > 0 && (position % width !=0 || position == height*width))
		ncDelete();
}



void ncClear()
{
	uint8_t * ptr = video+2;
	for(int i=0 ; i<height*width ; i++) {
		if (i % width  == 0 && i>0)
			ptr += shift;
		(*ptr) = ' ';
		*(ptr+1) = 7;
		ptr+=2;
	}
	position = 0;
	currentVideo = video;
}

