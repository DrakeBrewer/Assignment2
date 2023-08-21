#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>


/*
    #### Must have attributes:
        - intr
        - erase
        - kill

    #### expected output - "sttyl":
        Speed 9600 baud; line = 0;
        eol = M-^?; eol2 = M-^?;
        ixany iutf8;

    #### expected output - "sttyl -a":
        Speed 9600 baud; rows 14; columns 184; line = 0;
        intr = ^C; quit = ^\; erase = ^?; kill = ^U; eof = ^D; eol = M-^?; eol2 = M-^?; swtch = <undef>; start = ^Q; stop = ^S; susp = ^Z; rprnt = ^R; werase = ^W; lnext = ^V; discard = ^O; min = 1; time = 0;
        -parenb -parodd -cmspar cs8 hupcl -cstopb cread -clocal -crtscts
        -ignbrk brkint -ignpar -parmrk -inpck -istrip -inlcr -igncr icrnl ixon -ixoff -iuclc ixany imaxbel iutf8
        opost -olcuc -ocrnl onlcr -onocr -onlret -ofill -ofdel nl0 cr0 tab0 bs0 vt0 ff0
        isig icanon iexten echo echoe echok -echonl -noflsh -xcase -tostop -echoprt echoctl echoke -flusho -extproc
*/

void showBaud(speed_t);
void getAttr(int);
void getControlChars(cc_t, char[20]);
void printInputMode(struct termios info, tcflag_t);
void setAttr();
char* lowerCase(char*);


int main (int argc, int **argv)
{
    struct termios info;
    struct winsize w;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    					
	if ( tcgetattr( 0 , &info ) == -1 ) {
		perror( "cannot get params about stdin");
		exit(1);
	}
    // terminal info
	showBaud (cfgetospeed(&info));
    printf("rows %d; ", w.ws_row);
    printf("columns %d; ", w.ws_col);
    printf("line = %d; ", info.c_line);

    // control chars
    getControlChars(info.c_cc[VINTR], "intr"); // intr
    getControlChars(info.c_cc[VQUIT], "quit");
    getControlChars(info.c_cc[VERASE], "erase"); // erase
    getControlChars(info.c_cc[VKILL], "kill"); // kill
    getControlChars(info.c_cc[VEOF], "eof");
    getControlChars(info.c_cc[VEOL], "eol");
    getControlChars(info.c_cc[VEOL2], "eol2");
    getControlChars(info.c_cc[VSWTC], "swtch");
    getControlChars(info.c_cc[VSTART], "start"); //
    getControlChars(info.c_cc[VSTOP], "stop"); //
    getControlChars(info.c_cc[VSUSP], "susp");
    getControlChars(info.c_cc[VREPRINT], "rprnt");
    getControlChars(info.c_cc[VWERASE], "werase"); //
    getControlChars(info.c_cc[VLNEXT], "lnext");
    getControlChars(info.c_cc[VDISCARD], "discard");
    getControlChars(info.c_cc[VMIN], "min"); //
    getControlChars(info.c_cc[VTIME], "time"); //

    // input modes
    printInputMode(info, IGNBRK);

    // output modes


    // local modes

}

void showBaud(speed_t speed)
/*
    Show the baud rate
*/
{
    printf("speed ");
    switch (speed) {
        case B0:      printf("0 ");       break;
        case B50:     printf("50 ");      break;
        case B75:     printf("75 ");      break;
        case B110:    printf("110 ");     break;
        case B134:    printf("134 ");     break;
        case B150:    printf("150 ");     break;
        case B200:    printf("200 ");     break;
        case B300:    printf("300 ");     break;
        case B600:    printf("600 ");     break;
        case B1200:   printf("1200 ");    break;
        case B1800:   printf("1800 ");    break;
        case B2400:   printf("2400 ");    break;
        case B4800:   printf("4800 ");    break;
        case B9600:   printf("9600 ");    break;
        case B19200:  printf("19200 ");   break;
        case B38400:  printf("38400 ");   break;
        case B57600:  printf("57600 ");   break;
        case B115200: printf("115200 ");  break;
        case B230400: printf("230400 ");  break;
        default:      printf("Unknown "); break;
    }
    printf("baud; ");
}

void getControlChars(cc_t cc, char name[20])
{
    if (cc < 2) {
        printf("%s = %d; ", name, cc);
    } else if (cc < 32)
        printf("%s = ^%c; ", name, (char)cc+64);
        // printf("%s = ^%c(%d); ", name, (char)cc+64, cc); // For testing
    else {
        printf("%s = <undef>; ", name);
    }
}

void printInputMode(struct termios info, tcflag_t inFlag) {
    info.c_iflag & inFlag ? printf("%s ") : printf("-%s ", getName(inFlag));
}

struct charinfo { cc_t fl_value; char *fl_name };

struct flaginfo { tcflag_t fl_value; char	*fl_name; };
struct flaginfo input_flags[] = {
    IGNBRK , "ignbrk",
    BRKINT , "brkint",
    IGNPAR , "ignpar",
    PARMRK , "parmrk",
    INPCK  , "inpck",
    ISTRIP , "istrip",
    INLCR  , "inlcr",
    IGNCR  , "igncr",
    ICRNL  , "icrnl",
    IXON   , "ixon",
    /* _IXANY  ,	"enable any char to restart output",	*/
    IXOFF  , "ixoff",
    0	   , NULL 
};

struct flaginfo output_flags[] = {
    OPOST   , "opost",
    OLCUC   , "olcuc",
    OCRNL   , "ocrnl",
    ONLCR   , "onlcr",
    ONOCR   , "onocr",
    ONLRET  , "onlret",
    IGNCR   , "igncr",
    ICRNL   , "icrnl",
    IXON    , "ixon",
    IXOFF   , "ixoff",
    IUCLC   , "iuclc",
    IXANY   , "ixany",
    IMAXBEL , "imaxbel",
    IUTF8   , "iutf8",
    0       , NULL
};

struct flaginfo local_flags[] = {
    ISIG   , "isig",
    ICANON , "icanon",
    /* _XCASE	,	"Canonical upper/lower appearance", */
    ECHO   , "echo",
    ECHOE  , "echoe",
    ECHOK  , "echok",
    0	   , NULL 
};