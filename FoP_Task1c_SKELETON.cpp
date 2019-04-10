//---------------------------------------------------------------------------
//Program: Skeleton for Task 1c – group assignment
//Author: Pascale Vacher
//Last updated: 23 February 2018
//---------------------------------------------------------------------------

//Go to 'View > Task List' menu to open the 'Task List' pane listing the initial amendments needed to this program

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

struct Item {
	int x, y;
	char symbol;
};

//---------------------------------------------------------------------------
//----- run game
//---------------------------------------------------------------------------

int main()
{
	//function declarations (prototypes)
	void initialiseGame(char g[][SIZEX], char m[][SIZEX], vector<Item>& snake, Item& mouse);
	void renderGame(const char g[][SIZEX], const string& mess);
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
	void updateGame(char g[][SIZEX], const char m[][SIZEX], Item& mouse, vector<Item>& s, const int kc, string& mess, bool& IsMousePresent);
	void CheatMode(vector<Item>& snake, vector<Item>& cheatSnake);
	bool wantsToQuit(const int key);
	bool isCheatKey(const int k);
	bool isArrowKey(const int k);
	int  getKeyPress();
	void checkScoreFile(const int score, const string name);
	void endProgram();

	//local variable declarations 
	char grid[SIZEY][SIZEX];			//grid for display
	char maze[SIZEY][SIZEX];			//structure of the maze
	//Item HEAD = { 0, 0, SPOT }; 		//spot's position and symbol
	//Item Tail = { 0,0,TAIL };			//Tail
	Item mouse = { 0,0, MOUSE };		//mouse
	Item pill = { 0,0, PILL };			//pill
	vector<Item> snake = {{ 0,0,HEAD }, { 0,0,TAIL }, { 0,0,TAIL }, { 0,0,TAIL }};
	vector<Item> cheatSnake = snake;
	string message("LET'S START...");	//current message to player
	bool inCheatMode = false;  //To check if already in cheatmode				//use struct???
	bool hasCheated = false;  //Use later when displaying score to keep it to 0
	string playername;		//For displaying and for score.txt file
	int score = 0;		//for score
	bool IsMousePresent = false;

	cout << "What is the player's name? \n";
	cin >> playername;			//This stays here too after player inputs it
	//action...
	seed();								//seed the random number generator
	SetConsoleTitle("FoP 2018-19 - Task 1c - Game Skeleton");
	initialiseGame(grid, maze, snake, mouse);	//initialise grid (incl. walls and spot)
	showMessage(clDarkCyan, clWhite, 40, 5, "TO TURN ON CHEAT MODE - ENTER 'C'");	//Initial Cheat instructions (Here for now)
	showMessage(clDarkBlue, clWhite, 40, 6, "Player name is " + playername);
	int key;							//current key selected by player
	int movesLeft = PILLMOVES;		//for how many turns left for pill							(if user eats a pill return it to 10 or if it's 0 return it to 10)
	//spawn a pill
	//if pill moves is 0 spawn another (in updategame? or rendergame? or the key input stage?
	do {
		renderGame(grid, message);			//display game info, modified grid and messages
		key = getKeyPress(); 	//read in  selected key: arrow or letter command
		//string moves = to_string(movesLeft);					Use later when pill implemented
		//showMessage(clRed, clYellow, 40, 13, moves);
		if (isArrowKey(key))
			updateGame(grid, maze, mouse, snake, key, message, IsMousePresent);                             
		else {
			if (toupper(key) == CHEAT) {
				hasCheated = true;            //better way of doing this every time?  Used to stop recording score
				inCheatMode = !inCheatMode;   //flips the bool
				if (inCheatMode == true) {
					showMessage(clDarkCyan, clWhite, 40, 5, "TO TURN OFF CHEAT MODE - ENTER 'C'");		//Display instructions for cheat mode
					message = "CHEAT MODE ON";
					CheatMode(snake, cheatSnake);
				}
			else {    //inCheatMode == False
				showMessage(clDarkCyan, clWhite, 40, 5, "TO TURN ON CHEAT MODE - ENTER 'C'");		//Display instructions for cheat mode			//EXTRACT THIS TO A FUNCTOIN
				message = "CHEAT MODE OFF";
				snake.resize(cheatSnake.size());   //Set to size of cheatSnake
				for (size_t i(1); i < cheatSnake.size(); ++i) {
					snake.at(i).symbol = cheatSnake.at(i).symbol;   //Set symbol of snake at the position to the cheatsnake's 
					snake.at(i).x = cheatSnake.at(i).x;   //Sets the cheatSnake position to the current position of the snake
					snake.at(i).y = cheatSnake.at(i).y;
													//Extra snake positions spawning at 0,0 not on top of the current tail end
					}
				}
			} else
				if (isArrowKey(key) == false && toupper(key) != CHEAT)                
					message = "INVALID KEY!";  //set 'Invalid key' message	
		}
		--movesLeft;		//here or at top??????
		++score;
	} while (!wantsToQuit(key));		//while user does not want to quit
	renderGame(grid, message);			//display game info, modified grid and messages
	checkScoreFile(score, playername);	//creates score file
	endProgram();						//display final message
	return 0;
}


//---------------------------------------------------------------------------
//----- initialise game state
//---------------------------------------------------------------------------

void initialiseGame(char grid[][SIZEX], char maze[][SIZEX], vector<Item>& snake, Item& mouse)
{ //initialise grid and place spot in middle
	bool IsMousePresent = false;
	void setInitialMazeStructure(char maze[][SIZEX]);
	void setSnakeInitialCoordinates(const  char maze[][SIZEX], vector<Item>& Snake);
	void updateGrid(char g[][SIZEX], const char m[][SIZEX], vector<Item>& snake, Item& mouse, bool& IsMousePresent); // Alex - Would need to add the MOUSE item here if it isn't public, I can't tell

	setInitialMazeStructure(maze);		//initialise maze
	setSnakeInitialCoordinates(maze, snake);
	updateGrid(grid, maze, snake, mouse, IsMousePresent);		//prepare grid
}

void setSnakeInitialCoordinates(const char maze[][SIZEX], vector<Item>& snake)
{ //set spot coordinates inside the grid at random at beginning of game

  //snake.at(0).y = random(SIZEY - 2);		//vertical coordinates in range 1-(SIZEY-2)
  //snake.at(0).x = random(SIZEX - 2);		//horizontal coordinate in range 1-(SIZEX - 2)
	do //Alex - Doesn't work. No idea why though. If it works then this can be cloned for the mouse
	{
		snake.at(0).y = random(SIZEY - 2);		// Attempts to find co-ordinates at random again if the co-ordinate is inside a wall
		snake.at(0).x = random(SIZEX - 2);

	} while (maze[snake.at(0).y][snake.at(0).x] == WALL);

	for (size_t i(1); i < snake.size(); ++i) {		//go through loop setting position of all the snake items to same spot
		snake.at(i).y = snake.at(0).y;
		snake.at(i).x = snake.at(0).x;
	}	//Place head last so it appears on top (on initial snake)

}

void setInitialMazeStructure(char maze[][SIZEX])
{ //set the position of the walls in the maze
//TODO: Amend initial maze configuration (change size changed and inner walls)
  //initialise maze configuration
	char initialMaze[SIZEY][SIZEX] 	//local array to store the maze structure
		= { { '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', '#', ' ', ' ', ' ', '#', '#', ' ', ' ', '#' },
		{ '#', ' ', ' ', '#', ' ', ' ', ' ', ' ', '#', ' ', ' ', '#' },
		{ '#', ' ', ' ', '#', '#', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
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

void updateGame(char grid[][SIZEX], const char maze[][SIZEX], Item& mouse, vector<Item>& snake, const int keyCode, string& mess, bool& IsMousePresent)
{ //update game
	//bool IsMousePresent = false;
	void updateGameData(const char g[][SIZEX], Item& mouse, vector<Item>& s, const int kc, string& m);
	void updateGrid(char g[][SIZEX], const char maze[][SIZEX], vector <Item>& s, Item& mouse, bool& IsMousePresent);		//does vector have to be const
	updateGameData(grid, mouse, snake, keyCode, mess);		//move spot in required direction
	updateGrid(grid, maze, snake, mouse, IsMousePresent);					//update grid information
}

void updateGameData(const char g[][SIZEX], Item& mouse, vector<Item>& snake, const int key, string& mess)
{ //move spot in required direction
	bool isArrowKey(const int k);
	void setKeyDirection(int k, int& dx, int& dy);
	/*bool IsMousePresent = false;*/   // Alex - Can't test this ='( - Used for the spawning of the mouse below 
	//assert (isArrowKey(key));                                                                                           //REMOVE FOR NOW SO CAN USE NON ARROW KEYS
 
	//reset message to blank
	//mess = "";

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
		case PILL:
			snake.resize(4);	//shrink snake back to 4
			//move snake
			//destroy pill
			break;
		case WALL:  		//hit a wall and stay there
			mess = "CANNOT GO THERE!";
			// Alex - Should end the game the same way that it does when you quit the program
			//showMessage(clDarkCyan, clWhite, 40, 8, "Big Oof, You are a dead boy.");
			//system("pause");	//hold output screen until a keyboard key is hit
			//End Game
	  //case mouse: - Alex - Should maybe get rid of the mouse and then have the tail of the snake grow by two. Not sure on this one, needs testing
		  //IsMousePresent = false
		  //snake.pushBack(TAIL, TAIL)
			break;
		}

	//if (IsMousePresent == false) // Alex - Supposedly should dump the mouse in a random place if there isn't one present - Mouse logic still needs to be added and changed as to not allow for it to appear in a wall
	//{
	//	mouse.y = random(SIZEY - 2);		//vertical coordinates in range 1-(SIZEY-2)
	//	mouse.x = random(SIZEX - 2);		//horizontal coordinate in range 1-(SIZEX - 2)
	//
	//  IsMousePresent = true;
	//}

}
void updateGrid(char grid[][SIZEX], const char maze[][SIZEX], vector<Item>& snake, Item& mouse, bool& IsMousePresent)
{ //update grid configuration after each move
	void placeMaze(char g[][SIZEX], const char b[][SIZEX]);
	void placeItem(char g[][SIZEX], const Item& spot);
	//bool IsMousePresent = false;
	placeMaze(grid, maze);	//reset the empty maze configuration into grid
	//go through place item for each spot of snake in loop
	for (size_t i(snake.size()-1); i > 0; --i) {			//Place head last so it appears on top (on initial snake)			ONLY LOOPING 3 TIMES (NOT SHOWING HEAD)
		placeItem(grid, snake.at(i));			//set current spot of snake tails
	}
	placeItem(grid, snake.at(0));			//set current spot of snake head

	if (IsMousePresent == false) 
	{
		mouse.y = random(SIZEY - 2);		//vertical coordinates in range 1-(SIZEY-2)
		mouse.x = random(SIZEX - 2);		//horizontal coordinate in range 1-(SIZEX - 2)

		//IsMousePresent = true;
		
        placeItem(grid, mouse);
		IsMousePresent = true;
	}
	placeItem(grid, mouse);
}


void CheatMode(vector<Item>& snake, vector<Item>& cheatSnake) {    //Reset snake     (Take in snake and return length of snake (reference or as int?) before function to be used to restore later

  for (int i(0); i < 4; ++i) {    //Beep Alarm 3 times  (can just \a\a\a?)      //NEED DELAY SO IT DOESN'T DO INSTANTLY
    cout << '\a';	//beep the alarm
    Sleep(100);
  }
	cheatSnake = snake;	//get original snake length before cheating abd send the variable back to main for turning cheat mode off
	snake.resize(3);  //Sets it back to 4
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
void renderGame(const char g[][SIZEX], const string& mess)
{ //display game title, messages, maze, spot and other items on screen
	string tostring(char x);
	string tostring(int x);
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
	void paintGrid(const char g[][SIZEX]);
//TODO: Change the colour of the messages
	//display game title
	showMessage(clBlack, clCyan, 0, 0, "Snek Gam");
//TODO: Make date & time into function

	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
	string str(buffer);

	showMessage(clWhite, clBlue, 40, 15, str);

	
	showMessage(clWhite, clBlue, 40, 0, "FoP Task 1c - February 2019   ");  
	showMessage(clWhite, clBlue, 40, 1, "The Big Oof Squad               ");
	showMessage(clWhite, clBlue, 40, 2, "CS4G2e ");
	showMessage(clWhite, clBlue, 40, 12, "Lewis Birkett,Alex Hughes,Aiden Fleming");		//No Spaces to fit all on one line (change later?)
	//display menu options available

	showMessage(clDarkCyan, clWhite, 40, 3, "TO MOVE - USE KEYBOARD ARROWS ");
	showMessage(clDarkCyan, clWhite, 40, 4, "TO QUIT - ENTER 'Q'           ");

	//print auxiliary messages if any
	showMessage(clBlack, clWhite, 40, 8, mess);	//display current message


	//display grid contents
	paintGrid(g);
}

void paintGrid(const char g[][SIZEX])
{ //display grid content on screen
	selectBackColour(clBlack);
	selectTextColour(clWhite);
	gotoxy(0, 2);
//TODO: Give a diferent colour to the symbol representing Spot
	for (int row(0); row < SIZEY; ++row)
	{
		for (int col(0); col < SIZEX; ++col)
			cout << g[row][col];	//output cell content
		cout << endl;
	}
}

void checkScoreFile(const int score, const string name) {		//independent function at minute, needs integration

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
			if (previousscore < score) {		//new high score
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

void endProgram()
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
	showMessage(clDarkCyan, clWhite, 40, 8, "Quitting Program");
	system("pause");	//hold output screen until a keyboard key is hit
}
