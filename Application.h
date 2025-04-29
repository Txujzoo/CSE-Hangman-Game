#ifndef APPLICATION_H
#define APPLICATION_H


Hello
#include <iostream>



#include "ucm_random.h"
#include <bobcat_ui/all.h>
#include <fstream>

class Application : public bobcat::Application_ {
    bobcat::Window* window;

    bobcat::TextBox* titleTextbox;

    // state variables
    std::string currentScreen;
    int secretNumber;
    int guesses;
    RNG generator;

    // shared screen variables
    bobcat::Button* returnToMainMenuButton;

    // main menu screen variables
    bobcat::Button* playGameButton;
    bobcat::Button* instructionsButton;
    bobcat::Button* statsButton;
    bobcat::Button* quitGameButton;

    // instructions screen variables
    bobcat::TextBox* instructionsTextbox;

    // statistics screen variables
    bobcat::TextBox* statisticsNumberTextbox;
    bobcat::TextBox* statisticsGuessesTextbox;
    bobcat::TextBox* statisticsOutcomesTextbox;

    // play game screen variables
    bobcat::IntInput* guessInput;
    bobcat::Button* guessButton;
    bobcat::Button* giveUpButton;

    void writeDataToFile(int secretNumber, int guesses, std::string outcome) {
        std::fstream file;
        file.open("data/statistics.csv", std::ios::app);
        if (file.is_open()) {
            file << secretNumber << "," << guesses << "," << outcome << std::endl;
            file.close();
        }
    }

    void readStatsFromFile() {
        std::fstream file;
        file.open("data/statistics.csv", std::ios::in);

        if (file.is_open()) {
            std::string line;

            statisticsNumberTextbox->label("Number\n");
            statisticsGuessesTextbox->label("Guesses\n");
            statisticsOutcomesTextbox->label("Outcome\n");

            while (getline(file, line)) {
                int commaCount = 0;

                std::string secretNumberText = "";
                std::string guessesText = "";
                std::string outcomeText = "";
                for (unsigned int i = 0; i < line.length(); i++) {
                    if (line[i] == ',') {
                        commaCount++;
                    }
                    else {
                        if (commaCount == 0) {
                            secretNumberText += line[i];
                        }
                        else if (commaCount == 1) {
                            guessesText += line[i];
                        }
                        else if (commaCount == 2) {
                            outcomeText += line[i];
                        }
                    }
                }

                statisticsNumberTextbox->label(statisticsNumberTextbox->label() + secretNumberText + "\n");
                statisticsGuessesTextbox->label(statisticsGuessesTextbox->label() + guessesText + "\n");
                statisticsOutcomesTextbox->label(statisticsOutcomesTextbox->label() + outcomeText + "\n");
            }

            file.close();
        }
    }

    // hide all visual components
    void hideAll() {
        playGameButton->hide();
        instructionsButton->hide();
        statsButton->hide();
        statisticsNumberTextbox->hide();
        statisticsGuessesTextbox->hide();
        statisticsOutcomesTextbox->hide();
        quitGameButton->hide();
        instructionsTextbox->hide();
        returnToMainMenuButton->hide();
        guessInput->hide();
        guessButton->hide();
        giveUpButton->hide();
    }

    // function to handle rendering of the current screen
    void renderCurrentScreen() {
        // hide all visual components
        hideAll();

        if (currentScreen == "Main Menu") {
            titleTextbox->label("Number Guessing Game");
            playGameButton->show();
            instructionsButton->show();
            statsButton->show();
            quitGameButton->show();
        }
        else if (currentScreen == "Instructions") {
            titleTextbox->label("How to Play");
            instructionsTextbox->show();
            returnToMainMenuButton->show();
        }
        else if (currentScreen == "Statistics") {
            readStatsFromFile();
            titleTextbox->label("Statistics");
            statisticsNumberTextbox->show();
            statisticsGuessesTextbox->show();
            statisticsOutcomesTextbox->show();
            returnToMainMenuButton->show();
        }
        else if (currentScreen == "Game") {
            secretNumber = generator.get(1, 100);
            guesses = 0;
            titleTextbox->label("Guess the Number");
            guessInput->show();
            guessButton->show();
            giveUpButton->show();
        }
    }

    void handleNavigationClick(bobcat::Widget* sender) {
        if (sender == playGameButton) {
            currentScreen = "Game";
            renderCurrentScreen();
        }
        else if (sender == instructionsButton) {
            currentScreen = "Instructions";
            renderCurrentScreen();
        }
        else if (sender == statsButton) {
            currentScreen = "Statistics";
            renderCurrentScreen();
        }
        else if (sender == returnToMainMenuButton) {
            currentScreen = "Main Menu";
            renderCurrentScreen();
        }
        else if (sender == quitGameButton) {
            // this will stop the application
            window->hide();
        }
    }

    void handleGameClick(bobcat::Widget* sender) {
        if (sender == guessButton) {
            if (!guessInput->empty()) {
                guesses++;
                int guess = guessInput->value();

                if (guess == secretNumber) {
                    bobcat::showMessage("That is correct!");
                    writeDataToFile(secretNumber, guesses, "Win");
                    currentScreen = "Main Menu";
                    renderCurrentScreen();
                }
                else if (guess < secretNumber) {
                    bobcat::showMessage("Go higher...");
                }
                else {
                    bobcat::showMessage("Go lower...");
                }
            }
        }
        else if (sender == giveUpButton) {
            bobcat::showMessage("Thank you for playing. The secret number was " + std::to_string(secretNumber) + ".");
            writeDataToFile(secretNumber, guesses, "Loss");
            currentScreen = "Main Menu";
            renderCurrentScreen();
        }
    }

public:
    Application(){
        window = new bobcat::Window(25, 75, 400, 400, "Number Guessing Game");

        // initialize state variables
        currentScreen = "Main Menu";

        titleTextbox = new bobcat::TextBox(25, 25, 350, 25, "Number Guessing Game");
        titleTextbox->align(FL_ALIGN_CENTER);
        titleTextbox->labelsize(20);

        // main menu screen variables
        playGameButton = new bobcat::Button(25, 100, 350, 50, "Play Game");
        instructionsButton = new bobcat::Button(25, 175, 350, 50, "View Instructions");
        statsButton = new bobcat::Button(25, 250,350, 50, "View Statistics");
        quitGameButton = new bobcat::Button(25, 325, 350, 50, "Quit Game");
        ON_CLICK(playGameButton, Application::handleNavigationClick);
        ON_CLICK(instructionsButton, Application::handleNavigationClick);
        ON_CLICK(statsButton, Application::handleNavigationClick);
        ON_CLICK(quitGameButton, Application::handleNavigationClick);

        // instructions screen variables
        instructionsTextbox = new bobcat::TextBox(25, 100, 350, 50, "Input a guess (number between 1 and 100).\nThe application will give you feedback.");

        // statistics screen variables
        statisticsNumberTextbox = new bobcat::TextBox(25, 100, 100, 250, "Number");
        statisticsGuessesTextbox = new bobcat::TextBox(125, 100, 100, 250, "Guesses");
        statisticsOutcomesTextbox = new bobcat::TextBox(225, 100, 100, 250, "Outcome");
        statisticsNumberTextbox->align(FL_ALIGN_TOP_LEFT);
        statisticsGuessesTextbox->align(FL_ALIGN_TOP_LEFT);
        statisticsOutcomesTextbox->align(FL_ALIGN_TOP_LEFT);

        // shared screen variables
        returnToMainMenuButton = new bobcat::Button(25, 350, 350, 25, "Return to Main Menu");
        ON_CLICK(returnToMainMenuButton, Application::handleNavigationClick);

        // game screen variables
        guessInput = new bobcat::IntInput(25, 100, 350, 25, "Input Guess:");
        guessButton = new bobcat::Button(25, 150, 350, 25, "Submit Guess");
        giveUpButton = new bobcat::Button(25, 200, 350, 25, "Give Up");
        ON_CLICK(guessButton, Application::handleGameClick);
        ON_CLICK(giveUpButton, Application::handleGameClick);

        renderCurrentScreen();

        window->show();
    }

    friend struct AppTest;
};

#endif
