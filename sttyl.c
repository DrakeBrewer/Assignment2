#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>

void showBaud(speed_t);
void setAttr();
void getAttr();


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
*/
{
    printf("speed ");
    switch ( speed ) {
        case B300:   printf("300 baud; ");   break;
        case B600:   printf("600 baud; ");   break;
        case B1200:  printf("1200 baud; ");  break;
        case B1800:  printf("1800 baud; ");  break;
        case B2400:  printf("2400 baud; ");  break;
        case B4800:  printf("4800 baud; ");  break;
        case B9600:  printf("9600 baud; ");  break;
        case B38400: printf("38400 baud; "); break; // For the box that I'm currently using
        default:     printf("Fast baud; ");  break;
    }
}

void setAttr()
{

}

void getAttr()
{

}