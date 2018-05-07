/**
 * Represents a RESTGame client in the program.
 */

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;

import java.io.*;

import org.apache.http.HttpEntity;
import org.apache.http.HttpHeaders;
import org.apache.http.HttpResponse;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.entity.StringEntity;

public class RESTGame {

  private String message;
  private int[][] board;
  private static int BOARDSIZE = 3;

  /**
   * Constructor to initiate a new instance.
   *
   * @param message the message associated with current game status read from JSON.
   * @param board   the current board read from JSON.
   *                return: a new RESTGame instance.
   *                side effect: none.
   */
  public RESTGame(String message, int[][] board) {
    this.message = message;
    this.board = board;
  }

  /**
   * Play the game.
   *
   * @param httpClient the httpclient used for connection.
   * @return a string with the game information comes back from the server.
   * side effect: none.
   */
  public String playGame(HttpClient httpClient) {

    // print out the current map
    System.out.println(this);

    //set up the network conditions
    int PORT = 1234;
    String PLAY_URL = "http://localhost:" + PORT + "/playgame";

    // get user to play the game
    getUserInput();

    // send back, convert to json file first
    Gson gson = new Gson();
    String jsonBoard = gson.toJson(board);

    String completeInput = "";

    try {
      HttpPost postRequest = new HttpPost(PLAY_URL);
      postRequest.setEntity(new StringEntity(jsonBoard));
      postRequest.setHeader(HttpHeaders.CONTENT_TYPE, "application/json");

      HttpResponse httpResponse = httpClient.execute(postRequest);
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


    } catch (IOException e) {
      e.printStackTrace();
    }

    return completeInput;

  }

  /**
   * Get user's input to play the game.
   * return: void.
   * side effect: change the "board" matrix.
   */
  private void getUserInput() {

    int col = 0, row = 0;
    String input = null;
    BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(System.in));

    while (true) {

      do {
        System.out.println("Please Enter a row number from 1 to " + BOARDSIZE);
        try {
          input = bufferedReader.readLine();
          row = Integer.parseInt(input);
        } catch (NumberFormatException e) {
          System.out.println("Invalid input. Please re-enter.");
          row = 0;
        } catch (IOException e) {
          row = 0;
        }
      } while (row < 1 || row > 3);

      do {
        System.out.println("Please Enter a column number from 1 to " + BOARDSIZE);
        try {
          input = bufferedReader.readLine();
          col = Integer.parseInt(input);
        } catch (NumberFormatException e) {
          System.out.println("Invalid input. Please re-enter.");
          col = 0;
        } catch (IOException e) {
          col = 0;
        }

      } while (col < 1 || col > 3);

      if (board[row - 1][col - 1] == 0) {
        board[row - 1][col - 1] = -1;
        break;
      } else {
        System.out.println("Occupied Spot. Please re-enter the position.");
      }

    }

    System.out.println("\n\n");

  }

  /**
   * Override the original toString method for pretty print of the game.
   *
   * @return the String that represents the current game status and information.
   * side effect: none.
   */
  @Override
  public String toString() {
    StringBuilder sb = new StringBuilder();

    sb.append(message).append("\n\n");
    sb.append("X: Enemy\n\nO: Player\n\n_: Empty Spot\n\n");
    sb.append("========================================");

    for (int i = 0; i < board.length; i++) {
      sb.append("\n\n");
      for (int j = 0; j < board[0].length; j++) {
        if (board[i][j] == 1)
          sb.append("X  ");
        else if (board[i][j] == -1)
          sb.append("O  ");
        else sb.append("_  ");
      }
    }

    sb.append("\n\n");
    return sb.toString();

  }


}