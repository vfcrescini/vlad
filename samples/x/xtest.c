#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

GC getGC(Display *d, Window w, int s) {
	XGCValues tempValues;
	GC tempGC;

	tempGC = XCreateGC(d, w, 0, &tempValues);
	XSetForeground(d, tempGC, WhitePixel(d, s));
	XSetBackground(d, tempGC, BlackPixel(d, s));
	XSetFont(d, tempGC, XLoadFont(d, "fixed"));
	
	return tempGC;
}

void loadFont(Display *d) {
	char **fontList;
	int fontCount;
	Font fontID;
	int i;
	
	fontList = XListFonts(d, "*", 1000, &fontCount);

	for (i = 0; i < fontCount; i++)
		printf("%s\n", fontList[i]);

	XFreeFontNames(fontList);

	fontID = XLoadFont(d, "fixed");

	printf("%lu\n", fontID);
}

int generatePixmap(Display *d, int s, unsigned int *width, unsigned int *height, Pixmap *pix) {
	int hotx;
	int hoty;
	
	return XReadBitmapFile(d, RootWindow(d, s), "/local/home/vino/tests/c/image.bmp", width, height, pix, &hotx, &hoty);
}

void drawStuff(Display *d, Window w, GC g, int width, int height) {
	XDrawRectangle(d, w, g, width / 4, height / 4, width / 2, height / 2); 
	XFillRectangle(d, w, g,  3 * width / 8, 3 * height / 8, width / 4, height / 4);
	XDrawString(d, w, g, width / 8, height / 8, "hello", 5);
}

int main() {
	Window mWindow;
	Display *mDisplay;
	XEvent mEvent;
	int mScreen;
	GC mGC;

	unsigned int pixWidth;
	unsigned int pixHeight;
	Pixmap pixMap;

	int currentWidth = 0;
	int currentHeight = 0;


	if ((mDisplay = XOpenDisplay(NULL)) == NULL)
		return -1;

//	loadFont(mDisplay);

	mScreen = DefaultScreen(mDisplay);

	mWindow = XCreateSimpleWindow(mDisplay, RootWindow(mDisplay, mScreen), 0, 0, 500, 250, 5, WhitePixel(mDisplay, mScreen), BlackPixel(mDisplay, mScreen));

	XSelectInput(mDisplay, mWindow, ExposureMask | KeyPressMask | StructureNotifyMask);

	mGC = getGC(mDisplay, mWindow, mScreen);

	XMapWindow(mDisplay, mWindow);

	drawStuff(mDisplay, mWindow, mGC, 100, 100);

//	XFlush(mDisplay);
//	while(1);

	while(1) {
		XNextEvent(mDisplay, &mEvent);
		switch(mEvent.type) {
			case Expose :
				
				drawStuff(mDisplay, mWindow, mGC, currentWidth, currentHeight);
/*				
				if (!generatePixmap(mDisplay, mScreen, &pixWidth, &pixHeight, &pixMap))
					XCopyPlane(mDisplay, pixMap, mWindow, mGC, 0, 0, pixWidth, pixHeight, 10, 10, 1);
				else
					printf("cannot open bitmap file\n");
*/				
				break;
			case ConfigureNotify :
				currentWidth = mEvent.xconfigure.width;
				currentHeight = mEvent.xconfigure.height;
				break;
			case KeyPress:
				XCloseDisplay(mDisplay);
				exit(0);
			default:
				break;
		}
	}
	return 0;
}
