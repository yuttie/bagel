#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>

int main() {
    // Xサーバーに接続
    Display *display;
    if ((display = XOpenDisplay(NULL)) == NULL) {
        fprintf(stderr, "Can't connect to X server!\n");
        exit(EXIT_FAILURE);
    }

    // XInput Extension available?
    int opcode, first_event, first_error;
    if (!XQueryExtension(display, "XInputExtension", &opcode, &first_event, &first_error)) {
        fprintf(stderr, "X Input extension not available.\n");
        exit(EXIT_FAILURE);
    }

    // Which version of XI2? We support 2.0
    int major = 2, minor = 0;
    if (XIQueryVersion(display, &major, &minor) == BadRequest) {
        fprintf(stderr, "XI2 not available. Server supports %d.%d\n", major, minor);
        exit(EXIT_FAILURE);
    }

    // 単一スクリーンのみサポート
    if (ScreenCount(display) != 1) {
        fprintf(stderr, "No support for multiple screens!\n");
        exit(EXIT_FAILURE);
    }

    // Select events
    Window root_window = DefaultRootWindow(display);
    XIEventMask event_mask;
    unsigned char mask[1] = {0};
    event_mask.deviceid = XIAllDevices;
    event_mask.mask_len = sizeof(mask);
    event_mask.mask = mask;
    XISetMask(mask, XI_Motion);
    XISelectEvents(display, root_window, &event_mask, 1);

    // イベントループ
    for (;;) {
        XEvent event;
        XGenericEventCookie* cookie = &event.xcookie;
        XNextEvent(display, &event);

        if (XGetEventData(display, cookie) &&
            cookie->type == GenericEvent &&
            cookie->extension == opcode)
        {
            XIDeviceEvent* event = cookie->data;

            switch(cookie->evtype) {
            case XI_Motion: {
                // モディファイアキーが押されていない時のみポインタを移動させる
                if (event->mods.base == 0) {
                    const int screenNum = DefaultScreen(display);
                    const int width  = DisplayWidth(display, screenNum);
                    const int height = DisplayHeight(display, screenNum);
                    const int min_x = 0;
                    const int max_x = width - 1;
                    const int min_y = 0;
                    const int max_y = height - 1;

                    const double new_x = event->root_x <= min_x ? max_x - 1
                                       : event->root_x >= max_x ? min_x + 1
                                       : event->root_x;
                    const double new_y = event->root_y <= min_y ? max_y - 1
                                       : event->root_y >= max_y ? min_y + 1
                                       : event->root_y;

                    // ポインタを移動
                    if (new_x != event->root_x || new_y != event->root_y) {
                        // Get the device ID of the master pointer
                        int num_devices;
                        XIDeviceInfo* device = XIQueryDevice(display, event->deviceid, &num_devices);
                        assert(num_devices == 1);
                        int deviceid = event->deviceid;
                        if (device->use == XIMasterPointer) {
                            deviceid = device->deviceid;
                        }
                        else if (device->use == XISlavePointer) {
                            deviceid = device->attachment;
                        }
                        XIFreeDeviceInfo(device);

                        XIWarpPointer(display,
                                      deviceid,
                                      None, root_window,
                                      0, 0, (unsigned int)width, (unsigned int)height,
                                      new_x, new_y);
                    }
                }
                break;
            }
            default:
                assert(false);
            }
        }
        XFreeEventData(display, cookie);
    }

    // Xサーバーへの接続を閉じる
    XCloseDisplay(display);

    return 0;
}


