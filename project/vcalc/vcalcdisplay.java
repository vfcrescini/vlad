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
import java.lang.Math;

public class vcalcdisplay extends Panel {
  private double fvalue = 0;
  private double buffer1 = 0;
  private double buffer2 = 0;
  private int decimal = 0;
  private char op = '\0';
  private int state = 0;
  private TextField tDisplay = new TextField("0");

  public vcalcdisplay() {
    setLayout(new BorderLayout());
    add(tDisplay, "North");
  }

  public void pressedCE() {
    fvalue = 0;
    updateDisplay();
  }	

  public void pressedC() {
    state = 0;
    op = '\0';
    fvalue = 0;
    buffer1 = 0;
    buffer2 = 0;
    updateDisplay();
  }

  public void pressedNum(int n) {
    if (n >= 0 && n <= 9) {
      switch(state) {
        case 0 :
	  fvalue = (fvalue * 10) + n;
          updateDisplay();
          break;
        case 1 :
          fvalue = fvalue + (n / (Math.pow(10, ++decimal)));
          updateDisplay();
          break;
        case 2 :
          buffer1 = fvalue;
          fvalue = n;
          state = 3;
          updateDisplay();
          break;
        case 3 :
	  fvalue = (fvalue * 10) + n;
          updateDisplay();
          break;
        case 4 :
          fvalue = fvalue + (n / (Math.pow(10, ++decimal)));
          updateDisplay();
          break;
        case 5 :
          fvalue = n;
          buffer1 = 0;
          buffer2 = 0;
          op = '\0';
          state = 0;
          updateDisplay();
          break;
        case 6 :
          fvalue = n;
          state = 0;
          updateDisplay();
          break;
      }
    } 
  }

  public void pressedOp(char newop) {
    if (newop == '+' || newop == '-' || newop == '*' || newop == '/') {
      switch(state) {
        case 0 :
        case 1 :
        case 2 :
          op = newop;
          state = 2;
          break;
        case 3 :
        case 4 :
          buffer2 = fvalue;
          calculateDisplay();
          buffer1 = fvalue;
          op = newop;
          state = 2;
          updateDisplay();
          break;
        case 5 :
        case 6 :
          op = newop;
          state = 2;
          break;
      }
    }
  }

  public void pressedEquals() {
    switch(state) {
      case 0 :
      case 1 :
        state = 6;
        break;
      case 2 :
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
        calculateDisplay();
        buffer1 = fvalue;
        state = 5;
        updateDisplay();
        break;
      case 3 :
      case 4 :
        buffer2 = fvalue;
        calculateDisplay();
        buffer1 = fvalue;
        state = 5;
        updateDisplay();
        break;
      case 5 :
        calculateDisplay();
        buffer1 = fvalue;
        updateDisplay();       
        break;
      case 6 :
        break;
    }
  }

  public void pressedDot() {
    switch(state) {
      case 0 :
        decimal = 0;
        state = 1;
        break;
      case 1 :
        break;
      case 2 :
        decimal = 0;
        buffer1 = fvalue;
        fvalue = 0;
        state = 4;
        updateDisplay();
        break;
      case 3 :
        decimal = 0;
        state = 4;
        break;
      case 4 :
        break;
      case 5 :
        decimal = 0;
        buffer1 = 0;
        buffer2 = 0;
        fvalue = 0;
        op = '\0';
        state = 1;
        updateDisplay();
        break;
      case 6 :
        decimal = 0;
        state = 1;
        break;
    }
  }

  private void updateDisplay() {
    tDisplay.setText(String.valueOf(fvalue));
  }

  private void calculateDisplay() {
    switch(op) {
      case '+' :
        fvalue = buffer1 + buffer2;
        break;
      case '-' :
        fvalue = buffer1 - buffer2;
        break;
      case '*' :
        fvalue = buffer1 * buffer2;
        break;
      case '/' :
        fvalue = (buffer2 == 0) ? 0 : buffer1 / buffer2;
        break;
      default :
        fvalue = 0;
        break;
    }
  }
}
