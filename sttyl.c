#include <stdio.h>
#include <stdlib.h>
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

struct charinfo { cc_t value; char *name; };
struct charinfo control_chars[] = {
    {VINTR    , "intr"   }, // intr
    {VQUIT    , "quit"   },
    {VERASE   , "erase"  }, // erase
    {VKILL    , "kill"   }, // kill
    {VEOF     , "eof"    },
    {VEOL     , "eol"    },
    {VEOL2    , "eol2"   },
    {VSWTC    , "swtch"  },
    {VSTART   , "start"  },
    {VSTOP    , "stop"   },
    {VSUSP    , "susp"   },
    {VREPRINT , "rprnt"  },
    {VWERASE  , "werase" },
    {VLNEXT   , "lnext"  },
    {VDISCARD , "discard"},
    {VMIN     , "min"    },
    {VTIME    , "time"   },
    {0        , NULL     }
};

struct flaginfo { tcflag_t value; char *name; };
struct flaginfo control_modes[] = {
    {PARENB  , "parenb" },
    {PARODD  , "parodd" },
    // {CMSPAR , "cmspar"  },
    {CS8     , "cs8"    },
    {HUPCL   , "hupcl"  },
    {CSTOPB  , "cstopb" },
    {CREAD   , "cread"  },
    {CLOCAL  , "clocal" },
    // {CRTSCTS , "crtscts"},
    {0       , NULL     }
};

struct flaginfo input_modes[] = {
    {IGNBRK , "ignbrk"},
    {BRKINT , "brkint"},
    {IGNPAR , "ignpar"},
    {PARMRK , "parmrk"},
    {INPCK  , "inpck" },
    {ISTRIP , "istrip"},
    {INLCR  , "inlcr" },
    {IGNCR  , "igncr" },
    {ICRNL  , "icrnl" },
    {IXON   , "ixon"  },
    {IXANY  , "ixany" },
    {IXOFF  , "ixoff" },
    {0	   , NULL     }
};

struct flaginfo output_modes[] = {
    {OPOST   , "opost"  },
    {OLCUC   , "olcuc"  },
    {OCRNL   , "ocrnl"  },
    {ONLCR   , "onlcr"  },
    {ONOCR   , "onocr"  },
    {ONLRET  , "onlret" },
    {IGNCR   , "igncr"  },
    {ICRNL   , "icrnl"  },
    {IXON    , "ixon"   },
    {IXOFF   , "ixoff"  },
    {IUCLC   , "iuclc"  },
    {IXANY   , "ixany"  },
    {IMAXBEL , "imaxbel"},
    {IUTF8   , "iutf8"  },
    {0       , NULL     }
};

struct flaginfo local_modes[] = {
    {ISIG    , "isig"   },
    {ICANON  , "icanon" },
    {IEXTEN  , "iexten" },
    {ECHO    , "echo"   },
    {ECHOE   , "echoe"  },
    {ECHOK   , "echok"  },
    {ECHONL  , "echonl" },
    {NOFLSH  , "noflsh" },
    // {XCASE  , "xcase"   },
    {TOSTOP  , "tostop" },
    // {ECHOPRT , "echoprt"},
    // {ECHOCTL , "echoctl"},
    // {ECHOKE  , "echoke" },
    // {FLUSHO  , "flusho" },
    // {EXTPROC , "extproc"},
    {0	   , NULL       }
};

void showBaud(speed_t);
void printModes(int, struct flaginfo []);
void printCntrlChars(cc_t *, struct charinfo []);
void printStty(struct termios info, struct winsize winfo);


int main (int argc, char **argv)
{
    struct termios info;
    struct winsize w;
	
	if ( tcgetattr( 0 , &info ) == -1 ) {
		perror( "cannot get params about stdin");
		exit(1);
	}

    if (argc == 1) {
        printStty(info, w);
    }
    
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

void printStty(struct termios stty_info, struct winsize w_info)
{
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w_info);
    // terminal info
	showBaud (cfgetospeed(&stty_info));
    printf("rows %d; ", w_info.ws_row);
    printf("columns %d; ", w_info.ws_col);
    printf("line = %d; ", stty_info.c_line);
    printf("\n");

    // control chars
    printCntrlChars(stty_info.c_cc, control_chars);

    // control modes
    printModes(stty_info.c_cflag, control_modes);

    // input modes
    printModes(stty_info.c_cflag, input_modes);

    // output modes
    printModes(stty_info.c_cflag, output_modes);

    // local modes
    printModes(stty_info.c_cflag, local_modes);
}

void printModes(int value, struct flaginfo modes[])
/*
    print flogs for the targeted mode and struct
*/
{
    for (int i = 0; modes[i].value; i++) {
        if (value & modes[i].value) {
            printf("%s ", modes[i].name);
        } else {
            printf("-%s ", modes[i].name);
        }
    }
    printf("\n");
}

void printCntrlChars(cc_t *val, struct charinfo chars[])
/*
    print flogs for the targeted chars and struct
*/
{
    // printf("%s = ^%c", chars[0].name, (char)val[chars[0].value]+64);
    for (int i = 0; chars[i].name != NULL; i++) {
        if (val[chars[i].value] < 2) {
            printf("%s = %d; ", chars[i].name, val[chars[i].value]);
        } else {
            printf("%s = ^%c; ", chars[i].name, (char)val[chars[i].value]+64);
        }
    }
    printf("\n");
}