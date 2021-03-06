#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <time.h>
#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

using namespace std;

string get_dev_name(string map) {
  DIR *dp;
  struct dirent *ep;
  size_t map_length = strlen(map.c_str());
  string dev_name;
  string dirname = "/dev/";
  
  dp = opendir(dirname.c_str());
  if (dp != NULL) {
    while (ep = readdir (dp)) {
      dev_name = ep->d_name;
      dev_name = dev_name.substr(0, map_length);
      if (strcmp(dev_name.c_str(), map.c_str()) == 0) {
        dev_name = ep->d_name;
        dev_name = dirname + dev_name;
        closedir (dp);
        return dev_name;
      }
    }
  }
  return "";
}

int open_port() {
  
  string dev_name_str = get_dev_name("tty.usbmodem");
  const char* dev_name = dev_name_str.c_str();
  if (strlen(dev_name) == 0) {
    return -1;
  }
  
  int descr = open(dev_name, O_RDWR | O_NOCTTY | O_NDELAY);
  
  if (descr == -1) {
    printf("Unable to open serial port\n");
  } else {
    fcntl(descr, F_SETFL, 0);
  }
  
  return descr;
}

int configure_port(int descr) {
  
  struct termios port_settings;
  
  cfsetispeed(&port_settings, B9600);
  cfsetospeed(&port_settings, B9600);
  
  port_settings.c_cflag &= ~PARENB;
  port_settings.c_cflag &= ~CSTOPB;
  port_settings.c_cflag &= ~CSIZE;
  port_settings.c_cflag |= CS8;
}

int write_data(int descr, int color) {
  
  unsigned char bytes[] = { color };
  
  write(descr, bytes, 1);
  
  return 0;
}

int main(int argc, char* argv[]) {
  
  int descr = open_port();
  
  if (descr == -1) {
    return -1;
  }
  
  configure_port(descr);
  write_data(descr, atoi(argv[1]));
  write_data(descr, atoi(argv[2]));
  write_data(descr, atoi(argv[3]));
  
  return 0;
}