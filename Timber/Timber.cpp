// Hello_SFML.cpp : This file contains the 'main' function. Program execution begins and ends there.


#include <iostream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

//Makes code easier to type using namespace
using namespace sf;

//Function declaration
void updateBranches(int seed);
const int NUM_BRANCHES = 6;
Sprite branches[NUM_BRANCHES];

//Position of the player/branch
enum class side { LEFT, RIGHT, NONE };
side branchPositions[NUM_BRANCHES];

int main()
{
    //Create a video mode object
	VideoMode vm(1920, 1080);

	//Create and open a window for the game
	RenderWindow window(vm, "Timber!!", Style::Fullscreen);

	//Create a texture to hold a graphic on the GPU
	Texture textureBackground;

	//Load graphic into the texture
	textureBackground.loadFromFile("graphics/background.png");

	//Create a sprite
	Sprite spriteBackground;

	//Attach the texture to the sprite
	spriteBackground.setTexture(textureBackground);

	//Set sprite background to cover the screen
	spriteBackground.setPosition(0, 0);

	//Make a tree sprite
	Texture textureTree;
	textureTree.loadFromFile("graphics/tree.png");
	Sprite spriteTree;
	spriteTree.setTexture(textureTree);
	spriteTree.setPosition(810, 0);

	//Prepare the bee
	Texture textureBee;
	textureBee.loadFromFile("graphics/bee.png");
	Sprite spriteBee;
	spriteBee.setTexture(textureBee);
	spriteBee.setPosition(0, 800);
	//Is the bee currently moving
	bool beeActive = false;
	//Speed of the bee
	float beeSpeed = 0.0f;

	//Making 3 clouds
	Texture textureCloud;
	textureCloud.loadFromFile("graphics/cloud.png");
	Sprite spriteCloud1;
	Sprite spriteCloud2;
	Sprite spriteCloud3;
	spriteCloud1.setTexture(textureCloud);
	spriteCloud2.setTexture(textureCloud);
	spriteCloud3.setTexture(textureCloud);
	spriteCloud1.setPosition(0, 0);
	spriteCloud2.setPosition(0, 250);
	spriteCloud3.setPosition(0, 500);
	
	bool cloud1Active = false;
	bool cloud2Active = false;
	bool cloud3Active = false;
	float cloud1Speed = 0.0f;
	float cloud2Speed = 0.0f;
	float cloud3Speed = 0.0f;

	//Variable to control time
	Clock clock;

	//Time bar
	RectangleShape timebar;
	float timeBarStartWidth = 400;
	float timeBarHeight = 80;
	timebar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
	timebar.setFillColor(Color::Red);
	timebar.setPosition((1920 / 2) - (timeBarStartWidth / 2), 980);

	Time gameTimeTotal;
	float timeReamaining = 6.0f;
	float timeBarWidthPerSec = timeBarStartWidth / timeReamaining;

	//Track whether the game is running
	bool paused = true;

	//Draw some text
	int score = 0;
	int highscore = 0;
	sf::Text messageText;
	sf::Text scoreText;
	sf::Text highscoreText;

	//Choosing a font
	Font font;
	font.loadFromFile("fonts/KOMIKAP_.ttf");

	//Set font to our message and score text
	messageText.setFont(font);
	scoreText.setFont(font);
	highscoreText.setFont(font);

	//Assign the message
	messageText.setString("Press Enter to start the game");
	scoreText.setString("Score = 0");
	highscoreText.setString("Highscore = 0");

	//Make the font size bigger
	messageText.setCharacterSize(75);
	scoreText.setCharacterSize(100);
	highscoreText.setCharacterSize(100);

	//Choose a color for the text
	messageText.setFillColor(Color::White);
	scoreText.setFillColor(Color::White);
	highscoreText.setFillColor(Color::White);

	//Position the text
	FloatRect textRect = messageText.getLocalBounds();
	messageText.setOrigin(
		textRect.left + textRect.width/2.0f,
		textRect.top + textRect.height/2.0f
	);
	
	messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
	scoreText.setPosition(20, 20);
	highscoreText.setPosition(1150, 20);

	//Prepare 6 branches
	Texture textureBranch;
	textureBranch.loadFromFile("graphics/branch.png");
	//Set texture for each branch sprite
	for (int i = 0; i < NUM_BRANCHES; i++) {
		branches[i].setTexture(textureBranch);
		branches[i].setPosition(-2000, -2000);
		//Set the sprite's origin to dead center
		//We can then spin it round without changing it's position
		branches[i].setOrigin(220, 20);
	}

	//Prepare the player
	Texture texturePlayer;
	texturePlayer.loadFromFile("graphics/player.png");
	Sprite spritePlayer;
	spritePlayer.setTexture(texturePlayer);
	spritePlayer.setPosition(580, 720);

	//Player starts at left
	side playerSide = side::LEFT;

	//Prepare the gravestone
	Texture textureRIP;
	textureRIP.loadFromFile("graphics/rip.png");
	Sprite spriteRIP;
	spriteRIP.setTexture(textureRIP);
	spriteRIP.setPosition(600, 860);

	//Prepare the axe
	Texture textureAxe;
	textureAxe.loadFromFile("graphics/axe.png");
	Sprite spriteAxe;
	spriteAxe.setTexture(textureAxe);
	spriteAxe.setPosition(700, 830);

	//Axe positions to line up with the tree
	const float AXE_POS_LEFT = 700;
	const float AXE_POS_RIGHT = 1075;

	//Prepare the flying log
	Texture textureLog;
	textureLog.loadFromFile("graphics/log.png");
	Sprite spriteLog;
	spriteLog.setTexture(textureLog);
	spriteLog.setPosition(810, 720);

	//Log movement variables
	bool logActive = false;
	float logSpeedX = 1000;
	float logSpeedY = -1500;

	//Control player input
	bool acceptInput = false;

	//Prepare the sounds
	//Chop sound
	SoundBuffer chopBuffer;
	chopBuffer.loadFromFile("sound/chop.wav");
	Sound chop;
	chop.setBuffer(chopBuffer);
	
	//Death sound
	SoundBuffer deathBuffer;
	deathBuffer.loadFromFile("sound/death.wav");
	Sound death;
	death.setBuffer(deathBuffer);
	
	//Out of time sound
	SoundBuffer ootBuffer;
	ootBuffer.loadFromFile("sound/out_of_time.wav");
	Sound outOfTime;
	outOfTime.setBuffer(ootBuffer);


	while (window.isOpen()) {
		/*
		***************************************************
		Handle player's input
		***************************************************
		*/

		//When the player releases the key
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::KeyReleased && !paused) {
				//Listen for key presses again
				acceptInput = true;
				//Hide the axe
				spriteAxe.setPosition(2000, spriteAxe.getPosition().y);
			}
		}

		//Exit the game
		if (Keyboard::isKeyPressed(Keyboard::Escape)) {
			window.close();
		}

		//Start the game
		if (Keyboard::isKeyPressed(Keyboard::Return)) {
			paused = false;

			//Reset the time and the score
			score = 0;
			timeReamaining = 6;

			//Make all the branches disappear
			for (int i = 1; i < NUM_BRANCHES; i++) {
				branchPositions[i] = side::NONE;
			}

			//Make sure the gravestone is hidden
			spriteRIP.setPosition(675, 2000);

			//Move the player into position
			spritePlayer.setPosition(580, 720);
			acceptInput = true;
		}

		//Wrap the player controls to make sure we are accepting input
		if (acceptInput) {

			//Handle the Right cursor key
			if (Keyboard::isKeyPressed(Keyboard::Right)) {
				//Make sure the player is on the right
				playerSide = side::RIGHT;
				score++;
				if (score >= highscore) {
					highscore = score;
				}
				//Add to the amount of time remaining
				timeReamaining += (2 / score) + 0.15;
				//Set the player and axe sprites to the right
				spriteAxe.setPosition(AXE_POS_RIGHT, spriteAxe.getPosition().y);
				spritePlayer.setPosition(1200, 720);
				//Update the branches
				updateBranches(score);
				//Set the log flying to the left
				spriteLog.setPosition(810, 720);
				logSpeedX = -5000;
				logActive = true;

				acceptInput = false;

				//Chop sound
				chop.play();
			}

			//Handle the Left cursor key
			if (Keyboard::isKeyPressed(Keyboard::Left)) {
				//Make sure the player is on the left
				playerSide = side::LEFT;
				score++;
				if (score >= highscore) {
					highscore = score;
				}
				//Add to the amount of time remaining
				timeReamaining += (2 / score) + 0.15;
				//Set the player and axe sprites to the right
				spriteAxe.setPosition(AXE_POS_LEFT, spriteAxe.getPosition().y);
				spritePlayer.setPosition(580, 720);
				//Update the branches
				updateBranches(score);
				//Set the log flying to the left
				spriteLog.setPosition(810, 720);
				logSpeedX = 5000;
				logActive = true;

				acceptInput = false;

				//Chop sound
				chop.play();
			}
		}

		/*
		***************************************************
		Update the screen
		***************************************************
		*/

		if (!paused) {

			//Measure time
			Time dt = clock.restart();

			//Subtract from the amount of time remaining
			timeReamaining -= dt.asSeconds();
			//Set size of timebar
			timebar.setSize(Vector2f(timeBarWidthPerSec * timeReamaining, timeBarHeight));
			//Player runs out of time
			if (timeReamaining <= 0.0f) {
				//Pause the game
				paused = true;
				//Change the message
				messageText.setString("Out of time!!");
				//Reposition the text based on it's size
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(
					textRect.left + textRect.width / 2.0f,
					textRect.top + textRect.height / 2.0f
				);
				messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

				//Out of time sound
				outOfTime.play();
			}

			//Set up the bee
			if (!beeActive) {

				//Give the bee a random speed
				srand((int)time(0) * 10);
				beeSpeed = (rand() % 200) + 200;

				//Give the bee a random height
				srand((int)time(0) * 10);
				float height = (rand() % 500) + 500;
				spriteBee.setPosition(2000, height);
				beeActive = true;
			}

			//Move the bee
			else {
				spriteBee.setPosition(
					spriteBee.getPosition().x - (beeSpeed * dt.asSeconds()),
					spriteBee.getPosition().y
				);
				//After the bee crosses the left edge of the screen
				if (spriteBee.getPosition().x < -100) {
					//Set up a new bee
					beeActive = false;
				}
			}

			//Set up and move the clouds
			//Cloud1 setup
			if (!cloud1Active) {

				//Give the cloud a random speed
				srand((int)time(0) * 10);
				cloud1Speed = (rand() % 200) + 50;

				//Give the cloud a random height
				srand((int)time(0) * 10);
				float height = (rand() % 150);
				spriteCloud1.setPosition(-300, height);
				cloud1Active = true;

			}
			//Move cloud1
			else {
				spriteCloud1.setPosition(
					spriteCloud1.getPosition().x + (cloud1Speed * dt.asSeconds()),
					spriteCloud1.getPosition().y
				);
				//After the cloud crosses the right edge of the screen
				if (spriteCloud1.getPosition().x > 1950) {
					cloud1Active = false;
				}
			}

			//Cloud2 setup
			if (!cloud2Active) {

				//Give the cloud a random speed
				srand((int)time(0) * 20);
				cloud2Speed = (rand() % 200) + 50;

				//Give the cloud a random height
				srand((int)time(0) * 20);
				float height = (rand() % 300) - 150;
				spriteCloud2.setPosition(-300, height);
				cloud2Active = true;

			}
			//Move cloud2
			else {
				spriteCloud2.setPosition(
					spriteCloud2.getPosition().x + (cloud2Speed * dt.asSeconds()),
					spriteCloud2.getPosition().y
				);
				//After the cloud crosses the right edge of the screen
				if (spriteCloud2.getPosition().x > 1950) {
					cloud2Active = false;
				}
			}

			//Cloud3 setup
			if (!cloud3Active) {

				//Give the cloud a random speed
				srand((int)time(0) * 30);
				cloud3Speed = (rand() % 200) + 50;

				//Give the cloud a random height
				srand((int)time(0) * 30);
				float height = (rand() % 450) - 150;
				spriteCloud3.setPosition(-300, height);
				cloud3Active = true;

			}
			//Move cloud3
			else {
				spriteCloud3.setPosition(
					spriteCloud3.getPosition().x + (cloud3Speed * dt.asSeconds()),
					spriteCloud3.getPosition().y
				);
				//After the cloud crosses the right edge of the screen
				if (spriteCloud3.getPosition().x > 1950) {
					cloud3Active = false;
				}
			}

			//Update the score text
			std::stringstream ss;
			ss << "Score = " << score;
			scoreText.setString(ss.str());
			std::stringstream hs;
			hs << "Highscore = " << highscore;
			highscoreText.setString(hs.str());

			//Update the branch sprites
			for (int i = 0; i < NUM_BRANCHES; i++) {
				float height = i * 150;
				if (branchPositions[i] == side::LEFT) {
					//Move the sprite to the left side
					branches[i].setPosition(610, height);
					//Flip the sprite round the other way
					branches[i].setRotation(180);
				}
				else if (branchPositions[i] == side::RIGHT) {
					//Move the sprite to the right side
					branches[i].setPosition(1330, height);
					//Set sprite rotation to normal
					branches[i].setRotation(0);
				}
				else {
					//Hide the branch
					branches[i].setPosition(3000, height);
				}
			}

			//Handle the flying log
			if (logActive) {
				spriteLog.setPosition(
					spriteLog.getPosition().x + (logSpeedX * dt.asSeconds()),
					spriteLog.getPosition().y + (logSpeedY * dt.asSeconds())
				);

				//After the log crosses the left or right edge of the screen
				if (spriteLog.getPosition().x < -100 || spriteLog.getPosition().y > 2000) {
					//Set up a new log
					logActive = false;
					spriteLog.setPosition(810, 720);
				}
			}

			//Handle the death of the player
			if (branchPositions[5] == playerSide) {
				//Death
				paused = true;
				acceptInput = false;
				//Gravestone
				spriteRIP.setPosition(525, 760);
				//Hide the player
				spritePlayer.setPosition(2000, 660);
				//Change the text of the message
				messageText.setString("You're Dead!!!");
				//Center it on the screen
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(
					textRect.left + textRect.width / 2.0f,
					textRect.top + textRect.height / 2.0f
				);
				messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

				//Death sound
				death.play();
			}

		} //End if(!paused)
		

		/*
		***************************************************
		Draw the screen
		***************************************************
		*/
		//Clear everything from last frame
		window.clear();

		//Draw the game screen here:

		//Background
		window.draw(spriteBackground);

		//Clouds
		window.draw(spriteCloud1);
		window.draw(spriteCloud2);
		window.draw(spriteCloud3);

		//Branches
		for (int i = 0; i < NUM_BRANCHES; i++) {
			window.draw(branches[i]);
		}

		//Tree
		window.draw(spriteTree);

		//Player
		window.draw(spritePlayer);

		//Axe
		window.draw(spriteAxe);

		//Flying log
		window.draw(spriteLog);

		//Gravestone
		window.draw(spriteRIP);

		//Bee
		window.draw(spriteBee);

		//Score
		window.draw(scoreText);

		//Highscore
		window.draw(highscoreText);

		//Timebar
		window.draw(timebar);

		//Message
		if (paused) {
			window.draw(messageText);
		}

		//Show everything we just drew:
		window.display();
	}

	return 0;
}

//Function to update branches
void updateBranches(int seed) {
	//Move all the branches down one place
	for (int j = NUM_BRANCHES - 1; j > 0; j--) {
		branchPositions[j] = branchPositions[j - 1];
	}
	//Spawn a new branch at position 0 (top of the branch)
	srand((int)time(0) + seed);
	int r = (rand() % 5);
	switch (r) {
	case 0:
		branchPositions[0] = side::LEFT;
		break;
	case 1:
		branchPositions[0] = side::RIGHT;
		break;
	default:
		branchPositions[0] = side::NONE;
		break;
	}

}






// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
