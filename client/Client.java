/**
 * Represents the client to play the game.
 */

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;

import java.io.*;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;


public class Client {

  private static int PORT = 1234;
  private static int BOARDSIZE = 3;
  private static String START_URL = "http://localhost:" + PORT + "/startgame";

  /**
   * Start the client program.
   *
   * @param args the argument inputs.
   *             returns: void
   *             side effects: none
   */
  public static void main(String[] args) {

    // client for connection
    HttpClient httpClient = new DefaultHttpClient();

    // collect the input.
    String completeInput = "";
    try {

      //Connect to the REST start end point.
      HttpGet httpGetRequest = new HttpGet(START_URL);
      HttpResponse httpResponse = httpClient.execute(httpGetRequest);

      System.out.println("----------------------------------------");
      System.out.println(httpResponse.getStatusLine());
      System.out.println("----------------------------------------");
      System.out.println("\n\n\n");

      HttpEntity entity = httpResponse.getEntity();

      byte[] buffer = new byte[1024];

      if (entity != null) {
        InputStream inputStream = entity.getContent();
        try {
          int bytesRead = 0;
          BufferedInputStream bis = new BufferedInputStream(inputStream);
          while ((bytesRead = bis.read(buffer)) != -1) {
            String chunk = new String(buffer, 0, bytesRead);
            completeInput += chunk;
          }
        } catch (Exception e) {
          e.printStackTrace();
        } finally {
          try {
            inputStream.close();
          } catch (Exception ignore) {
          }
        }
      }

      Gson gson = new GsonBuilder().create();
      RESTGame restGame = null;

      // judge if the game continues
      while (completeInput.indexOf("Game Ends") < 0) {
        // parse in the current board
        restGame = gson.fromJson(completeInput, RESTGame.class);
        completeInput = restGame.playGame(httpClient);
      }

      // play the last game
      restGame = gson.fromJson(completeInput, RESTGame.class);
      System.out.println(restGame);

    } catch (Exception e) {
      e.printStackTrace();
    } finally {
      httpClient.getConnectionManager().shutdown();
    }

    System.out.println("Client Connection Closed.\n");


  }


}