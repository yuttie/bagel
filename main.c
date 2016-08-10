#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>

int main() {
    // Xサーバーに接続
    Display *display;
    if ((display = XOpenDisplay(NULL)) == NULL) {
        fprintf(stderr, "Can't connect to X server!\n");
        exit(EXIT_FAILURE);
    }

    // 単一スクリーンのみサポート
    if (ScreenCount(display) != 1) {
        fprintf(stderr, "No support for multiple screens!\n");
        exit(EXIT_FAILURE);
    }

    // Select events
    Window rootWindow = DefaultRootWindow(display);
    XSelectInput(display, rootWindow, PointerMotionMask);

    // イベントループ
    XEvent event;
    for (;;) {
        XNextEvent(display, &event);

        // モディファイアキーが押されていない時のみポインタを移動させる
        if (!(event.xmotion.state & (ShiftMask | ControlMask | Mod1Mask))) {
            const int screenNum = DefaultScreen(display);
            const int width  = DisplayWidth(display, screenNum);
            const int height = DisplayHeight(display, screenNum);

            const int x = event.xmotion.x_root;
            const int y = event.xmotion.y_root;
            int newX = x;
            int newY = y;

            if      (x == 0)             newX = width - 2;
            else if (x == (width - 1))   newX = 1;

            if      (y == 0)             newY = height - 2;
            else if (y == (height - 1))  newY = 1;

            // ポインタを移動
            if (newX != x || newY != y) {
                XWarpPointer(display,
                             None, rootWindow,
                             0, 0, width, height,
                             newX, newY);
            }
        }
    }

    // Xサーバーへの接続を閉じる
    XCloseDisplay(display);

    return 0;
}


