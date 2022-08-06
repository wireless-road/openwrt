#include "buttons.h"


static const char *const evval[3] = {
    "RELEASED",
    "PRESSED ",
    "REPEATED"
};


int buttons_init(void){
    int fd = -1;
    fd = open(INPUT_DEV, O_RDONLY);
    if (fd < 0)
    {
        printf("open buttons event failed.\n");
    } else {
        int flags = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }
    return fd;
}

int buttons_handler(int *fd){
    int ret = -1;
    struct input_event ev;
    memset(&ev, 0, sizeof(struct input_event));

    ret = read( *fd,
                &ev,
                sizeof(struct input_event));

    if (ret == (ssize_t)-1) {
        if (errno == EAGAIN || errno == EINTR) {
                return 0;
        }
    } else {
        if (ret != sizeof ev) {
            errno = EIO;
            return -1;
        }

        if (ev.type == EV_KEY && ev.value == 1) {
            switch (ev.code)
            {
                case BTN_0: // start 1
                    button_handler(ev.code);
                    break;
                case BTN_1:// stop 1
                    button_handler(ev.code);
                    break;
                case BTN_2: //start 2
                    button_handler(ev.code);
                    break;
                case BTN_3: // stop 2
                    button_handler(ev.code);
                    break;
                default: // non-keyboard event
                    break;
            }
        }
    }
    return 0;
}
