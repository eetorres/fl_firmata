/*  Fl_Firmata_Test
 *  Copyright 2013, Edmanuel Torres (eetorres@gmail.com)
 *  get the latest version from https://github.com/eetorres
 *
 *  this program is a port of the Firmata Test Program by
 *  Paul Stoffregen (paul@pjrc.com) to the FLTK GUI.
 *  http://www.pjrc.com/teensy/firmata_test/
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
#include "fl_firmata.h"

int main(void){
  fl_firmata *firmata = new fl_firmata();
  firmata->show();
  Fl::run();
  return 0;
}
