/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU CC; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 * 
 * Copyright (C) 2002 Vino Crescini
 *
 */

import java.awt.*;

public class vcalcdisplay extends Panel {
  private double fvalue = 0;
  private double buffer1 = 0;
  private double buffer2 = 0;
  private char op = '\0';
  private int state = 0;
  private TextField tdisplay = new TextField("0");

  public vcalcdisplay() {
    setLayout(new BorderLayout());
    add(tdisplay, "North");
  }

  public void pressed_ce() {
    fvalue = 0;
    updatedisplay();
  }	

  public void pressed_c() {
    state = 0;
    op = '\0';
    fvalue = 0;
    buffer1 = 0;
    buffer2 = 0;
    updatedisplay();
  }

  public void pressed_num(int n) {
    if (n >= 0 && n <= 9) {
      switch(state) {
        case 0 :
          fvalue *= 10;
          fvalue += n;
          updatedisplay();
          break;
        case 1 :
          buffer1 = fvalue;
          fvalue = n;
          state = 2;
          updatedisplay();
          break;
        case 2 :
          fvalue *= 10;
          fvalue += n;
          updatedisplay();
          break;
        case 3 :
          fvalue = n;
          buffer1 = 0;
          buffer2 = 0;
          op = '\0';
          state = 0;
          updatedisplay();
          break;
      }
    } 
  }

  public void pressed_op(char newop) {
    if (newop == '+' || newop == '-' || newop == '*' || newop == '/') {
      switch(state) {
        case 0 :
          op = newop;
          state = 1;
          break;
        case 1 :
          op = newop;
          break;
        case 2 :
          buffer2 = fvalue;
          fvalue = calculate(op, buffer1, buffer2);
          buffer1 = fvalue;
          op = newop;
          state = 1;
          updatedisplay();
          break;
        case 3 :
          op = newop;
          state = 1;
          break;
      }
    }
  }

  public void pressed_equals() {
    switch(state) {
      case 0 :
        break;
      case 1 :
        switch(op) {
          case '+' :
          case '-' :
            buffer1 = 0;
            buffer2 = fvalue;
            break;
          case '*' :
            buffer1 = fvalue;
            buffer2 = fvalue;
            break;
          case '/' :
            buffer1 = 1;
            buffer2 = fvalue;
            break;
        }
        fvalue = calculate(op, buffer1, buffer2);
        buffer1 = fvalue;
        state = 3;
        updatedisplay();
        break;
      case 2 :
        buffer2 = fvalue;
        fvalue = calculate(op, buffer1, buffer2);
        buffer1 = fvalue;
        state = 3;
        updatedisplay();
        break;
      case 3 :
        fvalue = calculate(op, buffer1, buffer2);
        buffer1 = fvalue;
        updatedisplay();       
        break;
    }
  }

  private void updatedisplay() {
    tdisplay.setText(String.valueOf(fvalue));
  }

  private double calculate(char op, double n1, double n2) {
    switch(op) {
      case '+' :
        return n1 + n2;
      case '-' :
        return n1 - n2;
      case '*' :
        return n1 * n2;
      case '/' :
        return (n2 == 0) ? 0 : n1 / n2;
      default :
        return 0;
    }
  }
}
