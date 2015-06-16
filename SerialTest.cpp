#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <wiringPi.h>
#include <wiringSerial.h>
int main ()
{
  int fd;
  unsigned char data=3;


  if ((fd = serialOpen ("/dev/ttyAMA0", 9600))<0)
  {
	fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno));
	return 1;
  }


  printf ("\nRaspberry Pi UART Test \n");
  
  while(1)
  {

	  serialPutchar(fd,data);
//	  printf ("\nPC > RPi = %c", (char)data);
  
//	  serialPutchar(fd, data);
//	  serialPuts(fd, "\n");
	  fflush(stdout);
  }
  

  return 0 ;
}
