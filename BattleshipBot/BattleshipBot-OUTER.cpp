/*
* Author: Coobie (Jacob)
* Created: 17/01/2017
* Revised: 22/03/2017
* Description: BattleShip Bots assignment OUTER pattern
* User advice: none
*/

//Included libraries
#include "stdafx.h"
#include <winsock2.h>
#include <math.h>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#pragma comment(lib, "wsock32.lib")

#define SHIPTYPE_BATTLESHIP	"0"
#define SHIPTYPE_FRIGATE	"1"
#define SHIPTYPE_SUBMARINE	"2"

#define STUDENT_NUMBER		"HMS BlowStuffUp"
#define STUDENT_FIRSTNAME	"Coobie"
#define STUDENT_FAMILYNAME	"(Jacob)"

//My ship type
#define MY_SHIP	SHIPTYPE_SUBMARINE

//LOCAL SERVER
#define IP_ADDRESS_SERVER	"127.0.0.1"

//EXTERNAL SERVER
//#define IP_ADDRESS_SERVER ""

#define PORT_SEND	 1924 // We define a port that we are going to use.
#define PORT_RECEIVE 1925 // We define a port that we are going to use.


#define MAX_BUFFER_SIZE	500
#define MAX_SHIPS		200

#define FIRING_RANGE	100

#define MOVE_LEFT		-1
#define MOVE_RIGHT		 1
#define MOVE_UP			 1
#define MOVE_DOWN		-1
#define MOVE_FAST		 2
#define MOVE_SLOW		 1


SOCKADDR_IN sendto_addr;
SOCKADDR_IN receive_addr;

SOCKET sock_send;  // This is our socket, it is the handle to the IO address to read/write packets
SOCKET sock_recv;  // This is our socket, it is the handle to the IO address to read/write packets

WSADATA data1; //Renamed from data due to issue when using iostream

char InputBuffer[MAX_BUFFER_SIZE];

int myX;
int myY;
int myHealth;
int myFlag;
int myType;

//Required for killing stationary ships
int closestPreviousX = 0;
int closestPreviousY = 0;

int myPreviousHealth = 0; //Detect respawn

//Required for pattern
long counter = 0;
int cValue = 250;
int shortCount = 0;
int choiceOnSpawn;

int noEnemyCount = 0; //For swap sides

//Default ship related
int number_of_ships;
int shipX[MAX_SHIPS];
int shipY[MAX_SHIPS];
int shipHealth[MAX_SHIPS];
int shipFlag[MAX_SHIPS];
int shipType[MAX_SHIPS];

bool message = false;
char MsgBuffer[MAX_BUFFER_SIZE];

bool fire = false;
int fireX;
int fireY;

bool moveShip = false;
int moveX;
int moveY;

bool setFlag = true;
int new_flag;

void send_message(char* dest, char* source, char* msg);
void fire_at_ship(int X, int Y);
void move_in_direction(int left_right, int up_down);
void set_new_flag(int newFlag);

int up_down;
int left_right;

int shipDistance[MAX_SHIPS];

int number_of_friends;
int friendX[MAX_SHIPS];
int friendY[MAX_SHIPS];
int friendHealth[MAX_SHIPS];
int friendFlag[MAX_SHIPS];
int friendDistance[MAX_SHIPS];
int friendType[MAX_SHIPS];

int number_of_enemies;
int enemyX[MAX_SHIPS];
int enemyY[MAX_SHIPS];
int enemyHealth[MAX_SHIPS];
int enemyFlag[MAX_SHIPS];
int enemyDistance[MAX_SHIPS];
int enemyType[MAX_SHIPS];

/*
* Function: isaFriend
* Description: checks whether ship is a friendly
* Parameters: index (int) - the index of the ship you want to check
* Returns: boolean
* Warnings: Currently only returns false as don't want any friends
*/
bool IsaFriend(int index)
{
	//bool rc;
	//rc = false;
	//if (shipFlag[index] == 12333124552)
	//{
	//	rc = true;  // I have just seen my friend 12333124552
	//}
	//return rc;
	return false; //Stop any trouble with flags (block all)
}

/*
* Function: goTowards
* Description: Moves the ship towards coordinates
* Parameters: x (int), y (int) - x and y coordinates you want to move towards
* Returns: none (void)
* Warnings: none
*/
void goTowards(int x, int y)
{
	if (myX > x)
	{ // Value of x is greater than myX go towards it means reduce myX
		left_right = MOVE_LEFT*MOVE_FAST;
	}
	else
	{ // Value of x is less than myX go towards it means increase myX
		left_right = MOVE_RIGHT*MOVE_FAST;
	}

	if (myY < y)
	{ // Value of y is less than myY go towards it means increase myY
		up_down = MOVE_UP*MOVE_FAST;
	}
	else
	{ // Value of y is greater than myY go towards it means reduce myY
		up_down = MOVE_DOWN*MOVE_FAST;
	}
}

/*
* Function: outputHeader
* Description: Outputs a nice looking title from the file Header.txt
* Parameters: none
* Returns: none (void)
* Warnings: none
*/
void outputHeader()
{// Reads from file Header.txt and prints it to the command line
	string line;
	ifstream inFile("Header.txt");
	if (inFile.is_open())
	{
		while (getline(inFile, line))
		{
			cout << line << endl;
		}
	}
	else
	{
		cout << "Not open";
	}
	inFile.close();
}

/*
* Function: secondaryOuputHeader
* Description: Outputs a nice looking picture depending on ship type
* Parameters: none
* Returns: none (void)
* Warnings: none
*/
void secondaryOuputHeader()
{ // Reads from different file based on ship type
	string line;

	if (MY_SHIP == "0")
	{
		//Battleship
		ifstream inFile("Battleship.txt");
		if (inFile.is_open())
		{
			while (getline(inFile, line))
			{
				cout << line << endl;
			}
		}
		else
		{
			cout << "Not open";
		}
		inFile.close();
	}
	else if (MY_SHIP == "1")
	{
		//Frigate
		ifstream inFile("Frigate.txt");
		if (inFile.is_open())
		{
			while (getline(inFile, line))
			{
				cout << line << endl;
			}
		}
		else
		{
			cout << "Not open";
		}
		inFile.close();
	}
	else if (MY_SHIP == "2")
	{
		//Submarine
		ifstream inFile("Submarine.txt");
		if (inFile.is_open())
		{
			while (getline(inFile, line))
			{
				cout << line << endl;
			}
		}
		else
		{
			cout << "Not open";
		}
		inFile.close();
	}
}

/*
* Function: inZone
* Description: Tells you whether a set of coordinates are within a square of a determined size of another set of coordinates
* Parameters: X1 (int), Y1 (int) - x and y coordinates you in see are in zone;
* Parameters: zoneX (int), zoneY(int) - x and y coordinates set for the MIDDLE of the zone
* Parameters: width (int), height (int) - The dimensions of the zone ()
* Returns: boolean - true if in zone
* Warnings: none
*/
boolean inZone(int X1, int Y1, int zoneX, int zoneY, int width, int height)
{
	if ((X1 < (zoneX + (.5 * width)) && X1 >(zoneX - (.5 * width)) && Y1 < (zoneY + (.5 * height)) && Y1 >(zoneY - (.5 * height))) == true)
	{ //Coordinates is in zone
		return true;
	}
	else
	{ //Not in the zone
		return false;
	}
}

/*
* Function: zone
* Description: Ouputs the zone which the ship is currently in. 9 zones total
* Parameters: none
* Returns: zoneValue (int) - the zone the ship in currently
* Warnings: 1000 (the size of the map) does not divide by 3 (3x3=9) this means the ship could be outside of a zone (returns 0)
*/
int zone()
{
	int zoneValue = 0;

	if ((inZone(myX, myY, 167, 167, 333, 333) == true))
	{
		//Zone 1
		zoneValue = 1;
	}
	else if ((inZone(myX, myY, 167, 500, 333, 333) == true))
	{
		//Zone 2
		zoneValue = 2;
	}
	else if ((inZone(myX, myY, 167, 833, 333, 333) == true))
	{
		//Zone 3
		zoneValue = 3;
	}
	else if ((inZone(myX, myY, 500, 833, 333, 333) == true))
	{
		//Zone 4
		zoneValue = 4;
	}
	else if ((inZone(myX, myY, 833, 833, 333, 333) == true))
	{
		//Zone 5
		zoneValue = 5;
	}
	else if ((inZone(myX, myY, 833, 500, 333, 333) == true))
	{
		//Zone 6
		zoneValue = 6;
	}
	else if ((inZone(myX, myY, 833, 167, 333, 333) == true))
	{
		//Zone 7
		zoneValue = 7;
	}
	else if ((inZone(myX, myY, 500, 167, 333, 333) == true))
	{
		//Zone 8
		zoneValue = 8;
	}
	else if ((inZone(myX, myY, 500, 500, 333, 333) == true))
	{
		//Zone 9
		zoneValue = 9;
	}

	return zoneValue;
}

/*
* Function: shipWillWin
* Description: Returns the most likely outcome between two ships for type1
* Parameters: type1 (int), type2 (int) - The two ship types you want to compare
* Returns: int - 3 = draw, 2 = lose, 1 = win
* Warnings: none
*/
int shipWillWin(int type1, int type2)
{
	//Ship types are equal
	if (type1 == type2)
	{
		return 3;
	}
	//Battleship vs Frigate
	else if ((type1 == 0) && (type2 == 1))
	{
		return 1;
	}
	//Battleship vs Sub
	else if ((type1 == 0) && (type2 == 2))
	{
		return 2;
	}
	//Frigate vs Battleship
	else if ((type1 == 1) && (type2 == 0))
	{
		return 2;
	}
	//Frigate vs Sub
	else if ((type1 == 1) && (type2 == 2))
	{
		return 1;
	}
	//Sub vs Battleship
	else if ((type1 == 2) && (type2 == 0))
	{
		return 1;
	}
	//Sub vs Frigate
	else if ((type1 == 2) && (type2 == 1))
	{
		return 2;
	}
	else //Default
	{
		return 2;
	}
}

/*
* Function: goAway
* Description: Moves the ship away from coordinates
* Parameters: x (int), y (int) - x and y coordinates you want to move away from
* Returns: none (void)
* Warnings: none
*/
void goAway(int x, int y)
{
	if (myX > x)
	{ // Move right away from x
		left_right = MOVE_RIGHT*MOVE_FAST;
	}
	else
	{ //Move left away from x
		left_right = MOVE_LEFT*MOVE_FAST;
	}

	if (myY > y)
	{ //Move up away from y
		up_down = MOVE_UP*MOVE_FAST;
	}
	else
	{ //Move down away from y
		up_down = MOVE_DOWN*MOVE_FAST;
	}
}

/*
* Function: nearest_enemy
* Description: returns the index of the nearest enemy
* Parameters: none
* Returns: nearest_ship (int) - the index of the closest enemy
* Warnings: none
*/
int nearest_enemy()
{
	int i;
	int nearest_ship = 0;

	//Loop round enemies to find the closest
	for (i = 0; i < number_of_enemies; i++)
	{
		if (enemyDistance[nearest_ship] > enemyDistance[i])
		{
			nearest_ship = i;
		}
	}
	return nearest_ship;
}

/*
* Function: most_damaged_ship
* Description: returns the index of the most damaged enemy
* Parameters: none
* Returns: most_damaged (int) the index of the most damaged enemy
* Warnings: none
*/
int most_damaged_ship()
{
	int i;
	int most_damaged = 0;

	//Loop round the enemies to find the one with the lowest health
	for (i = 0; i < number_of_enemies; i++)
	{
		if (enemyHealth[most_damaged] > enemyHealth[i])
		{
			most_damaged = i;
		}
	}
	return most_damaged;
}

/*
* Function: attack
* Description: fires at and moves towards enemy
* Parameters: i (int) - the index of the enemy
* Returns: none
* Warnings: Only works with enemies
*/
void attack(int i)
{
	fire_at_ship(enemyX[i], enemyY[i]);
	goTowards(enemyX[i], enemyY[i]);
}

/*
* Function: measureDistance
* Description: Calculates distance between two coordinates
* Parameters: x1 (int), y1 (int) - first set of coordinates
* Parameters: x2 (int), y2 (int) - second set of coordinates
* Returns: output (int) - the distance between the two sets of coordinates
* Warnings: none
*/
int measureDistance(int x1, int y1, int x2, int y2)
{ 
	int output = (int)sqrt((double)((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2)));

	return output;
}

/*
* Function: killStationary
* Description: Attacks the nearest ship if it has not moved
* Parameters: none
* Returns: none (void)
* Warnings: none
*/
void killStationary()
{
	if (number_of_enemies != 0)
	{
		if ((enemyX[nearest_enemy()] == closestPreviousX) && (enemyY[nearest_enemy()] == closestPreviousY))
		{ //They haven't moved
			attack(nearest_enemy());
		}
		//Store the coordinates of the nearest enemy to check next time
		closestPreviousX = enemyX[nearest_enemy()];
		closestPreviousY = enemyY[nearest_enemy()];
	}
}

/*
* Function: setRespawnZone
* Description: Sets which point of the pattern to go to first on spawn
* Parameters: none
* Returns: none (void)
* Warnings: none
*/
void setRespawnZone()
{
	if ((zone() != 9) && (zone() != 0))
	{ //Go to point of the pattern in the spawn zone
		counter = cValue*(zone() - 1);
	}
	else if (zone() == 0)
	{ //Spawned outside the zones, find closest outer point
		int dist1 = abs(measureDistance(myX, myY, 200, 200));
		int dist2 = abs(measureDistance(myX, myY, 200, 800));
		int dist3 = abs(measureDistance(myX, myY, 800, 800));
		int dist4 = abs(measureDistance(myX, myY, 800, 200));

		if ((dist1 < dist2) && (dist1 < dist3) && (dist1 < dist4))
		{
			counter = cValue * 0;
		}
		else if ((dist2 < dist1) && (dist2 < dist3) && (dist2 < dist4))
		{
			counter = cValue * 2;
		}
		else if ((dist3 < dist1) && (dist3 < dist2) && (dist3 < dist4))
		{
			counter = cValue * 4;
		}
		else if ((dist4 < dist1) && (dist4 < dist2) && (dist4 < dist3))
		{
			counter = cValue * 6;
		}
		else
		{
			counter = cValue * 0;
		}
	}
	else if (zone() == 9)
	{ //Ship is in the middle move to the closest point
		int distLeft = abs(measureDistance(myX, myY, 250, 500));
		int distUp = abs(measureDistance(myX, myY, 500, 750));
		int distRight = abs(measureDistance(myX, myY, 750, 500));
		int distDown = abs(measureDistance(myX, myY, 500, 250));

		if ((distLeft < distUp) && (distLeft < distRight) && (distLeft < distDown))
		{
			counter = cValue * 1;
		}
		else if ((distUp < distLeft) && (distUp < distRight) && (distUp < distDown))
		{
			counter = cValue * 3;
		}
		else if ((distRight < distLeft) && (distRight < distUp) && (distRight < distDown))
		{
			counter = cValue * 5;
		}
		else if ((distDown < distUp) && (distDown < distLeft) && (distDown < distRight))
		{
			counter = cValue * 7;
		}
		else
		{
			counter = cValue * 7;
		}
	}
}

/*
* Function: checkTypeAndHealth
* Description: Checks if the enemies are a ship that we would lose to and that if they are their health is above a value
* Parameters: none
* Returns: output (bool) - true is contains ship we would lose to and health is above value
* Warnings: none
*/
bool checkTypeAndHealth()
{
	bool output = false;
	int i;
	//Loops over each enemy looking for a ship that we would lose to with health over 4.
	for (i = 0; i <= number_of_enemies; i++)
	{
		if ((shipWillWin(myType, enemyType[i]) == 2) && (enemyHealth[i] > 4))
		{ //We would lose to them
			output = true;
		}
	}
	return output;
}

/*
* Function: spawnPattern
* Description: Generates a random number on spawn which chooses the pattern
* Parameters: none
* Returns: none (void)
* Warnings: If spawan - death occurs the random number might be the same
*/
void spawnPattern()
{
	srand(time(NULL));
	//Random number to choose path between 1 and 10.
	choiceOnSpawn = rand() % 10 + 1;
}

/*
* Function: goToPattern
* Description: Moves around the pattern
* Parameters: index (int) - pattern index
* Returns: none (void)
* Warnings: none
*/
void goToPattern(int index)
{ // If choiceOnSpawn is 1 to 5 then normal pattern for that life, 6 to 10 then extreme pattern
	if (choiceOnSpawn <= 5)
	{
		//Normal
		int locationsXY1[8][2] = { { 200,200 },{ 250,500 },{ 200,800 },{ 500,750 },{ 800,800 },{ 750,500 },{ 800,200 },{ 500,250 } };
		goTowards(locationsXY1[index][0], locationsXY1[index][1]);
	}
	else if (choiceOnSpawn > 5)
	{
		//Extreme
		int locationsXY2[8][2] = { { 200,200 },{ 330,500 },{ 200,800 },{ 500,670 },{ 800,800 },{ 670,500 },{ 800,200 },{ 500,330 } };
		goTowards(locationsXY2[index][0], locationsXY2[index][1]);
	}
}

/*
* Function: moveOn
* Description: when the ship reaches a point on the pattern it will move on to the next after a short period has passed
* Parameters: index (int) - pattern index
* Returns: int the index of the closest ship
* Warnings: none
*/
void moveOn(int index)
{
	if (choiceOnSpawn <= 5)
	{
		//Normal
		int locationsXY1[8][2] = { { 200,200 },{ 250,500 },{ 200,800 },{ 500,750 },{ 800,800 },{ 750,500 },{ 800,200 },{ 500,250 } };
		if (inZone(myX, myY, locationsXY1[index][0], locationsXY1[index][1], 10, 10) == true)
		{
			shortCount++;
		}
	}
	else if (choiceOnSpawn > 5)
	{
		//Extreme
		int locationsXY2[8][2] = { { 200,200 },{ 330,500 },{ 200,800 },{ 500,670 },{ 800,800 },{ 670,500 },{ 800,200 },{ 500,330 } };
		if (inZone(myX, myY, locationsXY2[index][0], locationsXY2[index][1], 10, 10) == true)
		{
			shortCount++;
		}
	}

	if (shortCount >= 6)
	{ //Been at point for long enough
		counter = cValue*(zone());
		shortCount = 0;
	}
}

/*
* Function: shipInRange
* Description: If there is a ship within range shoot it
* Parameters: ship (int) enemy you want to shoot (normally closest)
* Returns: none (void)
* Warnings: none
*/
void shipInRange(int ship)
{
	if (inZone(myX, myY, enemyX[ship], enemyY[ship], 100, 100) == true)
	{
		fire_at_ship(enemyX[ship], enemyY[ship]);
	}
}

/*
* Function: swapSides
* Description: swaps sides to go towards for the pattern
* Parameters: none
* Returns: none (void)
* Warnings: none
*/
void swapSides()
{ //This means that the index will equal the opposite point of the pattern
	if (zone() >= 5)
	{// zone is 1 more than index so when zone() equals 1 index is 0.
		counter = cValue*((zone() - 5)); 
	}
	else
	{
		counter = cValue*((zone() + 3));
	}
}

/*
* Function: resetNoEnemy
* Description: resets the counter for no enemy - use for when there are enemies or on respawn
* Parameters: none
* Returns: none (void)
* Warnings: none
*/
void resetNoEnemy()
{
	noEnemyCount = 0;
}

/*
* Function: noEnemy
* Description: if there are no enemies the counter increments and moves on if none seen in a while
* Parameters: none
* Returns: none (void)
* Warnings: none
*/
void noEnemy()
{
	noEnemyCount++;
	if ((noEnemyCount > 260) && (zone() != 9) && (zone() != 0))
	{ //No enemies have been seen for a while so swap sides.
		resetNoEnemy();
		swapSides();
	}
}

/*
* Function: flagSetter
* Description: Changes my flag to closest enemies if they have one otherwise sets as default friend one
* Parameters: none
* Returns: none (void)
* Warnings: none
*/
void flagSetter()
{
	if (number_of_enemies != 0)
	{
		int nearestShip = nearest_enemy();

		if (enemyFlag[nearestShip] != 0)
		{ //If the nearest enemy doesn't have a flag of zero, use their flag
			set_new_flag(enemyFlag[nearestShip]);
		}
		else
		{ //Their flag must be zero so use the default flag
			set_new_flag(123);
		}
	}
	else
	{ //When there are no enemies have a flag of zero
		set_new_flag(0);
	}
}

/*
* Function: establishShips
* Description: In default code this is located at the top of tactics, called just before tactics
* Parameters: none
* Returns: none (void)
* Warnings: none
*/
void establishShips()
{
	int i;
	number_of_friends = 0;
	number_of_enemies = 0;

	if (number_of_ships > 1)
	{
		for (i = 1; i < number_of_ships; i++)
		{
			shipDistance[i] = (int)sqrt((double)((shipX[i] - shipX[0])*(shipX[i] - shipX[0]) + (shipY[i] - shipY[0])*(shipY[i] - shipY[0])));
		}

		for (i = 1; i < number_of_ships; i++)
		{
			if (IsaFriend(i))
			{
				friendX[number_of_friends] = shipX[i];
				friendY[number_of_friends] = shipY[i];
				friendHealth[number_of_friends] = shipHealth[i];
				friendFlag[number_of_friends] = shipFlag[i];
				friendDistance[number_of_friends] = shipDistance[i];
				friendType[number_of_friends] = shipType[i];
				number_of_friends++;
			}
			else
			{
				enemyX[number_of_enemies] = shipX[i];
				enemyY[number_of_enemies] = shipY[i];
				enemyHealth[number_of_enemies] = shipHealth[i];
				enemyFlag[number_of_enemies] = shipFlag[i];
				enemyDistance[number_of_enemies] = shipDistance[i];
				enemyType[number_of_enemies] = shipType[i];
				number_of_enemies++;
			}
		}
	}
}

/*
* Function: tactics
* Description: Where most of the tactic code resides
* Parameters: none
* Returns: none (void)
* Warnings: none
*/
void tactics() //OUTER VERSION
{
	int nearestShip = nearest_enemy();
	int mostDamagedShip = most_damaged_ship();

	if ((myPreviousHealth < myHealth)) //Check for respawn
	{
		spawnPattern();
		setRespawnZone();
		resetNoEnemy();
	}

	//Pattern related code
	int index;
	counter++;
	index = abs(((counter / cValue) % 8)); // set pattern index
	moveOn(index);

	flagSetter();//Flag setter

	//No enemies
	if (number_of_enemies < 1)
	{
		//No enemies carry out pattern set
		goToPattern(index);
		noEnemy();
	}

	//Only one enemy
	else if (number_of_enemies == 1)
	{
		resetNoEnemy();
		//If in fight there is a disadvantage
		if (shipWillWin(myType, enemyType[0]) == 2)
		{
			if (enemyHealth[0] <= 4)
			{ //They are on low health
				attack(0);
			}
			else
			{
				//Avoid the enemy
				goAway(enemyX[0], enemyY[0]);
			}
		}

		//If the ship types are equal
		else if (shipWillWin(myType, enemyType[0]) == 3)
		{
			if (myHealth == enemyHealth[0])
			{ //Health is equal
				if (abs(measureDistance(myX,myY,enemyX[0],enemyY[0])) < 60)
				{ //Only attack them if we are within 60
					attack(0);
				}
				else
				{ //Carry on with pattern
					goToPattern(index);
				}
			}
			else if (myHealth > enemyHealth[0])
			{ //My health is greater than theirs
				attack(0);
			}
			else
			{ //Their health is greater
				goAway(enemyX[0], enemyY[0]);
			}
		}

		//If the odds are that we will win
		else if (shipWillWin(myType, enemyType[0]) == 1)
		{
			attack(0);
		}
	}//End of 1 enemy

	 //There are 2 enemy ships
	else if (number_of_enemies == 2)
	{
		resetNoEnemy();
		
		if (inZone(enemyX[0], enemyY[0], enemyX[1], enemyY[1], 30, 30) == false)
		{ //Two ships are not together
			if (shipWillWin(myType, enemyType[nearestShip]) == 1)
			{  //Nearest is weak against us
				attack(nearestShip);
			}
			else if (((shipWillWin(myType, enemyType[nearestShip]) == 2) || (shipWillWin(myType,enemyType[nearestShip]) == 3)) && (enemyHealth[nearestShip] <= 4))
			{ //Nearest should be vulnerable to us
				attack(nearestShip);
			}
			else
			{ //We would come off badly in a fight so move away
				goAway(enemyX[nearestShip], enemyY[nearestShip]);
			}

		}
		else //Two ships are together
		{
			if ((shipWillWin(myType, enemyType[0]) == 1) && (enemyType[0] == enemyType[1]))
			{ //Both what we win against
				attack(nearestShip);
			}
			else if ((shipWillWin(myType,enemyType[0]) == 3) && (enemyType[0] == enemyType[1]) && (enemyHealth[0] > 4) && (enemyHealth[1] > 4))
			{ //Both same type as us and above health
				goAway(enemyX[nearestShip], enemyY[nearestShip]);
			}
			else if (checkTypeAndHealth() == false)
			{ //Checks if group contains what we lose to with plenty of health
				attack(nearestShip);
			}
			else
			{
				goAway(enemyX[nearestShip], enemyY[nearestShip]);
			}
		}
	}
	else if (number_of_enemies >= 3)
	{
		resetNoEnemy();

		//TODO: find out if nearest ship is seperated
		bool nearestGroup = true;
		int areaSize = 35;
		if (nearestShip == 0)
		{ //Checks if the nearest ship is in a group with the others
			if ((inZone(enemyX[nearestShip], enemyY[nearestShip], enemyX[1], enemyY[1], areaSize, areaSize) == false) && (inZone(enemyX[nearestShip], enemyY[nearestShip], enemyX[2], enemyY[2], areaSize, areaSize) == false))
			{
				nearestGroup = false;
			}
		}
		else if (nearestShip == 1)
		{//Checks if the nearest ship is in a group with the others
			if ((inZone(enemyX[nearestShip], enemyY[nearestShip], enemyX[0], enemyY[0], areaSize, areaSize) == false) && (inZone(enemyX[nearestShip], enemyY[nearestShip], enemyX[2], enemyY[2], areaSize, areaSize) == false))
			{
				nearestGroup = false;
			}
		}
		else if (nearestShip == 2)
		{//Checks if the nearest ship is in a group with the others
			if ((inZone(enemyX[nearestShip], enemyY[nearestShip], enemyX[1], enemyY[1], areaSize, areaSize) == false) && (inZone(enemyX[nearestShip], enemyY[nearestShip], enemyX[0], enemyY[0], areaSize, areaSize) == false))
			{
				nearestGroup = false;
			}
		}
		else if (nearestShip == 3)
		{//Checks if the nearest ship is in a group with the others
			if ((inZone(enemyX[nearestShip], enemyY[nearestShip], enemyX[1], enemyY[1], areaSize, areaSize) == false) && (inZone(enemyX[nearestShip], enemyY[nearestShip], enemyX[0], enemyY[0], areaSize, areaSize) == false) && (inZone(enemyX[nearestShip], enemyY[nearestShip], enemyX[2], enemyY[2], areaSize, areaSize) == false))
			{
				nearestGroup = false;
			}
		}

		if (nearestGroup == true)
		{ //The nearest ship is in the group
			if (checkTypeAndHealth() == true)
			{ //Contains a ship that we lose to with health above value
				goAway(enemyX[nearestShip], enemyY[nearestShip]);
			}
			else
			{ //Attack them
				attack(nearestShip);
			}
		}
		else
		{ //Nearest ship is not in the group			
			if (shipWillWin(myType,enemyType[nearestGroup]) == 1)
			{ //Nearest which is not in group is weak to us
				attack(nearestShip);
			}
			else if ((shipWillWin(myType, enemyType[nearestShip]) == 3) && (myHealth > enemyHealth[nearestShip]))
			{ //Nearest which is not in group is our type but has less health
				attack(nearestShip);
			}
			else if ((shipWillWin(myType, enemyType[nearestShip]) == 2) && (enemyHealth[nearestShip] <= 4))
			{ //Nearest which is not in group should win against us however is on low health
				attack(nearestShip);
			}
			else
			{ //It would not be favourable to attack them
				goAway(enemyX[nearestShip], enemyY[nearestShip]);
			}
		}
		
	}

	//Checks to override other commands
	if (number_of_enemies >= 1)
	{
		killStationary();
		shipInRange(nearestShip);
	}

	//Checks required for respawn detection
	myPreviousHealth = myHealth;
	move_in_direction(left_right, up_down);
}

/*
* Function: messageReceived
* Description: Checks if the message recieved is matching the required criteria
* Parameters: msg (char*)
* Returns: none (void)
* Warnings: none
*/
void messageReceived(char* msg)
{
	/*int X;
	int Y;

	printf("%s\n", msg);

	if (sscanf_s(msg, "Message 12345678, 23456789, Im at %d %d", &X, &Y) == 2)
	{
		printf("My friend is at %d %d\n", X, Y);
	}*/
}


/*************************************************************/
/********* Your tactics code ends here ***********************/
/*************************************************************/

void communicate_with_server()
{
	char buffer[4096];
	int  len = sizeof(SOCKADDR);
	char chr;
	bool finished;
	int  i;
	int  j;
	int  rc;
	char* p;

	sprintf_s(buffer, "Register  %s,%s,%s,%s", STUDENT_NUMBER, STUDENT_FIRSTNAME, STUDENT_FAMILYNAME, MY_SHIP);
	sendto(sock_send, buffer, strlen(buffer), 0, (SOCKADDR *)&sendto_addr, sizeof(SOCKADDR));

	while (true)
	{
		if (recvfrom(sock_recv, buffer, sizeof(buffer) - 1, 0, (SOCKADDR *)&receive_addr, &len) != SOCKET_ERROR)
		{
			p = ::inet_ntoa(receive_addr.sin_addr);

			if ((strcmp(IP_ADDRESS_SERVER, "127.0.0.1") == 0) || (strcmp(IP_ADDRESS_SERVER, p) == 0))
			{
				if (buffer[0] == 'M')
				{
					messageReceived(buffer);
				}
				else
				{
					i = 0;
					j = 0;
					finished = false;
					number_of_ships = 0;

					while ((!finished) && (i<4096))
					{
						chr = buffer[i];

						switch (chr)
						{
						case '|':
							InputBuffer[j] = '\0';
							j = 0;
							sscanf_s(InputBuffer, "%d,%d,%d,%d,%d", &shipX[number_of_ships], &shipY[number_of_ships], &shipHealth[number_of_ships], &shipFlag[number_of_ships], &shipType[number_of_ships]);
							number_of_ships++;
							break;

						case '\0':
							InputBuffer[j] = '\0';
							sscanf_s(InputBuffer, "%d,%d,%d,%d,%d", &shipX[number_of_ships], &shipY[number_of_ships], &shipHealth[number_of_ships], &shipFlag[number_of_ships], &shipType[number_of_ships]);
							number_of_ships++;
							finished = true;
							break;

						default:
							InputBuffer[j] = chr;
							j++;
							break;
						}
						i++;
					}

					myX = shipX[0];
					myY = shipY[0];
					myHealth = shipHealth[0];
					myFlag = shipFlag[0];
					//myType = shipType[0];

					if (MY_SHIP == "0")
					{
						//Battleship
						myType = 0;
					}
					else if (MY_SHIP == "1")
					{
						//Frigate
						myType = 1;
					}
					else if (MY_SHIP == "2")
					{
						//Sub
						myType = 2;
					}
				}
				establishShips();
				tactics();

				if (message)
				{
					sendto(sock_send, MsgBuffer, strlen(MsgBuffer), 0, (SOCKADDR *)&sendto_addr, sizeof(SOCKADDR));
					message = false;
				}

				if (fire)
				{
					sprintf_s(buffer, "Fire %s,%d,%d", STUDENT_NUMBER, fireX, fireY);
					sendto(sock_send, buffer, strlen(buffer), 0, (SOCKADDR *)&sendto_addr, sizeof(SOCKADDR));
					fire = false;
				}

				if (moveShip)
				{
					sprintf_s(buffer, "Move %s,%d,%d", STUDENT_NUMBER, moveX, moveY);
					rc = sendto(sock_send, buffer, strlen(buffer), 0, (SOCKADDR *)&sendto_addr, sizeof(SOCKADDR));
					moveShip = false;
				}

				if (setFlag)
				{
					sprintf_s(buffer, "Flag %s,%d", STUDENT_NUMBER, new_flag);
					sendto(sock_send, buffer, strlen(buffer), 0, (SOCKADDR *)&sendto_addr, sizeof(SOCKADDR));
					setFlag = false;
				}

			}
		}
		else
		{
			printf_s("recvfrom error = %d\n", WSAGetLastError());
		}
	}

	printf_s("Student %s\n", STUDENT_NUMBER);
}



void send_message(char* dest, char* source, char* msg)
{
	message = true;
	sprintf_s(MsgBuffer, "Message %s,%s,%s,%s", STUDENT_NUMBER, dest, source, msg);
}



void fire_at_ship(int X, int Y)
{
	fire = true;
	fireX = X;
	fireY = Y;
}



void move_in_direction(int X, int Y)
{
	if (X < -2) X = -2;
	if (X >  2) X = 2;
	if (Y < -2) Y = -2;
	if (Y >  2) Y = 2;

	moveShip = true;
	moveX = X;
	moveY = Y;
}


void set_new_flag(int newFlag)
{
	setFlag = true;
	new_flag = newFlag;
}



int _tmain(int argc, _TCHAR* argv[])
{
	char chr = '\0';

	printf("\n");
	outputHeader();
	printf("UWE Computer and Network Systems Assignment 2 (2016-17)\n");
	printf("\n");
	secondaryOuputHeader();
	printf("\n");

	if (WSAStartup(MAKEWORD(2, 2), &data1) != 0) return(0);

	//sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);  // Here we create our socket, which will be a UDP socket (SOCK_DGRAM).
	//if (!sock)
	//{
	//	printf("Socket creation failed!\n");
	//}

	sock_send = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);  // Here we create our socket, which will be a UDP socket (SOCK_DGRAM).
	if (!sock_send)
	{
		printf("Socket creation failed!\n");
	}

	sock_recv = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);  // Here we create our socket, which will be a UDP socket (SOCK_DGRAM).
	if (!sock_recv)
	{
		printf("Socket creation failed!\n");
	}

	memset(&sendto_addr, 0, sizeof(SOCKADDR_IN));
	sendto_addr.sin_family = AF_INET;
	sendto_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS_SERVER);
	sendto_addr.sin_port = htons(PORT_SEND);

	memset(&receive_addr, 0, sizeof(SOCKADDR_IN));
	receive_addr.sin_family = AF_INET;
	//	receive_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS_SERVER);
	receive_addr.sin_addr.s_addr = INADDR_ANY;
	receive_addr.sin_port = htons(PORT_RECEIVE);

	int ret = bind(sock_recv, (SOCKADDR *)&receive_addr, sizeof(SOCKADDR));
	//	int ret = bind(sock_send, (SOCKADDR *)&receive_addr, sizeof(SOCKADDR));
	if (ret)
	{
		printf("Bind failed! %d\n", WSAGetLastError());
	}

	communicate_with_server();

	closesocket(sock_send);
	closesocket(sock_recv);
	WSACleanup();

	while (chr != '\n')
	{
		chr = getchar();
	}

	return 0;
}
