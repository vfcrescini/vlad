// the cheat driver class
// Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>

import java.io.*;
import java.lang.*;
import java.util.*;

class driver
{
  public static void main(String a_arg[])
  {
    int [][][]prefs;
    int i, j, k;

    // create instance of the class with the right server & port
    cheat ch = new cheat(4041, "137.154.151.110");

    // now connect
    ch.connect();

    // get prefs
    prefs = ch.get_agent_prefs();

    // dump array
    for (i = 0; i < 8; i++) {
      System.out.println("agent " + i);
      for (j = 0; j < 8; j++) {
        System.out.print("client " + j + " ");
        for (k = 0; k < 6; k++) {
          System.out.print(prefs[i][j][k] + " ");
        }
        System.out.println();
      }
      System.out.println();
    }

    // disconnect
    ch.disconnect();
  }
}
