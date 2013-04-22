/*  Firmata class
 *  Copyright 2013, Edmanuel Torres (eetorres@gmail.com)
 *
 *  this is a  port of the Firmata Test Program by
 *  Paul Stoffregen (paul@pjrc.com) to the FLTK GUI.
 *  https://github.com/firmata/firmata_test.git
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _FL_FIRMATA_H_
#define _FL_FIRMATA_H_

#include"serial.h"
#include<iostream>

typedef struct {
  uint8_t mode;
  uint8_t analog_channel;
  uint64_t supported_modes;
  uint32_t value;
} pin_t;

#define MODE_INPUT    0x00
#define MODE_OUTPUT   0x01
#define MODE_ANALOG   0x02
#define MODE_PWM      0x03
#define MODE_SERVO    0x04
#define MODE_SHIFT    0x05
#define MODE_I2C      0x06

#define START_SYSEX             0xF0 // start a MIDI Sysex message
#define END_SYSEX               0xF7 // end a MIDI Sysex message
#define PIN_MODE_QUERY          0x72 // ask for current and supported pin modes
#define PIN_MODE_RESPONSE       0x73 // reply with current and supported pin modes
#define PIN_STATE_QUERY         0x6D
#define PIN_STATE_RESPONSE      0x6E
#define CAPABILITY_QUERY        0x6B
#define CAPABILITY_RESPONSE     0x6C
#define ANALOG_MAPPING_QUERY    0x69
#define ANALOG_MAPPING_RESPONSE 0x6A
#define REPORT_FIRMWARE         0x79 // report name and version of the firmware

class CFirmata{

private:

  int parse_count;
  int parse_command_len;
  uint8_t parse_buf[4096];
  //
  std::string status;
  std::string port_name;
  std::vector<std::string> port_list;
  //
  pin_t pin_info[128];
  std::string firmata_name;
  std::string status_text;
  unsigned int rx_count, tx_count;
  //
  void parse(const uint8_t *buf, int len);
  void do_message(void);

public:

  Serial port;

  CFirmata();
  ~CFirmata();

  void init_data(void);
  void open_port(void);
  void close_port(void);
  void update_status(void);
  //
  void add_item_to_grid(int row, int col, void *item);
  //
  void set_port_name(int);
  //
  std::string get_port_name(void);
  std::vector<std::string> get_port_list(void);
  pin_t get_pin_info(int);
  //
  void set_toggle_button(int,int);
  void set_slider_drag(int,int);
  void set_pin_mode(int,uint8_t);
  //
  std::string get_status(void);
  bool is_port_open(void);
  void parse_event(void);
  virtual void add_pin(int,pin_t)=0;
  virtual void update_pin_value(int,pin_t)=0;

protected:

};

#endif