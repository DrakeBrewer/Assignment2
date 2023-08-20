#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
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
void setAttr();


int main (int argc, int **argv)
{
    struct termios info;

    					
	if ( tcgetattr( 0 , &info ) == -1 ) {
		perror( "cannot get params about stdin");
		exit(1);
	}
	showBaud (cfgetospeed(&info));

}

void showBaud(speed_t speed)
/*
    get the baud rate for the port
    I found a few lists online for the most common baud rates.
*/
{
    printf("speed ");
    switch (speed) {
        case B110:    printf("110 ");    break;
        case B300:    printf("300 ");    break;
        case B600:    printf("600 ");    break;
        case B1200:   printf("1200 ");   break;
        case B2400:   printf("2400 ");   break;
        case B4800:   printf("4800 ");   break;
        case B9600:   printf("9600 ");   break;
        case B19200:  printf("19200 ");  break;
        case B38400:  printf("38400 ");  break;
        case B57600:  printf("57600 ");  break;
        case B115200: printf("115200 "); break;
        default:      printf("Fast ");   break;
    }
    printf("baud; ");
}

void getAttr(int fd)
{

}

void setAttr()
{

}