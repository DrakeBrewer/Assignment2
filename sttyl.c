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
struct baudinfo { speed_t value; char *name; };
struct baudinfo baud_rates[] = {
    { B0     , "0"       },
    { B50    , "50"      },
    { B75    , "75"      },
    { B110   , "110"     },
    { B134   , "134"     },
    { B150   , "150"     },
    { B200   , "200"     },
    { B300   , "300"     },
    { B600   , "600"     },
    { B1200  , "1200"    },
    { B1800  , "1800"    },
    { B2400  , "2400"    },
    { B4800  , "4800"    },
    { B9600  , "9600"    },
    { B19200 , "19200"   },
    { B38400 , "38400"   },
    { B57600 , "57600"   },
    { B115200, "115200"  },
    { B230400, "230400"  },
    { 0      , NULL      }
};

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
    {IXOFF  , "ixoff" },
    {IXANY  , "ixany" },
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

struct flaginfo *flag_arrays[] = {
    control_modes,
    input_modes,
    output_modes,
    local_modes,
    NULL // End marker
};

void showBaud(speed_t);
void printModes(int, struct flaginfo []);
void printCntrlChars(cc_t *, struct charinfo []);
void printStty(struct termios info, struct winsize winfo);
void modifyAttr(char*, struct termios *info, int);
void modifyChar(char**, int);

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
    } else {
        // loop through all entered arguments
        for (int ii = 1; ii < argc; ii++) {
           if (argv[ii][0] == '-') {
                modifyAttr(argv[ii], &info, 0);
                
            } else {
                int foundCC = 0;
                // check if any of the arg matches any of the control chars
                for (int jj = 0; control_chars[jj].name; jj++) {
                    if (strcmp(argv[ii], control_chars[jj].name) == 0) {
                        foundCC = 1;

                        // Check if user manually entered control char
                        if ((int)argv[ii+1][0] == '^') {
                            info.c_cc[control_chars[jj].value] = (int)argv[ii+1][1]-'@';
                        }

                        // check if user entered a control char
                        else if ((int)*argv[ii+1] >= 0 && (int)*argv[ii+1] < 32) {
                            info.c_cc[control_chars[jj].value] = (int)*argv[ii+1];
                        }

                        // check if user entered a number
                        else if ((int)*argv[ii+1] > 47 && (int)*argv[ii+1] < 58) {
                            // info.c_lflag &= ~ICANON;
                            info.c_cc[control_chars[jj].value] = atoi(argv[ii+1]);

                        }

                        // handle other entries
                        else {
                            info.c_cc[control_chars[jj].value] = (int)*argv[ii+1];
                        }
                        printf("%c\n", argv[ii+1]);
                        ii++;
                    }
                }
                if (!foundCC){
                    // no control chars so check for attributes
                    modifyAttr(argv[ii], &info, 1);
                }
            }
        }
        if ( tcsetattr( 0 , TCSANOW, &info ) == -1 ) {
            perror( "cannot get params about stdin");
            exit(2);
        }
    }
    return 0;
}

void showBaud(speed_t speed)
/*
    Show the baud rate
*/
{
    int found = 0;
    printf("speed ");
    for (int ii = 0; baud_rates[ii].name; ii++) {
        if (baud_rates[ii].value == speed) {
            found = 1;
            printf("%s baud; ", baud_rates[ii].name);
            break;
        }
    }
    if (!found) {
        printf("unkown baud; ");
    }
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
    printModes(stty_info.c_iflag, input_modes);

    // output modes
    printModes(stty_info.c_oflag, output_modes);

    // local modes
    printModes(stty_info.c_lflag, local_modes);
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
    for (int i = 0; chars[i].name != NULL; i++) {
        if (chars[i].value == VMIN || chars[i].value == VTIME) {
            printf("%s = %d; ", chars[i].name, val[chars[i].value]);
        } 
        else if (val[chars[i].value] < 32) {
            printf("%s = ^%c; ", chars[i].name, val[chars[i].value]+'@');
        } 
        else if (val[chars[i].value] < 128) {
            printf("%s = %c; ", chars[i].name, val[chars[i].value]);
        }
        else {
            printf("%s = <undef>; ", chars[i].name);
        }
    }
    printf("\n");
}

void modifyAttr(char *attr, struct termios *stty_info, int enable)
{
    int found = 0;
    char *newAttr = attr + 1;

    for (int ii = 0; flag_arrays[ii]; ii++) {
        struct flaginfo *modes = flag_arrays[ii];
        for (int jj = 0; modes[jj].name; jj++) {
            if (enable) {
            // Enable attribute
                if (strcmp(attr, modes[jj].name) == 0) {
                    found = 1;
                    switch (jj) {
                        case 0: stty_info->c_cflag |= modes[jj].value; break; // control modes on
                        case 1: stty_info->c_iflag |= modes[jj].value; break; // input modes on
                        case 2: stty_info->c_oflag |= modes[jj].value; break; // output modes on
                        case 3: stty_info->c_lflag |= modes[jj].value; break; // local modes on
                        default: break;
                    }
                }
            } else {
            // Disable attribute
                if (strcmp(newAttr, modes[jj].name) == 0) {
                    found = 1;
                    switch (jj) {
                        case 0: stty_info->c_cflag &= ~modes[jj].value; break; // control modes off
                        case 1: stty_info->c_iflag &= ~modes[jj].value; break; // input modes off
                        case 2: stty_info->c_oflag &= ~modes[jj].value; break; // output modes off
                        case 3: stty_info->c_lflag &= ~modes[jj].value; break; // local modes off
                        default: break;
                    }
                }
            }
        }
        if (found) {
            break;
        }
    }
    if (!found) {
        printf("sttyl: invalid argument \'%s\'\n", attr);
        exit(3);
    }
}