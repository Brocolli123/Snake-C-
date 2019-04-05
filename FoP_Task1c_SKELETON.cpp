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

//to move snake move head to new position and delete the tail	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

int main()
{
	//function declarations (prototypes)
	void initialiseGame(char g[][SIZEX], char m[][SIZEX], vector<Item>& snake);
	void renderGame(const char g[][SIZEX], const string& mess);
	void updateGame(char g[][SIZEX], const char m[][SIZEX], vector<Item>& s, const int kc, string& mess);
	void CheatMode(vector<Item>& snake, vector<Item>& cheatSnake);
	bool wantsToQuit(const int key);
	bool isCheatKey(const int k);
	bool isArrowKey(const int k);
	int  getKeyPress();
	void checkScoreFile(const int score);		//take score in?
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
	bool inCheatMode = false;  //To check if already in cheatmode
	bool hasCheated = false;  //Use later when displaying score to keep it to 0
	size_t cheatLength;

	//have instructions about turning on/off cheat mode (how to turn on by default (is set off at start))
  //when size grows needs to check if incheatmode is true, then if it is only increase the cheatLength not current length

	//action...
	seed();								//seed the random number generator
	SetConsoleTitle("FoP 2018-19 - Task 1c - Game Skeleton");
	initialiseGame(grid, maze,snake);	//initialise grid (incl. walls and spot)
	int key;							//current key selected by player
	do {
		renderGame(grid, message);			//display game info, modified grid and messages
		key = getKeyPress(); 	//read in  selected key: arrow or letter command
		if (isArrowKey(key))
			updateGame(grid, maze, snake, key, message);                             
    else {
      if (toupper(key) == CHEAT) {
        hasCheated = true;            //better way of doing this every time?  Used to stop recording score
        inCheatMode = !inCheatMode;   //flips the bool
        if (inCheatMode == true) {
          //How to use cheat mode message
          message = "CHEAT MODE ON";        //Not displaying this message?
          CheatMode(snake, cheatSnake);
          updateGame(grid, maze, snake, key, message);
        }
        else {    //inCheatMode == False
          message = "CHEAT MODE OFF";
          //snake.resize(cheatSnake.size());   //Set to size of cheatSnake
          //for (size_t i(1); i < cheatSnake.size() - 1; ++i) {
            //snake.at(i).symbol = cheatSnake.at(i).symbol;   //Set symbol of snake at the position to the cheatsnake's 
            //snake.at(i).x = cheatSnake.at(i).x;   //Sets the cheatSnake position to the current position of the snake
            //snake.at(i).y = cheatSnake.at(i).y;
            //what happens past the length of the snake with the extended tail?
          //}
          snake = cheatSnake;   //return to it's pre-cheat length                       (THIS PUTS IT IN THE OLD SNAKE'S POSITION)
          //Update how to use cheat mode message
        }
      }
      //else                                                                                 //ALWAYS RUNNING THROUGH THIS WHY????                  
        //message = "INVALID KEY!";  //set 'Invalid key' message
    }
	} while (!wantsToQuit(key));		//while user does not want to quit
	renderGame(grid, message);			//display game info, modified grid and messages
	endProgram();						//display final message
	return 0;
}


//---------------------------------------------------------------------------
//----- initialise game state
//---------------------------------------------------------------------------

void initialiseGame(char grid[][SIZEX], char maze[][SIZEX], vector<Item>& snake)
{ //initialise grid and place spot in middle
	void setInitialMazeStructure(char maze[][SIZEX]);
	void setSnakeInitialCoordinates(vector<Item>& Snake);
	void updateGrid(char g[][SIZEX], const char m[][SIZEX], vector<Item>& snake);

	setInitialMazeStructure(maze);		//initialise maze
	setSnakeInitialCoordinates(snake);
	updateGrid(grid, maze, snake);		//prepare grid
}

void setSnakeInitialCoordinates(vector<Item>& snake)
{ //set spot coordinates inside the grid at random at beginning of game
   
	snake.at(0).y = random(SIZEY - 2);		//vertical coordinates in range 1-(SIZEY-2)
	snake.at(0).x = random(SIZEX - 2);		//horizontal coordinate in range 1-(SIZEX - 2)
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

void updateGame(char grid[][SIZEX], const char maze[][SIZEX], vector<Item>& snake, const int keyCode, string& mess)
{ //update game
	void updateGameData(const char g[][SIZEX], vector<Item>& s, const int kc, string& m);
	void updateGrid(char g[][SIZEX], const char maze[][SIZEX], vector <Item>& s);		//does vector have to be const
	updateGameData(grid, snake, keyCode, mess);		//move spot in required direction
	updateGrid(grid, maze, snake);					//update grid information
}

void updateGameData(const char g[][SIZEX], vector<Item>& snake, const int key, string& mess)
{ //move spot in required direction
	bool isArrowKey(const int k);
	void setKeyDirection(int k, int& dx, int& dy);
	//assert (isArrowKey(key));                                                                                           //REMOVE FOR NOW SO CAN USE NON ARROW KEYS
 
	//reset message to blank
	mess = "";

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
			//End Game
			break;
		}

}
void updateGrid(char grid[][SIZEX], const char maze[][SIZEX], vector<Item>& snake)
{ //update grid configuration after each move
	void placeMaze(char g[][SIZEX], const char b[][SIZEX]);
	void placeItem(char g[][SIZEX], const Item& spot);
	placeMaze(grid, maze);	//reset the empty maze configuration into grid
	//go through place item for each spot of snake in loop
	for (size_t i(snake.size()-1); i > 0; --i) {			//Place head last so it appears on top (on initial snake)			ONLY LOOPING 3 TIMES (NOT SHOWING HEAD)
		placeItem(grid, snake.at(i));			//set current spot of snake tails
	}
	placeItem(grid, snake.at(0));			//set current spot of snake head
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
	showMessage(clBlack, clYellow, 0, 0, "Snek Gam");
//TODO: Display date and time from the system
	
	//string month;
	//struct tm newtime; // Declares the structure for displaying time
	//time_t now = time(0); // Gets the system time
	//localtime_s(&newtime, &now); // Gets the local time
	//switch (newtime.tm_mon + 1) //Selects the month based on the number contained in newtime
	//{
	//case(1):
	//	month = "January";
	//	break;
	//case(2):
	//	month = "February";
	//	break;
	//case(3):
	//	month = "March";
	//	break;
	//case(4):
	//	month = "April";
	//	break;
	//case(5):
	//	month = "May";
	//	break;
	//case(6):
	//	month = "June";
	//	break;
	//case(7):
	//	month = "July";
	//	break;
	//case(8):
	//	month = "August";
	//	break;
	//case(9):
	//	month = "September";
	//	break;
	//case(10):
	//	month = "October";
	//	break;
	//case(11):
	//	month = "November";
	//	break;
	//default:
	//	month = "December";
	//	break;

	//}
	//cout << newtime.tm_mday << " " << month << " " << (1900 + newtime.tm_year) << ", "; //Outputs the Date
	//cout << newtime.tm_hour << ":" << setfill('0') << setw(2) << newtime.tm_min << ":" << setfill('0') << setw(2) << newtime.tm_sec << endl; //Outputs the Time
	
	
	
	showMessage(clWhite, clRed, 40, 0, "FoP Task 1c - February 2019   ");
//TODO: Show course SE/CS4G/CS, group number, students names and ids        CS4G2e  Lewis Birkett B8018431, Alex Hughes B7022472, Aiden Fleming B8025218
	showMessage(clWhite, clRed, 40, 1, "Pascale Vacher                ");
	//display menu options available
//TODO: Show other options availables when ready...
	showMessage(clRed, clYellow, 40, 3, "TO MOVE - USE KEYBOARD ARROWS ");
	showMessage(clRed, clYellow, 40, 4, "TO QUIT - ENTER 'Q'           ");

	//print auxiliary messages if any
	showMessage(clBlack, clWhite, 40, 8, mess);	//display current message

//TODO: Show your course, your group number and names on screen

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

void checkScoreFile(const int score) {
		string name;
		cout << "\nWhat is the player's name? ";
		cin >> name;
		//get score from program

		ifstream fin;
		fin.open(name + ".txt", ios::in);	//opens file with name as filename. in read mode

		if (fin.fail())
		{
			cout << "\nNew Player: ";		//new player
			ofstream fout;
			fout.open(name + ".txt", ios::out);	//create file
			fout << score;						//set score
			fout.close();
		}
		else { //player already exists
			int previousscore;
			fin >> previousscore;
			if (previousscore < score) {		//if current score is above score in file
				fin.close();
				cout << "\nNew High Score: ";
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
	showMessage(clRed, clYellow, 40, 8, "Quitting Program");	
	system("pause");	//hold output screen until a keyboard key is hit
}
