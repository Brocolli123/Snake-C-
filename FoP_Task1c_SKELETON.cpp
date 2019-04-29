//---------------------------------------------------------------------------
//Program: Skeleton for Task 1c – group assignment
//Author: Pascale Vacher
//Last updated: 23 February 2018
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//----- include libraries
//---------------------------------------------------------------------------

//include standard libraries
#include <fstream>
#include <iostream>	
#include <iomanip> 
#include <conio.h> 
#include <cassert> 
#include <string>
#include <sstream>
#include <ctime>
#include <vector>
#include <stdlib.h>
using namespace std;

//include our own libraries
#include "RandomUtils.h"    //for seed, random
#include "ConsoleUtils.h"	//for clrscr, gotoxy, etc.
#include "TimeUtils.h"		//for getSystemTime, timeToString, etc.

//---------------------------------------------------------------------------
//----- define constants
//---------------------------------------------------------------------------

const int PILLMOVES(10);		//10 by default (Max moves of a pill)
//defining the size of the grid
const int  SIZEX(12);    	//horizontal dimension
const int  SIZEY(10);		//vertical dimension
//defining symbols used for display of the grid and content
const char HEAD('@');   	//spot
const char MOUSE('+');
const char PILL('0');
const char TAIL('=');
const char TUNNEL(' ');    	//tunnel
const char WALL('#');    	//border
//defining the command letters to move the spot on the maze
const int  UP(72);			//up arrow
const int  DOWN(80); 		//down arrow
const int  RIGHT(77);		//right arrow
const int  LEFT(75);		//left arrow
//defining the other command letters
const char QUIT('Q');		//to end the game
const char CHEAT('C');
const char SPEED('Z');

struct Item {
	int x, y;
	char symbol;
  bool visible;     
};


struct GameData {
    int miceEaten;        //mice that have been eaten so far
    bool inCheatMode;     //if user is in cheat mode
    bool hasCheated;      //if user has used cheat mode to disable score
    bool isDead;          //if user is dead for ending program
    bool isInvincible;    //of user is unkillable in cheat mode (do need both of these if use incheatmode?)
    bool spawnPill;       //So pill is only spawned every other mouse
    int movesLeft;        //moves left before pill disappears
    int pillNo;
    string playername;
    int score;
    int tempKey;      //Variables for automatic motion
    bool keyPressed;
    int snakeSpeed;
    int currentSpeed;
    bool speedIncrease;
    float timer = 60;
    float Ticks1 = 0;
    float Ticks2 = 0;
    float Ticks3 = 0;
    int timerOutput = 60;
    bool outOfTime;
};

//---------------------------------------------------------------------------
//----- run game
//---------------------------------------------------------------------------

int main()
{
  //function declarations (prototypes)
  void initialiseGame(char g[][SIZEX], char m[][SIZEX], vector<Item>& snake, Item& mouse, Item& pill, GameData& gD);
  void renderGame(const char g[][SIZEX], const string& mess, GameData& gD);
  void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
  void updateGame(char g[][SIZEX], const char m[][SIZEX], Item& mouse, Item& pill, vector<Item>& s, const int kc, string& mess, GameData& gD);
  void CheatMode(vector<Item>& snake, vector<Item>& cheatSnake, GameData& gD, string& message);
  void SnakeMove(GameData& gD);
  void setKeyDirection(const int key, int& dx, int& dy);
  bool wantsToQuit(const int key);
  bool isCheatKey(const int k);
  bool isArrowKey(const int k);
  int  getKeyPress();
  void writeScoreFile(const int score, const string name);
  int readScoreFile(const string name);
  void endProgram(bool isDead, GameData gD);   //Doesn't need reference because it quits program there

  //local variable declarations 
  char grid[SIZEY][SIZEX];			//grid for display
  char maze[SIZEY][SIZEX];			//structure of the maze
  Item mouse = { 0,0, MOUSE, false };		//mouse			//could change this definition to somewhere else?
  Item pill = { 0,0, PILL, false };                                                                                      //add visible to these??
  vector<Item> snake = { { 0,0,HEAD }, { 0,0,TAIL }, { 0,0,TAIL }, { 0,0,TAIL } };                                  //add visible to these??
  vector<Item> cheatSnake = snake;
  string message("LET'S START...");	//current message to player
  string playername;		//For displaying and for score.txt file
  int key = 0;							//current key selected by player
  GameData gameData = { 0, false, false, false, false, false, PILLMOVES, 2, "player", 0, 0, false, 500, 0, true };

  cout << "What is the player's name? \n";
  cin >> gameData.playername;			//This stays here too after player inputs it
  gameData.score = readScoreFile(gameData.playername);    //reads file for old score
  seed();								//seed the random number generator
  SetConsoleTitleA("FoP 2018-19 - Task 1c - Game Skeleton");
  initialiseGame(grid, maze, snake, mouse, pill, gameData);	//initialise grid (incl. walls and spot)
  showMessage(clDarkCyan, clWhite, 40, 5, "TO TURN ON CHEAT MODE - ENTER 'C'");	//Initial Cheat instructions (Here for now)
  showMessage(clDarkBlue, clWhite, 40, 6, "Player name is " + playername);
  bool timerIsRunning = false;
  int movesLeft = PILLMOVES;		//for how many turns left for pill							(if user eats a pill return it to 10 or if it's 0 return it to 10)
  //spawn a pill
  //if pill moves is 0 spawn another (in updategame? or rendergame? or the key input stage? every second mouse? (use a bool or index?)

  do {  
    renderGame(grid, message, gameData);          //display game info, modified grid and messages
    SnakeMove(gameData);      //Increase speed over time

    gameData.keyPressed = false;
    if (_kbhit()) {
        key = getKeyPress(); 	//read in  selected key: arrow or letter command
        gameData.keyPressed = true;
    }

    string moves = to_string(movesLeft);					//Show how many moves left pill has
    showMessage(clRed, clYellow, 40, 13, moves);                                                    //MOVES goes to -9 or something when player dies
    string scorestring = to_string(gameData.score);			//turn the score to a string
    showMessage(clDarkBlue, clWhite, 40, 16, scorestring);		//Show player score and update
    string miceEatString = to_string(gameData.miceEaten);				//Have both on same line?
    showMessage(clDarkBlue, clWhite, 40, 17, miceEatString + "/7 Mice Eaten");		//Show mice eaten and update
    if (timerIsRunning == false) {
        gameData.Ticks2 = clock();
    }

    if (gameData.keyPressed) {
        if (isArrowKey(key)) {
            updateGame(grid, maze, mouse, pill, snake, key, message, gameData);
            gameData.tempKey = key;
            timerIsRunning = true;
        }
        if (toupper(key) == CHEAT) {
            CheatMode(snake, cheatSnake, gameData, message);
        }
        if (toupper(key) == SPEED) {
            if (gameData.speedIncrease) {
                gameData.speedIncrease = false;
                gameData.currentSpeed = gameData.snakeSpeed;
                gameData.snakeSpeed = 500;
            }
            else {
                gameData.snakeSpeed = gameData.currentSpeed;
                gameData.speedIncrease = true;
            }
        }
        if (isArrowKey(key) == false && toupper(key) != CHEAT) {
            message = "INVALID KEY!";  //set 'Invalid key' message
        }
        else {    //Key is valid, do all counters
          --gameData.movesLeft;
          if (gameData.hasCheated == false) {
            ++gameData.score;        //increment score on move (if user hasn't cheated)  //Score not going above 0?????
          }
          else {
            gameData.score = 0;  //Has cheated so score is 0 (display message somewhere if user has cheated?)
          }
        }
    }
    else if (gameData.tempKey != 0)
    {
       updateGame(grid, maze, mouse, pill, snake, gameData.tempKey, message, gameData);
       --gameData.movesLeft;
       if (gameData.hasCheated == false) {
         ++gameData.score;        //increment score on move (if user hasn't cheated)  //Score not going above 0?????
       }
       else {
         gameData.score = 0;  //Has cheated so score is 0 (display message somewhere if user has cheated?)
       }
    }

    if (gameData.miceEaten >= 7)  //Quit game if all mice are eaten (should call endprogram)
    {
      return 0;
    }

    } while (!wantsToQuit(key));		//while user does not want to quit
    renderGame(grid, message, gameData);			//display game info, modified grid and messages
    writeScoreFile(gameData.score, playername);	//creates score file    //NEED TO WRITE TO THIS IF PLAYER IS KILLED TOO (ALWAYS END UP HERE INSTEAD OF BREAKING OUT WHEN KILLED)
    endProgram(gameData.isDead, gameData);						//display final message
    return 0;
  }

//---------------------------------------------------------------------------
//----- initialise game state
//---------------------------------------------------------------------------

void initialiseGame(char grid[][SIZEX], char maze[][SIZEX], vector<Item>& snake, Item& mouse, Item& pill, GameData& gD)
{ //initialise grid and place spot in middle
	void setInitialMazeStructure(char maze[][SIZEX]);
	void setSnakeInitialCoordinates(const  char maze[][SIZEX], vector<Item>& Snake);
	void updateGrid(char g[][SIZEX], const char m[][SIZEX], vector<Item>& snake, Item& mouse, Item& pill, GameData& gD); // Alex - Would need to add the MOUSE item here if it isn't public, I can't tell

	setInitialMazeStructure(maze);		//initialise maze
	setSnakeInitialCoordinates(maze, snake);
	updateGrid(grid, maze, snake, mouse, pill, gD);		//prepare grid
}

void setSnakeInitialCoordinates(const char maze[][SIZEX], vector<Item>& snake)
{ //set spot coordinates inside the grid at random at beginning of game

	//do
	//{
		//snake.at(0).y = random(SIZEY - 2);		// Finds co-ordinates at random again if the co-ordinate is inside a wall
		//snake.at(0).x = random(SIZEX - 2);
    snake.at(0).x = 4;    //Initial co-ords of snake set to middle of grid (Pascale wants this to be 0,0?????)
    snake.at(0).y = 5;
	//} while (maze[snake.at(0).y][snake.at(0).x] == WALL);   //not grid here doesn't have it here???

	for (size_t i(1); i < snake.size(); ++i) {		//go through loop setting position of all the snake items to same spot
		snake.at(i).y = snake.at(0).y;
		snake.at(i).x = snake.at(0).x;
	}	//Place head last so it appears on top (on initial snake)

}

void setInitialMazeStructure(char maze[][SIZEX])
{ //set the position of the walls in the maze
  //initialise maze configuration
	char initialMaze[SIZEY][SIZEX] 	//local array to store the maze structure
		= { { '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', '#', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', '#', '#', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', '#', '#', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', '#', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#' } };
	//with '#' for wall, ' ' for tunnel, etc. 
	//copy into maze structure with appropriate symbols
	for (int row(0); row < SIZEY; ++row)
		for (int col(0); col < SIZEX; ++col)
			switch (initialMaze[row][col])
			{
			//not a direct copy, in case the symbols used are changed
			case '#': maze[row][col] = WALL; break;
			case ' ': maze[row][col] = TUNNEL; break; 
			}
}

//---------------------------------------------------------------------------
//----- Update Game
//---------------------------------------------------------------------------

void updateGame(char grid[][SIZEX], const char maze[][SIZEX], Item& mouse, Item& pill, vector<Item>& snake, const int keyCode, string& mess, GameData& gD)
{ //update game
	void updateGameData(const char g[][SIZEX], Item& mouse, Item& pill, vector<Item>& s, const int kc, string& m, GameData& gD);
	void updateGrid(char g[][SIZEX], const char maze[][SIZEX], vector <Item>& s, Item& mouse, Item& pill, GameData& gD);		//does vector have to be const
	updateGameData(grid, mouse, pill, snake, keyCode, mess, gD);		//move spot in required direction
	updateGrid(grid, maze, snake, mouse, pill, gD);					//update grid information
    
}

void updateGameData(const char g[][SIZEX], Item& mouse, Item& pill, vector<Item>& snake, const int key, string& mess, GameData& gD)
{ //move spot in required direction
	void endProgram(bool isDead, GameData gD);
	bool isArrowKey(const int k);
	void setKeyDirection(int k, int& dx, int& dy);
    void countdownTimer(GameData& gD);
	assert (isArrowKey(key));                                                                                 
	//calculate direction of movement for given key
	int dx(0), dy(0);
	setKeyDirection(key, dx, dy);

	//check new target position in grid and update game data (incl. spot coordinates) if move is possible
	switch (g[snake.at(0).y + dy][snake.at(0).x + dx])		//checking every point of snake for collision with wall
		{			//...depending on what's on the target position in grid...
		case TUNNEL:		//can move
			for (size_t i(snake.size() - 1); i > 0 ; --i) {		//move tail first then head.
				snake.at(i).y = snake.at(i - 1).y;				//set to position before it
				snake.at(i).x = snake.at(i - 1).x;
			}
			snake.at(0).y += dy;	//go in that Y direction
			snake.at(0).x += dx;	//go in that X direction
			break;
		case WALL:  		//hit a wall and stay there
			mess = "CANNOT GO THERE!";
      if (gD.isInvincible == false) {       //If player is killable end game
        gD.isDead = true; //Player is dead      
      }
			break;
	   case MOUSE: //- Alex - Should maybe get rid of the mouse and then have the tail of the snake grow by two. Not sure on this one, needs testing
		   gD.miceEaten++;
		   for (size_t i(snake.size() - 1); i > 0; --i) {		//move tail first then head.
			   snake.at(i).y = snake.at(i - 1).y;				//set to position before it
			   snake.at(i).x = snake.at(i - 1).x;
		   }
		   snake.at(0).y += dy;	//go in that Y direction
		   snake.at(0).x += dx;	//go in that X direction
		   mouse.visible = false;
		   Item growSnake;
		   growSnake.symbol = TAIL;
		   growSnake.x = snake.at((snake.size() - 1)).x;
		   growSnake.y = snake.at((snake.size() - 1)).y;
		   snake.push_back(growSnake);
		   break;
	   case PILL:
		   snake.resize(4);
		   for (size_t i(snake.size() - 1); i > 0; --i) {
			   snake.at(i).y = snake.at(i - 1).y;
			   snake.at(i).x = snake.at(i - 1).x;
		   }
		   snake.at(0).y += dy;
		   snake.at(0).x += dx;
		   pill.visible = false;
		   break;
     case TAIL:
       mess = "CANNOT GO THERE!";
       if (gD.isInvincible == false) {        //If player is killable end game
         gD.isDead = true; //Player is dead                                                                            
       }
       break;
	   default:
		   void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
		   showMessage(clDarkCyan, clWhite, 40, 16, "Quitting Program");  //We're in the endgame now
		}
    
  if (gD.isDead == true) {
    endProgram(true, gD);       //player is dead, send message to exit
  }
	//if (IsMousePresent == false) // Alex - Supposedly should dump the mouse in a random place if there isn't one present - Mouse logic still needs to be added and changed as to not allow for it to appear in a wall
	//{
	//	mouse.y = random(SIZEY - 2);		//vertical coordinates in range 1-(SIZEY-2)
	//	mouse.x = random(SIZEX - 2);		//horizontal coordinate in range 1-(SIZEX - 2)
	//
	//  IsMousePresent = true;
	//}

  countdownTimer(gD);
}

void updateGrid(char grid[][SIZEX], const char maze[][SIZEX], vector<Item>& snake, Item& mouse, Item& pill, GameData& gD)
{ //update grid configuration after each move
	void placeMaze(char g[][SIZEX], const char b[][SIZEX]);
	void placeItem(char g[][SIZEX], const Item& spot);
    void removeItem(char g[][SIZEX], const Item& spot);

	placeMaze(grid, maze);	//reset the empty maze configuration into grid
	//go through place item for each spot of snake in loop
	for (size_t i(snake.size()-1); i > 0; --i) {			//Place head last so it appears on top (on initial snake)			ONLY LOOPING 3 TIMES (NOT SHOWING HEAD)
		placeItem(grid, snake.at(i));			//set current spot of snake tails
	}
	placeItem(grid, snake.at(0));			//set current spot of snake head

  if (gD.movesLeft <= 0)  //pill run out of moves
  {
    removeItem(grid, pill);   //Get rid of pill
    pill.visible = false;
    gD.movesLeft = 10;  //reset counter
  }

	if (mouse.visible == false) 
	{
		do {
			mouse.y = random(SIZEY - 2);		//vertical coordinates in range 1-(SIZEY-2)
			mouse.x = random(SIZEX - 2);		//horizontal coordinate in range 1-(SIZEX - 2)
		} while (grid[mouse.y][mouse.x] != TUNNEL);
    placeItem(grid, mouse);			//moving after first placement
		mouse.visible = true;	
	}
	placeItem(grid, mouse);

  if (pill.visible == false) {  //change to only spawn every other mouse
    gD.movesLeft = 10;  //reset moves left on new pill
      do {
        pill.y = random(SIZEY - 2);		//place in random spot
        pill.x = random(SIZEX - 2);
      } while (grid[pill.y][pill.x] != TUNNEL);		//keeps randomly placing and goes to 0,0        (CHANGED TO GRID)
    }
    pill.visible = true;
    placeItem(grid, pill);
    ++gD.pillNo;  //use to decide when to spawn pill
}


void CheatMode(vector<Item>& snake, vector<Item>& cheatSnake, GameData& gD, string& message) {    //Reset snake     (Take in snake and return length of snake (reference or as int?) before function to be used to restore later
  void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);

  if (gD.hasCheated != true) {  //Set true if not already.
    gD.hasCheated = true; //Use to pause score
  }
  gD.inCheatMode = !gD.inCheatMode; //Flips the bool

      //Update game meant to be in here??
    if (gD.inCheatMode == true) {
      for (int i(0); i < 4; ++i) {   
        cout << '\a';	          //beep the alarm x3
        Sleep(100);
      }
      cheatSnake = snake;	//get original snake length before cheating abd send the variable back to main for turning cheat mode off
      snake.resize(4);  //Sets it back to 4

      gD.isInvincible = true;   //for not killing snake
      showMessage(clDarkCyan, clWhite, 40, 5, "TO TURN OFF CHEAT MODE - ENTER 'C'");		//Display instructions for cheat mode
      message = "CHEAT MODE ON";
    }
    else {    //inCheatMode == False   Resize back to original
      gD.isInvincible = false;  //no longer unkillable
      showMessage(clDarkCyan, clWhite, 40, 5, "TO TURN ON CHEAT MODE - ENTER 'C'");		//Display instructions for cheat mode			//EXTRACT THIS TO A FUNCTOIN
      message = "CHEAT MODE OFF";
      // SnakeNeedsToGrow = true;
      //snake.resize(cheatSnake.size());   //Set to size of cheatSnake
      int sizeDifference = cheatSnake.size() - snake.size();
      for (int i = 0; i < sizeDifference; i++)
      {
        Item growSnake;
        growSnake.symbol = TAIL;
        growSnake.x = snake.at((snake.size() - 1)).x;
        growSnake.y = snake.at((snake.size() - 1)).y;
        snake.push_back(growSnake);
      }
    }
  }

void SnakeMove(GameData& gD) {    //Increase speed over time
  Sleep(gD.snakeSpeed);
  if (gD.speedIncrease == true) {
    if (gD.snakeSpeed >= 250) {
      gD.snakeSpeed -= 4;
    }
    else {
      gD.snakeSpeed = 250;
    }
  }
  else
  {
    gD.snakeSpeed = 500;
  }
}

void placeMaze(char grid[][SIZEX], const char maze[][SIZEX])
{ //reset the empty/fixed maze configuration into grid
	for (int row(0); row < SIZEY; ++row)
		for (int col(0); col < SIZEX; ++col)
			grid[row][col] = maze[row][col];
}

void placeItem(char g[][SIZEX], const Item& item)
{ //place item at its new position in grid
	g[item.y][item.x] = item.symbol;
}

void removeItem(char g[][SIZEX], const Item& item) {
  g[item.y][item.x] = TUNNEL;   //Gets rid of the item
}
//---------------------------------------------------------------------------
//----- process key
//---------------------------------------------------------------------------
void setKeyDirection(const int key, int& dx, int& dy)
{ //calculate direction indicated by key
	bool isArrowKey(const int k);
	//assert (isArrowKey(key));       
	switch (key)	//...depending on the selected key...
	{
	case LEFT:  	//when LEFT arrow pressed...
		dx = -1;	//decrease the X coordinate
		dy = 0;
		break;
	case RIGHT: 	//when RIGHT arrow pressed...
		dx = +1;	//increase the X coordinate
		dy = 0;
		break;
	case UP:  	//when UP arrow pressed...
		dx = 0;	//Increase the Y coordinate
		dy = -1;
		break;
	case DOWN: 	//when DOWN arrow pressed...
		dx = 0;	//Decrease the Y coordinate
		dy = +1;
		break;
	}
}

int getKeyPress()
{ //get key or command selected by user
  //KEEP THIS FUNCTION AS GIVEN
	int keyPressed;
	keyPressed = toupper(_getch());			//read in the selected arrow key or command letter
	while (keyPressed == 224) 		//ignore symbol following cursor key
		keyPressed = _getch();
	return keyPressed;				 
}
bool isArrowKey(const int key)
{	//check if the key pressed is an arrow key (also accept 'K', 'M', 'H' and 'P')
	return (key == LEFT) || (key == RIGHT) || (key == UP) || (key == DOWN);
}
bool wantsToQuit(const int key)
{	//check if the user wants to quit (when key is 'Q' or 'q')
	return toupper(key) == QUIT;
}

//---------------------------------------------------------------------------
//----- display info on screen
//---------------------------------------------------------------------------

string tostring(int x)
{	//convert an integer to a string
	std::ostringstream os;
	os << x;
	return os.str();
}
string tostring(char x) 
{	//convert a char to a string
	std::ostringstream os;
	os << x;
	return os.str();
}
void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message)
{	//display a string using specified colour at a given position 
	gotoxy(x, y);
	selectBackColour(backColour);
	selectTextColour(textColour);
	cout << message + string(40 - message.length(), ' ');
}

void renderGame(const char g[][SIZEX], const string& mess, GameData& gD)
{ //display game title, messages, maze, spot and other items on screen
	string tostring(char x);
	string tostring(int x);
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
	void paintGrid(const char g[][SIZEX]);
    void displayDateAndTime();

	//display game title
	showMessage(clBlack, clCyan, 0, 0, "Snek Gam");
    displayDateAndTime();
	showMessage(clWhite, clBlue, 40, 0, "FoP Task 1c - February 2019   ");  
	showMessage(clWhite, clBlue, 40, 1, "The Big Oof Squad               ");
	showMessage(clWhite, clBlue, 40, 2, "CS4G2e ");
	showMessage(clWhite, clBlue, 40, 12, "Lewis Birkett,Alex Hughes,Aiden Fleming");		//No Spaces to fit all on one line (change later?)
    showMessage(clWhite, clBlue, 40, 11, "b8018431, b7022472, b8025218");		//No Spaces to fit all on one line (change later?)
	
  showMessage(clDarkCyan, clWhite, 40, 5, "TO TURN ON CHEAT MODE - ENTER 'C'");	//Initial Cheat instructions (Here for now)
  showMessage(clDarkBlue, clWhite, 40, 6, "Player name is " + gD.playername);
  string moves = to_string(gD.movesLeft);					//Show how many moves left pill has
  showMessage(clRed, clYellow, 40, 13, moves);                                                    //MOVES goes to -9 or something when player dies
  string scorestring = to_string(gD.score);			//turn the score to a string
  showMessage(clDarkBlue, clWhite, 40, 16, scorestring);		//Show player score and update
  string miceEatString = to_string(gD.miceEaten);				//Have both on same line?
  showMessage(clDarkBlue, clWhite, 40, 17, miceEatString + "/7 Mice Eaten");		//Show mice eaten and update

                                                                                      //display menu options available
    
	showMessage(clDarkCyan, clWhite, 40, 3, "TO MOVE - USE KEYBOARD ARROWS ");
	showMessage(clDarkCyan, clWhite, 40, 4, "TO QUIT - ENTER 'Q'           ");

	//print auxiliary messages if any
	showMessage(clBlack, clWhite, 40, 8, mess);	//display current message

    showMessage(clDarkCyan, clWhite, 40, 8, "Time remaining: " + to_string(gD.timerOutput) + " seconds");


	//display grid contents
	paintGrid(g);
}

void displayDateAndTime()
{
    void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);

    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
    string str(buffer);

    showMessage(clWhite, clBlue, 40, 15, str);
}

void paintGrid(const char g[][SIZEX])
{ //display grid content on screen
	selectBackColour(clBlack);
	selectTextColour(clWhite);
	gotoxy(0, 2);
	for (int row(0); row < SIZEY; ++row)
	{
		for (int col(0); col < SIZEX; ++col)
			cout << g[row][col];	//output cell content
		cout << endl;
	}
}

void writeScoreFile(const int score, const string name) {		//Displaying high score vs current score    //always going to be 500, it's always above current score

		ifstream fin;
		fin.open(name + ".txt", ios::in);	//opens file with name as filename. in read mode

		if (fin.fail())
		{//new player
			ofstream fout;
			fout.open(name + ".txt", ios::out);	//create file
			fout << "500";						//set score
			fout.close();
		}

		else { //player already exists
			int previousscore;
			fin >> previousscore;
			if (previousscore < score) {		//new high score (and not the 500 default)
				fin.close();
				ofstream fout;
				fout.open(name + ".txt", ios::out);	//open file
				fout << score;						//change score to new score
				fout.close();
			}
			else
				fin.close();
		}
	}

int readScoreFile(const string name) { 
  
  ifstream fin;
  fin.open(name + ".txt", ios::in);	//opens file with name as filename. in read mode

  if (fin.fail())           //If file doesn't exist has default score
  {//new player
    return 500;   //No previous score (creates file later)
  }
  int score;
  fin >> score;   //reads score from file if not
  return score;
}

void endProgram(bool isDead, GameData gD)
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);

  if (isDead && gD.outOfTime) {
      showMessage(clDarkCyan, clWhite, 40, 8, "Time's up, you lose, Quitting Program");
  }
  else if (isDead) {
    showMessage(clDarkCyan, clWhite, 40, 8, "You Died With " + to_string(gD.timerOutput) + " seconds remaining");    //Dead message        NOT ENDING GAMME WITH THIS ONE???
    showMessage(clDarkCyan, clWhite, 40, 9, "Quitting Program");
  }

  else {
    showMessage(clDarkCyan, clWhite, 40, 8, "Quitting Program");    //Regular Quit message
  }
	system("pause");	//hold output screen until a keyboard key is hit
  exit(0);
}

void countdownTimer(GameData& gD)
{
    gD.Ticks1 = clock();
    gD.Ticks3 = (gD.Ticks1 - gD.Ticks2) / 1000;
    gD.timer -= gD.Ticks3;
    gD.timerOutput = round(gD.timer);
    gD.Ticks2 = clock();
    if (gD.timer <= 0) {
        gD.outOfTime = true;
        endProgram(true, gD);
    }
}
