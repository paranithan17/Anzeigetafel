/**
 * @file main.cpp
 * @brief Application entry point for the Football Scoreboard.
 *
 * Initializes and launches a modular graphical scoreboard system for managing
 * football match events. Creates two synchronized main windows: Control (operator)
 * and Scoreboard (audience display), linked via Qt signals/slots.
 *
 * Features:
 * - Real-time goal tracking with timestamps and player attribution.
 * - Two-half timer management (00:00–45:00, 45:00–90:00).
 * - Own goal and undo support.
 * - CSV import for player lists (UTF-8, supports umlauts).
 * - Team emblems and fullscreen toggle.
 * - Match state machine with window toggling.
 *
 * @author Paranithan Paramalingam (BFH-Ti)
 * @version 2.1, 2025-12-25
 *
 * @note Currently designed for 2×45 minute football matches only.
 */

#include <QApplication>
#include "controll_window.h"
#include "score_board.h"
#include "timer.h"
#include "score_memory.h"
#include "match_controller.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  score_memory *ScoreMem = new score_memory();
  timer *gameTime = new timer();
  match_controller *controller = new match_controller(ScoreMem, gameTime);

  Score_board *scoreboard = new Score_board(ScoreMem, gameTime);
  scoreboard->show();

  controll_window *window = new controll_window(controller);
  window->show();

  // Set bidirectional window references for toggling
  scoreboard->setControlWindow(window);
  window->setScoreboard(scoreboard);

  // Add emblem changed signal connection
  QObject::connect(window, &controll_window::emblemChanged, scoreboard, &Score_board::updateEmblem);

  // Match controller is the source for interaction state changes
  QObject::connect(controller, &match_controller::matchStateChanged,
                   scoreboard, &Score_board::setMatchState);

  return app.exec();
}