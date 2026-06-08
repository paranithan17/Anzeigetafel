/**
 * @file main.cpp
 * @brief Football Scoreboard — application entry point.
 *
 * Initializes and starts the Control (operator) and Scoreboard (audience)
 * windows, the match timer, score memory and WebSocket control interface.
 *
 * Features: real-time goal tracking, two-half timer, own-goal/undo,
 * CSV player import (UTF-8), team emblems, fullscreen toggle, and match state.
 *
 * Author: Paranithan Paramalingam (BFH-Ti)
 * Version: 3.0
 * Date: 2026-06-07
 */

#include <QApplication>
#include "controll_window.h"
#include "score_board.h"
#include "timer.h"
#include "score_memory.h"
#include "match_controller.h"
#include "websocket.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  score_memory *ScoreMem = new score_memory();
  timer *gameTime = new timer();
  match_controller *controller = new match_controller(ScoreMem, gameTime);

  // Set up web server for browser control
  websocket *websocket_server = new websocket(controller);

  // Start WebSocket server on port 8080
  websocket_server->start(8080);

  // Show the control window
  controll_window *window = new controll_window(controller);
  window->show();

  // Show the scoreboard
  Score_board *scoreboard = new Score_board(ScoreMem, gameTime);
  scoreboard->show();

  // Set bidirectional window references for toggling
  scoreboard->setControlWindow(window);
  window->setScoreboard(scoreboard);

  // Add emblem changed signal connection
  QObject::connect(controller, &match_controller::emblemChanged, scoreboard, &Score_board::updateEmblem);

  // Match controller is the source for interaction state changes
  QObject::connect(controller, &match_controller::matchStateChanged,
                   scoreboard, &Score_board::setMatchState);

  return app.exec();
}