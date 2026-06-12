/**
 * @file controll_window.h
 * @brief GUI Control window for match operator (View component).
 *
 * Pure presentation layer that provides the operator control interface for managing
 * matches. Displays team rosters, score, timer, and match state. All user interactions
 * are collected via dialogs and delegated to match_controller for processing. This component
 * receives updates from match_controller through Qt signals and updates the display accordingly.
 *
 * Features:
 * - Team roster UI with add/remove player dialogs
 * - Goal logging dialogs with player selection and own-goal detection
 * - Timer control buttons with state validation feedback
 * - Match state selection (PreGame → FirstHalf → HalfTime → SecondHalf → PostGame)
 * - Team emblem upload and logo management
 * - CSV import dialogs for batch player roster management
 * - Scoreboard toggle via double-click feature
 * - Real-time score and time display updates from controller
 *
 * Design Pattern:
 * Implements the View component of the Model-View-Controller pattern. All business logic is delegated to
 * match_controller, enabling code reuse for web interfaces and other UI clients.
 * Uses Qt signals/slots for communication with model.
 *
 * @author Paranithan Paramalingam (BFH-Ti)
 * @version 3.0, 12. June 2026 (Refactored for Model-View-Controller pattern)
 */
#ifndef CONTROLL_WINDOW_H
#define CONTROLL_WINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QRadioButton>
#include <QGroupBox>
#include <QMouseEvent>

#include "match_controller.h"

class controll_window : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the control window and wires it to the controller.
     *
     * Creates all UI elements for operator interaction and connects view events
     * to controller actions.
     *
     * @param controller Pointer to the match controller (must not be nullptr)
     * @param parent Optional parent widget (default nullptr)
     * @return void
     */
    controll_window(match_controller *controller, QWidget *parent = nullptr);

    /**
     * @brief Sets the scoreboard widget used for visibility toggling.
     *
     * Stores a reference to the scoreboard window so it can be shown or hidden
     * from this control window.
     *
     * @param board Pointer to the scoreboard widget
     * @return void
     */
    void setScoreboard(QWidget *board);

    /**
     * @brief Toggles scoreboard visibility.
     *
     * Shows the scoreboard if hidden and hides it if currently visible.
     *
     * @return void
     */
    void toggleScoreboard();

signals:
    /**
     * @brief Emitted when a team emblem image has changed.
     *
     * Notifies connected views (for example the scoreboard) to reload and
     * display the updated emblem.
     *
     * @param team Team identifier (e.g., "Home" or "Away")
     * @param filepath Absolute path to the emblem image file
     * @return void
     */
    void emblemChanged(const QString &team, const QString &filepath);

protected:
    /**
     * @brief Handles mouse double-click events on the control window.
     *
     * Used as a shortcut interaction to toggle scoreboard visibility.
     *
     * @param event Mouse double-click event data
     * @return void
     */
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private slots:
    /**
     * @brief Opens dialog flow to add a player to team 1.
     * @return void
     */
    void addPlayerTeam1();

    /**
     * @brief Opens dialog flow to add a player to team 2.
     * @return void
     */
    void addPlayerTeam2();

    /**
     * @brief Removes the selected player from team 1.
     *
     * Triggered from team list interaction with the selected list item.
     *
     * @param item Pointer to the clicked/selected team 1 list item
     * @return void
     */
    void removePlayerTeam1(QListWidgetItem *item);

    /**
     * @brief Removes the selected player from team 2.
     *
     * Triggered from team list interaction with the selected list item.
     *
     * @param item Pointer to the clicked/selected team 2 list item
     * @return void
     */
    void removePlayerTeam2(QListWidgetItem *item);

    /**
     * @brief Opens goal-entry dialog for team 1.
     *
     * Collects scorer and own-goal information and delegates processing to
     * the controller.
     *
     * @return void
     */
    void addGoalTeam1();

    /**
     * @brief Opens goal-entry dialog for team 2.
     *
     * Collects scorer and own-goal information and delegates processing to
     * the controller.
     *
     * @return void
     */
    void addGoalTeam2();

    /**
     * @brief Starts or advances timer state through the controller.
     *
     * Applies controller validation rules before starting match time.
     *
     * @return void
     */
    void startTimer();

    /**
     * @brief Logs relevant match events/state for operator tracking.
     * @return void
     */
    void log();

    /**
     * @brief Opens file chooser and loads emblem for team 1.
     *
     * Emits emblemChanged after a valid selection.
     *
     * @return void
     */
    void loadEmblemTeam1();

    /**
     * @brief Opens file chooser and loads emblem for team 2.
     *
     * Emits emblemChanged after a valid selection.
     *
     * @return void
     */
    void loadEmblemTeam2();

    /**
     * @brief Imports team 1 roster from CSV.
     *
     * Delegates parsing and insertion logic to controller/model components.
     *
     * @return void
     */
    void importTeam1();

    /**
     * @brief Imports team 2 roster from CSV.
     *
     * Delegates parsing and insertion logic to controller/model components.
     *
     * @return void
     */
    void importTeam2();

    /**
     * @brief Applies currently selected match state radio option.
     *
     * Converts UI selection into controller state transition request.
     *
     * @return void
     */
    void applyStateSelection();

    /**
     * @brief Refreshes score label from current controller/model state.
     * @return void
     */
    void updateScoreDisplay();

    /**
     * @brief Updates the time label with formatted elapsed time.
     *
     * @param elapsedTime Formatted time string (e.g., MM:SS)
     * @return void
     */
    void updateTimeDisplay(const QString &elapsedTime);

    /**
     * @brief Rebuilds team 1 player list in the UI.
     * @return void
     */
    void updateTeamList1();

    /**
     * @brief Rebuilds team 2 player list in the UI.
     * @return void
     */
    void updateTeamList2();

    /**
     * @brief Syncs radio-state selection when controller state changes.
     *
     * @param state Integer representation of match_controller::MatchState
     * @return void
     */
    void controllerStateChanged(int state);

private:
    /**
     * @brief Controller reference used for all business logic operations.
     */
    match_controller *m_controller = nullptr;

    /**
     * @brief Optional scoreboard window reference for toggling visibility.
     */
    QWidget *scoreboard = nullptr;

    /** @brief Button to add a player to team 1. */
    QPushButton *btnAddPlayerTeam1;
    /** @brief Button to add a player to team 2. */
    QPushButton *btnAddPlayerTeam2;
    /** @brief Button to register a goal for team 1. */
    QPushButton *btnGoalTeam1;
    /** @brief Button to register a goal for team 2. */
    QPushButton *btnGoalTeam2;
    /** @brief Button to start/control timer progression. */
    QPushButton *btnStartTimer;
    /** @brief Button to trigger event logging action. */
    QPushButton *btnLog;
    /** @brief Button to apply selected match state. */
    QPushButton *btnApplyState;
    /** @brief Button to load emblem for team 1. */
    QPushButton *btnAddEmblemTeam1;
    /** @brief Button to load emblem for team 2. */
    QPushButton *btnAddEmblemTeam2;
    /** @brief Button to import team 1 roster from CSV. */
    QPushButton *btnImportTeam1;
    /** @brief Button to import team 2 roster from CSV. */
    QPushButton *btnImportTeam2;

    /** @brief List widget displaying team 1 roster. */
    QListWidget *listTeam1;
    /** @brief List widget displaying team 2 roster. */
    QListWidget *listTeam2;

    /** @brief Radio option for PreGame state. */
    QRadioButton *radioPreGame;
    /** @brief Radio option for FirstHalf state. */
    QRadioButton *radioFirstHalf;
    /** @brief Radio option for HalfTime state. */
    QRadioButton *radioHalfTime;
    /** @brief Radio option for SecondHalf state. */
    QRadioButton *radioSecondHalf;
    /** @brief Radio option for PostGame state. */
    QRadioButton *radioPostGame;

    /** @brief Label showing current score. */
    QLabel *score;
    /** @brief Label showing current elapsed match time. */
    QLabel *time;

    /**
     * @brief Returns match state selected by radio buttons.
     *
     * Reads current UI radio selection and maps it to controller enum value.
     *
     * @return Selected match_controller::MatchState
     */
    match_controller::MatchState selectedState() const;

    /**
     * @brief Updates radio button selection from a given match state.
     *
     * Used to keep UI state synchronized with controller state.
     *
     * @param state Match state to reflect in radio buttons
     * @return void
     */
    void setStateSelection(match_controller::MatchState state);
};

#endif // CONTROLL_WINDOW_H
