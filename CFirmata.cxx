/*  Firmata class
 *  Copyright 2013, Edmanuel Torres (eetorres@gmail.com)
 *
 *  this is a port of the Firmata Test Program by
 *  Paul Stoffregen (paul@pjrc.com) for the FLTK GUI.
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

#include "CFirmata.h"

#define DEBUG 1

CFirmata::CFirmata(){
  port.Set_baud(57600);
}

CFirmata::~CFirmata(){
  // the port is closed
  if(port.Is_open()){
    port.Close();//<------------------
#if DEBUG
    printf("port is closed\n");
#endif
  }
}

void CFirmata::init_data(void){
  port.Set_baud(57600);
  for(int i=0; i < 128; i++) {
    pin_info[i].mode = 255;
    pin_info[i].analog_channel = 127;
    pin_info[i].supported_modes = 0;
    pin_info[i].value = 0;
  }
  tx_count = rx_count = 0;
  firmata_name = "";
  update_status();
#if DEBUG_CONTROLS
  // For testing only, add many controls so something
  // appears in the window without requiring any
  // actual communication...
  for (int i=0; i<20; i++) {
    pin_info[i].supported_modes = 7;
    add_pin(i,pin_info[i]);
  }
#endif
}

void CFirmata::open_port(void){
  port.Close();
  //printf("OnPort, id = %d, name = %s\n", id, (const char *)port_name);
  //printf("OnPort, name = %s\n", (const char *)port_name.c_str());
  port.Open(port_name);
  port.Set_baud(57600);

  if (port.Is_open()) {
#if DEBUG
    printf("port is open\n");
#endif
    firmata_name = "";
    rx_count = tx_count = 0;
    parse_count = 0;
    parse_command_len = 0;
    update_status();
    uint8_t buf[3];
    buf[0] = START_SYSEX;
    buf[1] = REPORT_FIRMWARE; // read firmata name & version
    buf[2] = END_SYSEX;
    port.Write(buf, 3);
    tx_count += 3;
    parse_event();
  } else {
    printf("error opening port\n");
  }
  update_status();
#if DEBUG_PORT
  // the port is close only when debuging
  if (port.Is_open()) {
    port.Close();//<------------------
    printf("port is closed\n");
  }
#endif
}

void CFirmata::close_port(void){
  port.Close();//<------------------
#if DEBUG_PORT
  if (!port.Is_open()) {
    printf("port is closed\n");
  }
#endif
  update_status();
}

void CFirmata::parse_event(void){
  uint8_t buf[1024];
  int r;
//#if DEBUG
//  printf("Idle event\n");
//#endif
  r = port.Input_wait(40);
  if (r > 0) {
    r = port.Read(buf, sizeof(buf));
    if (r < 0) {
      // error
      return;
    }
    if (r > 0) {
      // parse
      rx_count += r;
      /*/for (int i=0; i < r; i++) {
        printf("%02X ", buf[i]);
      }
      printf("\n");*/
      parse(buf, r); // <----------
      update_status();
    }
  } else if (r < 0) {
    return;
  }
}

void CFirmata::parse(const uint8_t *buf, int len){
  const uint8_t *p, *end;
  p = buf;
  end = p + len;
  for (p = buf; p < end; p++) {
    uint8_t msn = *p & 0xF0;
    if (msn == 0xE0 || msn == 0x90 || *p == 0xF9) {
      parse_command_len = 3;
      parse_count = 0;
    } else if (msn == 0xC0 || msn == 0xD0) {
      parse_command_len = 2;
      parse_count = 0;
    } else if (*p == START_SYSEX) {
      parse_count = 0;
      parse_command_len = sizeof(parse_buf);
    } else if (*p == END_SYSEX) {
      parse_command_len = parse_count + 1;
    } else if (*p & 0x80) {
      parse_command_len = 1;
      parse_count = 0;
    }
    if (parse_count < (int)sizeof(parse_buf)) {
      parse_buf[parse_count++] = *p;
    }
    if (parse_count == parse_command_len) {
      do_message();
      parse_count = parse_command_len = 0;
    }
  }
}

void CFirmata::do_message(void){
  uint8_t cmd = (parse_buf[0] & 0xF0);
  //printf("message, %d bytes, %02X\n", parse_count, parse_buf[0]);
  if (cmd == 0xE0 && parse_count == 3) {
    int analog_ch = (parse_buf[0] & 0x0F);
    int analog_val = parse_buf[1] | (parse_buf[2] << 7);
    for (int pin=0; pin<128; pin++) {
      if (pin_info[pin].analog_channel == analog_ch) {
        pin_info[pin].value = analog_val;
        //printf("pin %d is A%d = %d\n", pin, analog_ch, analog_val);
        update_pin_value(pin,pin_info[pin]);
        return;
      }
    }
    return;
  }
  if (cmd == 0x90 && parse_count == 3) {
    int port_num = (parse_buf[0] & 0x0F);
    int port_val = parse_buf[1] | (parse_buf[2] << 7);
    int pin = port_num * 8;
    //printf("port_num = %d, pin = %d, port_val = %d\n", port_num, pin, port_val);
    for (int mask=1; mask & 0xFF; mask <<= 1, pin++) {
      if (pin_info[pin].mode == MODE_INPUT) {
        uint32_t val = (port_val & mask) ? 1 : 0;
        if (pin_info[pin].value != val) {
          //printf("pin %d is %d\n", pin, val);
          pin_info[pin].value = val;
          update_pin_value(pin,pin_info[pin]);
        }
      }
    }
    return;
  }

  if (parse_buf[0] == START_SYSEX && parse_buf[parse_count-1] == END_SYSEX) {
    // Sysex message
    if (parse_buf[1] == REPORT_FIRMWARE) {
      char name[140];
      int len=0;
      for (int i=4; i < parse_count-2; i+=2) {
        name[len++] = (parse_buf[i] & 0x7F) | ((parse_buf[i+1] & 0x7F) << 7);
      }
      name[len++] = '-';
      name[len++] = parse_buf[2] + '0';
      name[len++] = '.';
      name[len++] = parse_buf[3] + '0';
      name[len++] = 0;
      firmata_name = name;
      // query the board's capabilities only after hearing the
      // REPORT_FIRMWARE message.  For boards that reset when
      // the port open (eg, Arduino with reset=DTR), they are
      // not ready to communicate for some time, so the only
      // way to reliably query their capabilities is to wait
      // until the REPORT_FIRMWARE message is heard.
      uint8_t buf[80];
      len=0;
      buf[len++] = START_SYSEX;
      buf[len++] = ANALOG_MAPPING_QUERY; // read analog to pin # info
      buf[len++] = END_SYSEX;
      buf[len++] = START_SYSEX;
      buf[len++] = CAPABILITY_QUERY; // read capabilities
      buf[len++] = END_SYSEX;
      for (int i=0; i<16; i++) {
        buf[len++] = 0xC0 | i;  // report analog
        buf[len++] = 1;
        buf[len++] = 0xD0 | i;  // report digital
        buf[len++] = 1;
      }
      port.Write(buf, len);
      tx_count += len;
    } else if (parse_buf[1] == CAPABILITY_RESPONSE) {
      int pin, i, n;
      for (pin=0; pin < 128; pin++) {
        pin_info[pin].supported_modes = 0;
      }
      for (i=2, n=0, pin=0; i<parse_count; i++) {
        if (parse_buf[i] == 127) {
          pin++;
          n = 0;
          continue;
        }
        if (n == 0) {
          // first byte is supported mode
          pin_info[pin].supported_modes |= (1<<parse_buf[i]);
        }
        n = n ^ 1;
      }
      // send a state query for for every pin with any modes
      for (pin=0; pin < 128; pin++) {
        uint8_t buf[512];
        int len=0;
        if (pin_info[pin].supported_modes) {
          buf[len++] = START_SYSEX;
          buf[len++] = PIN_STATE_QUERY;
          buf[len++] = pin;
          buf[len++] = END_SYSEX;
        }
        port.Write(buf, len);
        tx_count += len;
      }
    } else if (parse_buf[1] == ANALOG_MAPPING_RESPONSE) {
      int pin=0;
      for (int i=2; i<parse_count-1; i++) {
        pin_info[pin].analog_channel = parse_buf[i];
        pin++;
      }
      return;
    } else if (parse_buf[1] == PIN_STATE_RESPONSE && parse_count >= 6) {
      int pin = parse_buf[2];
      pin_info[pin].mode = parse_buf[3];
      pin_info[pin].value = parse_buf[4];
      if (parse_count > 6) pin_info[pin].value |= (parse_buf[5] << 7);
      if (parse_count > 7) pin_info[pin].value |= (parse_buf[6] << 14);
      //printf("add pin\n");
      add_pin(pin,pin_info[pin]);// <------ virtual
    }
    return;
  }
}

void CFirmata::update_status(void){
  char ch_buff[254];
  //std::string status;
  std::string port_name;
  if(port.Is_open()) {
    port_name=port.get_name();
    sprintf(ch_buff,"%s %s Tx:%u Rx:%u",port_name.c_str(),firmata_name.c_str(),tx_count,rx_count);
    status = ch_buff;
  }else{
    status = "Please choose serial port";
  }
  status_text = status;
}

void CFirmata::set_port_name(int i){
  port_name = port.get_port_name(i);
#if DEBUG
  printf(" Port Name: %s\n",port_name.c_str());
#endif
}

std::string CFirmata::get_port_name(void){
  return port.get_name();
}

std::vector<std::string> CFirmata::get_port_list(void){
  return port.port_list();
}

pin_t CFirmata::get_pin_info(int i){
  return pin_info[i];
}

void CFirmata::set_toggle_button(int n, int val){
  int pin = n;
  if (pin < 0 || pin > 127) return;
  //printf("Toggle Button, id = %d, pin=%d, val=%d\n", id, pin, val);
  pin_info[pin].value = val;
  int port_num = pin / 8;
  int port_val = 0;
  for (int i=0; i<8; i++) {
    int p = port_num * 8 + i;
    if (pin_info[p].mode == MODE_OUTPUT || pin_info[p].mode == MODE_INPUT) {
      if (pin_info[p].value) {
        port_val |= (1<<i);
      }
    }
  }
  uint8_t buf[3];
  buf[0] = 0x90 | port_num;
  buf[1] = port_val & 0x7F;
  buf[2] = (port_val >> 7) & 0x7F;
  port.Write(buf, 3);
  tx_count += 3;
  update_status();
}

void CFirmata::set_slider_drag(int n, int val){
  int pin = n;
  if (pin < 0 || pin > 127) return;
  if (pin <= 15 && val <= 16383) {
    //printf("Slider Drag, pin=%d, val=%d\n", pin, val);
    uint8_t buf[3];
    buf[0] = 0xE0 | pin;
    buf[1] = val & 0x7F;
    buf[2] = (val >> 7) & 0x7F;
    port.Write(buf, 3);
    tx_count += 3;
  } else {
    //printf("Slider Drag, pin=%d, val=%d\n", pin, val);
    uint8_t buf[12];
    int len=4;
    buf[0] = 0xF0;
    buf[1] = 0x6F;
    buf[2] = pin;
    buf[3] = val & 0x7F;
    if (val > 0x00000080) buf[len++] = (val >> 7) & 0x7F;
    if (val > 0x00004000) buf[len++] = (val >> 14) & 0x7F;
    if (val > 0x00200000) buf[len++] = (val >> 21) & 0x7F;
    if (val > 0x10000000) buf[len++] = (val >> 28) & 0x7F;
    buf[len++] = 0xF7;
    port.Write(buf, len);
    tx_count += len;
  }
  update_status();
}

void CFirmata::set_pin_mode(int pin, uint8_t mode){
  // send the mode change message
  uint8_t buf[4];
  buf[0] = 0xF4;
  buf[1] = pin;
  buf[2] = mode;
  port.Write(buf, 3);
  tx_count += 3;
  pin_info[pin].mode = mode;
  pin_info[pin].value = 0;
}

std::string CFirmata::get_status(void){
  return status;
}

bool CFirmata::is_port_open(void){
  return port.Is_open();
}

// END