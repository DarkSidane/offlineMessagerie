#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_MESSAGES 10
#define MESSAGE_LENGTH 100

Display *display;
Window window;
GC gc;
XFontStruct *font;
char messages[MAX_MESSAGES][MESSAGE_LENGTH];
int messageCount = 0;
char username[MESSAGE_LENGTH];

void drawMessageHistory() {
	XClearWindow(display, window);
	XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));

	int i;
	int y = 60;
	for (i = 0; i < messageCount; i++) {
		char message[MESSAGE_LENGTH + 20];
		char buf[MESSAGE_LENGTH + 20] = "";
		time_t now = time(NULL);
		struct tm *localTime = localtime(&now);
		strftime(message, sizeof(message),  "(%H:%M) : ", localTime);
		strcat(buf, username);
		strcat(buf, message);
		strcpy(message, buf);
		strcat(message, messages[i]);
		XDrawString(display, window, gc, 10, y, message, strlen(message));
		y += 20;
	}
}
void drawTextInputField(const char *input) {
	XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
	char buf[MESSAGE_LENGTH + 20] = "";
	strcat(buf, username);
	strcat(buf, " : ");
	XDrawString(display, window, gc, 10, 40, buf, strlen(buf));
	XDrawRectangle(display, window, gc, 120, 30, 250, 20);
	XSetForeground(display, gc, WhitePixel(display, DefaultScreen(display)));
	XFillRectangle(display, window, gc, 121, 31, 248, 18);
	XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
	XDrawString(display, window, gc, 130, 45, input, strlen(input));
}

void drawSendButton() {
    XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
    XDrawRectangle(display, window, gc, 380, 30, 70, 20);
    XDrawString(display, window, gc, 385, 45, "Envoyer", 7);
}
int main(int argc, char* argv[]) {
	strcpy(username, argv[1]);
	display = XOpenDisplay(NULL);
	if (display == NULL) {
		fprintf(stderr, "Cannot open display\n");
		return 1;
	}

	int screen = DefaultScreen(display);
	Window root = RootWindow(display, screen);
	unsigned int border_width = 1;
	unsigned int win_width = 500;
	unsigned int win_height = 300;
	unsigned int win_x = (DisplayWidth(display, screen) - win_width) / 2;
	unsigned int win_y = (DisplayHeight(display, screen) - win_height) / 2;

	window = XCreateSimpleWindow(display, root, win_x, win_y, win_width, win_height,
			border_width, BlackPixel(display, screen), WhitePixel(display, screen));
	XSelectInput(display, window, ExposureMask | KeyPressMask | ButtonPressMask);

	gc = XCreateGC(display, window, 0, 0);
	font = XLoadQueryFont(display, "fixed");
	XSetFont(display, gc, font->fid);

	Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(display, window, &wmDeleteMessage, 1);

	XMapWindow(display, window);

	XEvent event;
	char input[MESSAGE_LENGTH];
	int inputIndex = 0;

	while (1) {
		XNextEvent(display, &event);
		if (event.type == Expose) {
			drawMessageHistory();
			drawTextInputField(input);
			drawSendButton();
		} 
		// ...

		else if (event.type == KeyPress) {
			char keyString[32];
			int count = XLookupString(&event.xkey, keyString, sizeof(keyString), NULL, NULL);
			if (count == 1) {
				if (keyString[0] == '\r' || keyString[0] == '\n') {
					if (strlen(input) > 0) {
						strncpy(messages[messageCount], input, MESSAGE_LENGTH);
						messageCount = (messageCount + 1) % MAX_MESSAGES;
						drawMessageHistory();
						memset(input, 0, sizeof(input));
						inputIndex = 0;
						drawTextInputField(input);
						drawSendButton();
					}
				} else if (keyString[0] == 8 || keyString[0] == 127) {  // Backspace or Delete key
					if (inputIndex > 0) {
						inputIndex--;
						input[inputIndex] = '\0';
						drawTextInputField(input);
					}
				} else {
					input[inputIndex] = keyString[0];
					inputIndex++;
					if (inputIndex >= MESSAGE_LENGTH) {
						inputIndex = MESSAGE_LENGTH - 1;
					}
					input[inputIndex] = '\0';
					drawTextInputField(input);
				}
			}
		}

		// ...
		else if (event.type == ButtonPress) {
			if (event.xbutton.button == Button1) {
				int mouseX = event.xbutton.x;
				int mouseY = event.xbutton.y;
				if (mouseX >= 380 && mouseX <= 450 && mouseY >= 30 && mouseY <= 50) {
					if (strlen(input) > 0) {
						strncpy(messages[messageCount], input, MESSAGE_LENGTH);
						messageCount = (messageCount + 1) % MAX_MESSAGES;
						drawMessageHistory();
						memset(input, 0, sizeof(input));
						inputIndex = 0;
						drawTextInputField(input);
						drawSendButton();
					}
				}
			}
		} else if (event.type == ClientMessage) {
			if (event.xclient.data.l[0] == wmDeleteMessage) {
				break;
			}
		}
	}

	XUnloadFont(display, font->fid);
	XFreeGC(display, gc);
	XDestroyWindow(display, window);
	XCloseDisplay(display);

	return 0;
}

