#include <sys/inotify.h>
#include <stdio.h>

#include <unistd.h>
       #include <errno.h>
       

int main()
{
int fd;

fd = inotify_init ();
if (fd < 0)
        perror ("inotify_init");
        
        int wd;
        
        wd = inotify_add_watch (fd,
                        "/home/koom/lemon/lemon2/l1",
                                        IN_MODIFY | IN_CREATE | IN_DELETE);
                                        
                                        if (wd < 0)
                                                perror ("inotify_add_watch");
                                                
/* size of the event structure, not counting name */
#define EVENT_SIZE  (sizeof (struct inotify_event))

/* reasonable guess as to size of 1024 events */
#define BUF_LEN        (1024 * (EVENT_SIZE + 16))

char buf[BUF_LEN];
int len, i = 0;

len = read (fd, buf, BUF_LEN);
if (len < 0) {
        if (errno == EINTR)
        	printf("            /* need to reissue system call */");
        else
                perror ("read");
} else if (!len)
        printf("/* BUF_LEN too small? */");
while (i < len) {
    struct inotify_event *event;
    event = (struct inotify_event *) &buf[i];
    printf ("wd=%d mask=%u cookie=%u len=%u\n",
    event->wd, event->mask,
    event->cookie, event->len);
    if (event->len)
        printf ("name=%s\n", event->name);
    i += EVENT_SIZE + event->len;
}
return 0;
}
