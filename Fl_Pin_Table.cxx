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
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Fl_Pin_Table.h"

//#define DEBUG 1

Fl_Pin_Table::Fl_Pin_Table(int x, int y, int w, int h, const char *l) : Fl_Scroll(x,y,w,h,l){
  X = x;
  Y = y;
  W = w;
  H = h;
  cellw = 80;
  cellh = 25;
  init_data();
}

inline void Fl_Pin_Table::cb_eval_pin_i(Fl_Widget* o, void* v) {
  eval_pin();
}

void Fl_Pin_Table::cb_eval_pin(Fl_Widget* o, void* data) {
  ((Fl_Pin_Table*)data)->cb_eval_pin_i(o,data);
}

void Fl_Pin_Table::add_pin(int pin, pin_t pin_info) {
  char buff[64];
  int mode=0;
  xx = X;
  yy = Y+rows*cellh;
  W = X+COLS*cellw;
  pin_number[rows]=pin;
  begin();
  {
#if DEBUG
    // create widgets
    printf(" Create widget\n");
#endif
    for( int c=0; c<COLS; c++ ){
      switch(c){
        case PIN_NAME:
        {
          Fl_Box *wpin = new Fl_Box(xx,yy,cellw,cellh);
          //pin->box(FL_NO_BOX);
          wpin->box(FL_GLEAM_DOWN_FRAME);
          sprintf(buff,"Pin %i",pin);
          wpin->copy_label(buff);
          pins[pin][PIN_NAME] = (void*)wpin;
          break;
        }
        case PIN_CHOICE:
        {
          Fl_Choice *wpin = new Fl_Choice(xx,yy,cellw,cellh);
          wpin->box(FL_GLEAM_DOWN_BOX);
          wpin->callback((Fl_Callback*)cb_eval_pin,(void*)this);
#if DEBUG
          printf(" pin mode is: %u\n",pin_info.mode);
#endif
          if(pin_info.supported_modes & (1<<MODE_INPUT)){
            wpin->add("Input");
            if (pin_info.mode == MODE_INPUT){
              mode=(int)wpin->size();
#if DEBUG
              printf(" mode is input: %i\n",mode);
#endif
              pin_mode[pin] = PIN_INPUT;
            }
          }
          if(pin_info.supported_modes & (1<<MODE_OUTPUT)){
            wpin->add("Output");
            if (pin_info.mode == MODE_OUTPUT){
              mode=(int)wpin->size();
#if DEBUG
              printf(" mode is output: %i\n",mode);
#endif
              pin_mode[pin] = PIN_OUTPUT;
            }
          }
          if(pin_info.supported_modes & (1<<MODE_ANALOG)){
            wpin->add("Analog");
            if (pin_info.mode == MODE_ANALOG){
              mode=(int)wpin->size();
#if DEBUG
              printf(" mode is analog: %i\n",mode);
#endif
              pin_mode[pin] = PIN_ANALOG;
            }
          }
          if(pin_info.supported_modes & (1<<MODE_PWM)){
            wpin->add("PWM");
            if (pin_info.mode == MODE_PWM){
              mode=(int)wpin->size();
#if DEBUG
              printf(" mode is pwm: %i\n",mode);
#endif
              pin_mode[pin] = PIN_PWM;
            }
          }
          if (pin_info.supported_modes & (1<<MODE_SERVO)){
            wpin->add("Servo");
            if (pin_info.mode == MODE_SERVO){
              mode=(int)wpin->size();
#if DEBUG
              printf(" mode is servo: %u\n",mode);
#endif
              pin_mode[pin] = PIN_SERVO;
            }
          }
          wpin->value(mode-2);
          pins[pin][PIN_CHOICE] = (void*)wpin;
          break;
        }
        case PIN_INPUT:
        {
          if (pin_info.supported_modes & (1<<MODE_INPUT)){
            Fl_Box *wpin = new Fl_Box(xx,yy,cellw,cellh);
            wpin->box(FL_GLEAM_THIN_DOWN_BOX);
            //in->box(FL_NO_BOX)
            wpin->copy_label("Low");
            if(c!=pin_mode[pin])
              wpin->deactivate();
            pins[pin][c] = (void*)wpin;
          }else{
            Fl_Box *wpin = new Fl_Box(xx,yy,cellw,cellh);
            wpin->box(FL_GLEAM_THIN_DOWN_BOX);
            wpin->deactivate();
            pins[pin][PIN_INPUT] = (void*)wpin;
          }
          break;
        }
        case PIN_OUTPUT:
        {
          if (pin_info.supported_modes & (1<<MODE_OUTPUT)){
          Fl_Light_Button *wpin = new Fl_Light_Button(xx,yy,cellw,cellh);
          wpin->box(FL_GLEAM_UP_BOX);
          wpin->callback((Fl_Callback*)cb_eval_pin,(void*)this);
          if(pin_info.value){
            wpin->label("High");
            wpin->value(1);
          }else{
            wpin->label("Low");
            wpin->value(0);
          }
          if(c!=pin_mode[pin])
            wpin->deactivate();
            pins[pin][c] = (void*)wpin;
          }else{
            Fl_Box *wpin = new Fl_Box(xx,yy,cellw,cellh);
            wpin->box(FL_GLEAM_THIN_DOWN_BOX);
            wpin->deactivate();
            pins[pin][PIN_OUTPUT] = (void*)wpin;
          }
          break;
        }
        case PIN_ANALOG:
        {
          if (pin_info.supported_modes & (1<<MODE_ANALOG)){
            Fl_Value_Output *wpin = new Fl_Value_Output(xx,yy,cellw,cellh);
            wpin->box(FL_GLEAM_THIN_DOWN_BOX);
            wpin->value(0.0);
            if(c!=pin_mode[pin])
              wpin->deactivate();
            pins[pin][c] = (void*)wpin;
          }else{
            Fl_Box *wpin = new Fl_Box(xx,yy,cellw,cellh);
            wpin->box(FL_GLEAM_THIN_DOWN_BOX);
            wpin->deactivate();
            pins[pin][PIN_ANALOG] = (void*)wpin;
          }
          break;
        }
        case PIN_PWM:
        {
          if (pin_info.supported_modes & (1<<MODE_PWM)){
            Fl_Slider *wpin = new Fl_Slider(xx,yy,cellw,cellh);
            wpin->type(FL_HORIZONTAL);
            wpin->box(FL_GLEAM_DOWN_BOX);
            wpin->callback((Fl_Callback*)cb_eval_pin,(void*)this);
            wpin->bounds(0,255);
            wpin->value(0.0);
            if(c!=pin_mode[pin])
              wpin->deactivate();
            pins[pin][c] = (void*)wpin;
          }else{
            Fl_Box *wpin = new Fl_Box(xx,yy,cellw,cellh);
            wpin->box(FL_GLEAM_THIN_DOWN_BOX);
            wpin->deactivate();
            pins[pin][PIN_PWM] = (void*)wpin;
          }
          break;
        }
        case PIN_SERVO:
        {
          if (pin_info.supported_modes & (1<<MODE_SERVO)){
            Fl_Slider *wpin = new Fl_Slider(xx,yy,cellw,cellh);
            wpin->type(FL_HOR_NICE_SLIDER);
            wpin->box(FL_GLEAM_DOWN_BOX);
            wpin->callback((Fl_Callback*)cb_eval_pin,(void*)this);
            wpin->bounds(0,180);
            wpin->value(0.0);
            if(c!=pin_mode[pin])
              wpin->deactivate();
            pins[pin][c] = (void*)wpin;
          }else{
            Fl_Box *wpin = new Fl_Box(xx,yy,cellw,cellh);
            wpin->box(FL_GLEAM_THIN_DOWN_BOX);
            wpin->deactivate();
            pins[pin][PIN_SERVO] = (void*)wpin;
          }
          break;
        }
      }
      xx += cellw;
    }
  }
  end();
  redraw();
  rows++;
}

void Fl_Pin_Table::update_pin_value(int pin, pin_t pin_info) {
  if(pin_info.mode== MODE_INPUT){
    Fl_Box *wpin = (Fl_Box*)pins[pin][PIN_INPUT];
    if(pin_info.value)
      wpin->copy_label("High");
    else
      wpin->copy_label("Low");
  }else if(pin_info.mode== MODE_ANALOG){
    Fl_Value_Output *wpin = (Fl_Value_Output*)pins[pin][PIN_ANALOG];
    wpin->value(pin_info.value);
  }
}

// bbox ( int &  X, int &  Y, int &  W, int &  H)
void Fl_Pin_Table::which_pin(int x, int y) {
  int xpos = xposition();
  int ypos = yposition();
  int xs = ceil(xpos/cellw);
  int ys = ceil(ypos/cellh);
  if(Fl::event_inside(X, Y, W, H)){
    // Scan rows
    for (int row = 0; row < rows; row++) {
      int tempY = Y+row*cellh-ypos%cellh;
      // Scan colums
      for (int column =0; column < COLS; column++) {
        int tempX = X+column*cellw-xpos%cellw;
        if (Fl::event_inside(tempX,tempY,cellw,cellh)){
          pcol = column+xs;
          prow = row+ys;
        }
      }
    }
  }
}

// handles incoming events.
int Fl_Pin_Table::handle(int event) {
  int ret;
  switch (event) {
     // PUSH mouse right click event
     case FL_PUSH:
     case FL_DRAG:
     case FL_RELEASE:
       // Find the clicked row and column
       which_pin(Fl::event_x(), Fl::event_y());
       eval_pin();
       break;
  }
  ret = Fl_Group::handle(event);
  return ret;
}

void Fl_Pin_Table::eval_pin(void){
#if DEBUG
  printf("[eval pin]\n");
#endif
  int pin;
  char *chm;
  if ((prow>=0 && prow<rows) && (pcol>0 && pcol<COLS)) {
    switch(pcol){
    case PIN_CHOICE:  // PIN_CHOICE
    {
      //printf("widget found at (%d,%d)\n",prow,pcol);
      pin = pin_number[prow];
      pint = get_pin_info(pin);
      Fl_Choice *wpin = (Fl_Choice*)pins[pin][PIN_CHOICE];
      uint8_t newmode=0;
      chm = (char*)wpin->text((int)wpin->value());
      if(!strcmp(chm,pin_modes[0].c_str())){
        newmode = MODE_INPUT;
      }else if(!strcmp(chm,pin_modes[1].c_str())){
        newmode = MODE_OUTPUT;
      }else if(!strcmp(chm,pin_modes[2].c_str())){
        newmode = MODE_ANALOG;
      }else if(!strcmp(chm,pin_modes[3].c_str())){
        newmode = MODE_PWM;
      }else if(!strcmp(chm,pin_modes[4].c_str())){
        newmode = MODE_SERVO;
      }
#if DEBUG
      printf(" choice text = %s\n",chm);
      printf(" output pin %d value = %d\n",pin,newmode);
#endif
      if(pint.mode != newmode){
#if DEBUG
        printf(" send the mode change message\n");
#endif
        pin_old_mode(pin,pint.mode);
        pin_new_mode(pin,newmode);
        set_pin_mode(pin,newmode);
      }
      break;
    }
    case PIN_OUTPUT: // MODE_OUTPUT
#if DEBUG
      printf("widget found at (%d,%d)\n",prow,pcol);
#endif
      pin = pin_number[prow];
      pint = get_pin_info(pin);
      if(pint.mode == MODE_OUTPUT){
        Fl_Light_Button *wpin = (Fl_Light_Button*)pins[pin][PIN_OUTPUT];
#if DEBUG
        printf(" output pin %d value = %d\n",pin,(int)wpin->value());
#endif
        if((int)wpin->value()){
          wpin->label("High");
        }else{
          wpin->label("Low");
        }
        set_toggle_button(pin,(uint32_t)wpin->value());
      }
      break;
    case PIN_PWM: // MODE_PWM
#if DEBUG
      printf("widget found at (%d,%d)\n",prow,pcol);
#endif
      pin = pin_number[prow];
      pint = get_pin_info(pin);
      if(pint.mode == MODE_PWM){
        Fl_Slider *wpin = (Fl_Slider*)pins[pin][PIN_PWM];
#if DEBUG
        printf(" pwm pin %d value = %d\n",pin,(int)wpin->value());
#endif
        set_slider_drag(pin,(int)wpin->value());
      }
      break;
    case PIN_SERVO: // MODE_SERVO
#if DEBUG
      printf("widget found at (%d,%d)\n",prow,pcol);
#endif
      pin = pin_number[prow];
      pint = get_pin_info(pin);
      if(pint.mode == MODE_SERVO){
        Fl_Slider *wpin = (Fl_Slider*)pins[pin][PIN_SERVO];
#if DEBUG
        printf(" servo pin %d value = %d\n",pin,(int)wpin->value());
#endif
        set_slider_drag(pin,(int)wpin->value());
      }
      break;
    }
  }
}

void Fl_Pin_Table::pin_old_mode(int pin, uint8_t oldmode){
#if DEBUG
  printf("[old mode]\n");
#endif
  if(oldmode == MODE_INPUT){
    Fl_Box *wpin = (Fl_Box*)pins[pin][PIN_INPUT];
    wpin->copy_label("Low");
#if DEBUG
    printf(" input pin %d value = %s\n",pin,(char*)wpin->label());
#endif
    wpin->deactivate();
  }else if(oldmode == MODE_OUTPUT){
    Fl_Light_Button *wpin = (Fl_Light_Button*)pins[pin][PIN_OUTPUT];
    wpin->label("Low");
    wpin->value(0);
#if DEBUG
    printf(" output pin %d value = %d\n",pin,(int)wpin->value());
#endif
    wpin->deactivate();
  }else if(oldmode == MODE_ANALOG){
    Fl_Value_Output *wpin = (Fl_Value_Output*)pins[pin][PIN_ANALOG];
#if DEBUG
    printf(" analog pin %d value = %d\n",pin,(int)wpin->value());
#endif
    wpin->deactivate();
  }else if(oldmode == MODE_PWM){
    Fl_Slider *wpin = (Fl_Slider*)pins[pin][PIN_PWM];
    wpin->value(0.0);
#if DEBUG
    printf(" pwm pin %d value = %d\n",pin,(int)wpin->value());
#endif
    wpin->deactivate();
  }else if(oldmode == MODE_SERVO){
    Fl_Slider *wpin = (Fl_Slider*)pins[pin][PIN_SERVO];
    wpin->value(0.0);
#if DEBUG
    printf(" servo pin %d value = %d\n",pin,(int)wpin->value());
#endif
    wpin->deactivate();
  }
}

void Fl_Pin_Table::pin_new_mode(int pin, uint8_t newmode){
#if DEBUG
  printf("[new mode]\n");
#endif
  if(newmode == MODE_INPUT){
    Fl_Box *wpin = (Fl_Box*)pins[pin][PIN_INPUT];
#if DEBUG
    printf(" input pin %d value = %s\n",pin,(char*)wpin->label());
#endif
    wpin->activate();
  }else if(newmode == MODE_OUTPUT){
    Fl_Light_Button *wpin = (Fl_Light_Button*)pins[pin][PIN_OUTPUT];
#if DEBUG
    printf(" output pin %d value = %d\n",pin,(int)wpin->value());
#endif
    wpin->activate();
  }else if(newmode == MODE_ANALOG){
    Fl_Value_Output *wpin = (Fl_Value_Output*)pins[pin][PIN_ANALOG];
#if DEBUG
    printf(" analog pin %d value = %d\n",pin,(int)wpin->value());
#endif
    wpin->activate();
  }else if(newmode == MODE_PWM){
    Fl_Slider *wpin = (Fl_Slider*)pins[pin][PIN_PWM];
#if DEBUG
    printf(" pwm pin %d value = %d\n",pin,(int)wpin->value());
#endif
    wpin->activate();
  }else if(newmode == MODE_SERVO){
    Fl_Slider *wpin = (Fl_Slider*)pins[pin][PIN_SERVO];
#if DEBUG
    printf(" servo pin %d value = %d\n",pin,(int)wpin->value());
#endif
    wpin->activate();
  }
}

void Fl_Pin_Table::send_open_port(void){
  if(!is_port_open()){
    init_data();
    open_port();
    //if(is_port_open())
      //Fl::add_idle(on_idle,(void*)this);
  }
}

void Fl_Pin_Table::get_next_data(void){
  if(port.Is_open()) {
    parse_event();
  }
}

void Fl_Pin_Table::send_close_port(void){
  if(port.Is_open()) {
    close_port();
  }
  //Fl::remove_idle(on_idle,(void*)this);
  clear();
  for(int r=0; r<rows; r++){
    for(int c=0; c<COLS; c++){
      pins[r][c]=0;
    }
  }
  redraw();
  rows=0;
}

// END