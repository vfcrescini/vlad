// the cheat class
// Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>

import java.io.*;
import java.lang.*;
import java.util.*;
import java.net.Socket;

class cheat
{
  private int port;
  private String server;
  private Socket socket;
  private LineNumberReader in_stream;
  private PrintWriter out_stream;

  public cheat(int a_port, String a_server)
  {
    port = a_port;
    server = a_server;
  }

  // returns true if successfull, false otherwise
  public boolean connect()
  {
    try {
      // establish socket connection
      socket = new Socket(server, port);

      // bind the streams
      in_stream = new LineNumberReader(new InputStreamReader(socket.getInputStream()));
      out_stream = new PrintWriter(socket.getOutputStream(), true);

      // register an anonymous agent
      out_stream.println("0,i,anonymous,applet,1.0");
    }
    catch(Exception e) {
      return false;
    }

    return true;
  }

  // returns true on success, false otherwise
  public boolean disconnect()
  {
    if (socket != null) {
      try {
        socket.close();
      }
      catch(Exception e) {
        return false;
      }
    }

    return true;
  }

  // returns a 3d array of agent preferences
  public int[][][] get_agent_prefs()
  {
    int [][][]pref = new int[8][8][6];
    boolean done = false;
    int agent_cnt = 0;

    if (in_stream == null || out_stream == null)
      return null;

    /* send a gs command */
    out_stream.println("0,gs");
    out_stream.flush();

    /* now loop until we stumble upon the prefs section */
    while (!done) {
      String s;
      String sub1;
      String sub2;
      StringTokenizer tokeniser;
      int agent_id;
      int client_cnt;
      int param_cnt;

      try {
        if ((s = in_stream.readLine()) == null)
          break;

        // get the first 2 substrings from the comma delimited line
        tokeniser = new StringTokenizer(s, ",");
        sub1 = tokeniser.nextToken();
        sub2 = tokeniser.nextToken();
 
        // the first char of the second token is the "command"
        switch(sub2.charAt(0)) {
          case 'c' :
            // consume the game id
            Integer.parseInt(tokeniser.nextToken());

            // get the agent id
            agent_id = Integer.parseInt(tokeniser.nextToken());

            // the next tokens are 6-field per-client data
            for (client_cnt = 0; client_cnt < 8 && tokeniser.hasMoreTokens(); client_cnt++) {
              for (param_cnt = 0; param_cnt < 6 && tokeniser.hasMoreTokens(); param_cnt++) {
                pref[agent_cnt][client_cnt][param_cnt] = Integer.parseInt(tokeniser.nextToken());
              }
            }

            // if we already have 8 agents, it is time to stop
            if ((++agent_cnt) > 7)
              done = true;

            break;
          default :
            // don't care about anything else
            break;
        }
      }
      catch(Exception e){
        return pref;
      }
    }

    return pref;
  }
}
