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
import java.awt.event.*;

public class vcalcpanel extends Panel implements ActionListener {
  private Button b0 = new Button("0");
  private Button b1 = new Button("1");
  private Button b2 = new Button("2");
  private Button b3 = new Button("3");
  private Button b4 = new Button("4"); 
  private Button b5 = new Button("5");
  private Button b6 = new Button("6");
  private Button b7 = new Button("7");
  private Button b8 = new Button("8");
  private Button b9 = new Button("9");
  private Button bDot = new Button(".");
  private Button bEq = new Button("=");
  private Button bPlus = new Button("+");
  private Button bMinus = new Button("-");
  private Button bTimes = new Button("*");
  private Button bDivide = new Button("/");
  private Button bMemAdd = new Button("M+");
  private Button bMemRem = new Button("M-");
  private Button bMemRec = new Button("MR");
  private Button bCancel = new Button("C");
  private vcalcdisplay cDisplay = new vcalcdisplay();
  
  public vcalcpanel() {
    Panel pKeypad = new Panel(new GridLayout(5, 4));

    b0.addActionListener(this);
    b1.addActionListener(this);
    b2.addActionListener(this);
    b3.addActionListener(this);
    b4.addActionListener(this);
    b5.addActionListener(this);
    b6.addActionListener(this);
    b7.addActionListener(this);
    b8.addActionListener(this);
    b9.addActionListener(this);
    bPlus.addActionListener(this);
    bMinus.addActionListener(this);
    bTimes.addActionListener(this);
    bDivide.addActionListener(this);
    bEq.addActionListener(this);
    bDot.addActionListener(this);
    bMemAdd.addActionListener(this);
    bMemRem.addActionListener(this);
    bMemRec.addActionListener(this);
    bCancel.addActionListener(this);

    pKeypad.add(bCancel);
    pKeypad.add(bMemAdd);
    pKeypad.add(bMemRem);
    pKeypad.add(bMemRec);
    pKeypad.add(b7); 
    pKeypad.add(b8); 
    pKeypad.add(b9); 
    pKeypad.add(bTimes);
    pKeypad.add(b4); 
    pKeypad.add(b5); 
    pKeypad.add(b6); 
    pKeypad.add(bDivide);
    pKeypad.add(b1); 
    pKeypad.add(b2); 
    pKeypad.add(b3);
    pKeypad.add(bPlus);
    pKeypad.add(b0);
    pKeypad.add(bDot);
    pKeypad.add(bEq);
    pKeypad.add(bMinus);

    setLayout(new BorderLayout());
    add(cDisplay, "North");
    add(pKeypad, "Center");
  }

  public void actionPerformed(ActionEvent e) {
    switch(e.getActionCommand().charAt(0)) {
      case '0' : 
      case '1' : 
      case '2' : 
      case '3' : 
      case '4' : 
      case '5' : 
      case '6' : 
      case '7' : 
      case '8' : 
      case '9' : 
        cDisplay.pressedNum(Integer.parseInt(e.getActionCommand()));
        break;
      case '+' :
      case '-' :
      case '*' :
      case '/' :
        cDisplay.pressedOp(e.getActionCommand().charAt(0));
        break; 
      case '=' :
        cDisplay.pressedEquals(); 
        break; 
      case 'C' :
        cDisplay.pressedC();
        break;
      case '.' :
        cDisplay.pressedDot();
        break;
    }
  }
}
