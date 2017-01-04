/*  Fl_Pin_Table class
 *  Copyright 2013, Edmanuel Torres (eetorres@gmail.com)
 *  get the latest version from https://github.com/eetorres
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

#ifndef _FL_PIN_TABLE_H_
#define _FL_PIN_TABLE_H_

//#define _USE_SCOPES_ 1

#include <cstdio>
#include <cmath>
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Value_Output.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Slider.H>
#ifdef _USE_SCOPES_
#include "Fl_Scope.H"
#endif
#include <FL/Fl_Box.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Choice.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Scroll.H>
#include "CFirmata.h"


#define COLS 7
#define ROWS 80

const  std::string pin_modes[5] = { "Input", "Output", "Analog", "PWM", "Servo"};

#define PIN_NAME   0
#define PIN_CHOICE 1
#define PIN_INPUT  2
#define PIN_OUTPUT 3
#define PIN_ANALOG 4
#define PIN_PWM    5
#define PIN_SERVO  6

class Fl_Pin_Table : public Fl_Scroll, public CFirmata {

private:
  int rows, cols;
  int prow, pcol;
  int X,Y,W,H;
  int xx, yy;
  int cellw;
  int cellh;
  pin_t pint;
  // pin pointers
  void *pins[ROWS][COLS];
  Fl_Scroll *pin_scroll;
  int pin_mode[ROWS];
  int pin_number[ROWS];
  void which_pin(int, int);
  void eval_pin(void);
  //
  void pin_old_mode(int,uint8_t);
  void pin_new_mode(int,uint8_t);
  //
  inline void cb_eval_pin_i(Fl_Widget*, void*);
  static void cb_eval_pin(Fl_Widget*, void*);

protected:

  virtual int handle(int event);

public:

  Fl_Pin_Table(int x, int y, int w, int h, const char *l=0);
  ~Fl_Pin_Table() { }
  void add_pin(int, pin_t);
  void update_pin_value(int, pin_t);
  void get_next_data(void);
  void send_open_port(void);
  void send_close_port(void);
  /*
  static void on_idle(void *userdata) {
    Fl_Pin_Table *o = (Fl_Pin_Table*)userdata;
    o->get_next_data();
  }
  */
};

#endif

// END
