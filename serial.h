
#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <stdint.h>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <string.h>
#include <string>


#define LINUX 1

#if defined(LINUX)
#include <termios.h>
#elif defined(MACOSX)
#include <termios.h>
#elif defined(WINDOWS)
#include <windows.h>
#endif

class Serial{

public:

  Serial();
  ~Serial();
  std::string error_message(void);
  std::vector<std::string> port_list(void);
  std::string get_port_name(int);
  int Open(const std::string& name);
  int Set_baud(int baud);
  int Set_baud(const std::string& baud_str);
  int Read(void *ptr, int count);
  int Write(const void *ptr, int len);
  int Input_wait(int msec);
  void Input_discard(void);
  int Set_control(int dtr, int rts);
  void Output_flush(void);
  void Close(void);
  bool Is_open(void);
  std::string get_name(void);

private:
  std::vector<std::string> list;
  bool port_is_open;
  std::string port_name;
  int baud_rate;
  std::string error_msg;

private:
#if defined(LINUX) || defined(MACOSX)
  int port_fd;
  struct termios settings_orig;
  struct termios settings;
#elif defined(WINDOWS)
  HANDLE port_handle;
  COMMCONFIG port_cfg_orig;
  COMMCONFIG port_cfg;
#endif
};

#endif // _SERIAL_H_
