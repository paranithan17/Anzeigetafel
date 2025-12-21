/**
 * BTE5058a Einstieg in  OOP, Mini-project Scoreboard
 * @file  main.cpp
 * @caption
 * This is main code which runs the score board.
 *
 * This program initializes and launches a modular graphical scoreboard system designed
 * for managing and displaying football match events using the Qt framework.
 * It creates two main windows:
 *
 *   -- Control Window: Allows the match operator to manage team rosters, log goals,
 *      start/reset the game timer, and correct events (e.g., undo last goal).
 *
 *   -- Scoreboard Window: Public display showing live score, match time,
 *      and goal events (player, time, team), with automatic updates.
 *
 * Special Features:
 *
 *   -- Real-time goal tracking with timestamp and player attribution.
 *
 *   -- Timer management supporting first and second halves (00:00–45:00, 45:00–90:00).
 *
 *   -- Support for own goals and undoing the last scoring event.
 *
 *   -- CSV import for player lists and optional emblem uploads for team branding.
 *
 *   -- Fullscreen display toggle for the scoreboard view.
 *
 *
 * This file is the entry point of the application, linking core components
 * (score_memory, timer, control_window, score_board) via Qt’s signal-slot mechanism
 * to enable live updates and interactivity.
 *
 *
 * @note At the moment this program allows only to be used in a football game for 2x 45 minutes.
 * There is no option to change the time and over time issues of the game.
 *
 *
 * @author Paranithan Paramalingam. BFH-Ti
 * @version V1.0, 29.05.2025
 * @version V2.0, 06.11.2025 added the resourcefile to show the execute file with a icon
 *
 *
 */

#include <QApplication>
#include "controll_window.h"

#include "score_board.h"
#include "timer.h"
#include "score_memory.h"

int main(int argc, char *argv[])
{

  QApplication app(argc, argv);
  app.setWindowIcon(QIcon(":/icons/fcs_256.png"));


  score_memory *ScoreMem = new score_memory();
  timer *gameTime = new timer();

  Score_board *scoreboard = new Score_board(ScoreMem, gameTime);
  scoreboard->show();

  controll_window *window = new controll_window();
  window->setScoreMemory(ScoreMem);
  window->setTimer(gameTime);
  window->show();

  /**********************************/
  // Add emblem
  QObject::connect(window, &controll_window::emblemChanged, scoreboard, &Score_board::updateEmblem);
  /**********************************/
  // When operator confirms a new match state, notify the scoreboard
  QObject::connect(window, &controll_window::matchStateChanged,
                   scoreboard, &Score_board::setMatchState);


  return app.exec();
}
