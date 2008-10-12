#include <stdio.h>
#include <time.h>
#include <X11/Xlib.h>

int main() {
    // Xサーバーに接続
    Display *display;
    if ((display = XOpenDisplay(NULL)) == NULL) {
        fprintf(stderr, "Can't connect to X server !\n");
        return 1;
    }
    // ディスプレイの大きさを取得
    int screen_num = DefaultScreen(display);
    int width  = DisplayWidth(display, screen_num);
    int height = DisplayHeight(display, screen_num);
    for (;;) {
        // ルートウィンドウと、そのウィンドウ上でのポインタの状態を取得
        Window defaultRoot = DefaultRootWindow(display);
        Window root, child;
        int x, y, winX, winY;
        unsigned int mask;
        XQueryPointer(display, defaultRoot, &root, &child,
                      &x, &y, &winX, &winY, &mask);
        // モディファイアキーが押されていない時のみポインタを移動させる
        if (!(mask & (ShiftMask | ControlMask | Mod1Mask))) {
            // ポインタを移動
            if (x == 0) {
                XWarpPointer(display, None, root, 0, 0, width, height,
                             width - 2, y);
            } else if (x == (width - 1)) {
                XWarpPointer(display, None, root, 0, 0, width, height,
                             1, y);
            }
            if (y == 0) {
                XWarpPointer(display, None, root, 0, 0, width, height,
                             x, height - 2);
            } else if (y == (height - 1)) {
                XWarpPointer(display, None, root, 0, 0, width, height,
                             x, 1);
            }
        }
        // スリープ
        const struct timespec req = {
            .tv_sec  = 0,
            .tv_nsec = 1 * 1000 * 1000
        };
        nanosleep(&req, NULL);
    }
    // Xサーバーへの接続を閉じる
    XCloseDisplay(display);

    return 0;
}


