/**
 * @file score_memory.h
 * @brief Score and goal event management.
 *
 * Manages all scoring information including goal events, player information, and score
 * calculation for both teams. Stores complete match score history and provides methods
 * to add/remove goals and retrieve current scores. Emits signals when goals are updated.
 *
 * Features:
 * - Goal event storage with player details and timestamps
 * - Home and away team score tracking
 * - Own goal detection and handling
 * - Score reset and goal removal with undo capability
 * - Signal-based notifications for score changes
 * - Complete match history for logging and review
 *
 * Data Structure:
 * Maintains a QList of GoalEvent objects, each containing player number, name, team
 * designation, timestamp, and own goal flag.
 *
 * @author Paranithan Paramalingam (BFH-Ti)
 * @version 3.0, 12. June 2026
 */
#ifndef SCORE_MEMORY_H
#define SCORE_MEMORY_H

#include <QObject>
#include <QString>
#include <QList>

struct Goal
{
    /** @brief Jersey number of the player associated with the goal event. */
    int playerNumber;
    /** @brief Player display name captured at event time. */
    QString player;
    /** @brief Formatted match timestamp for the goal (e.g., MM:SS). */
    QString timeStamp;
    /** @brief Team identifier for the event (e.g., "Home" or "Away"). */
    QString team;
    /** @brief True if the event is an own goal. */
    bool ownGoal = false;
};

class score_memory : public QObject
{
    Q_OBJECT

private:
    /** @brief Ordered list of all recorded goal events in the match. */
    QList<Goal> goals;

public:
    /**
     * @brief Constructs score memory storage object.
     *
     * @param parent Optional Qt parent object (default nullptr)
     * @return void
     */
    score_memory(QObject *parent = nullptr);

    /**
     * @brief Returns all recorded goal events.
     *
     * @return Copy of goal-event list in insertion order
     */
    QList<Goal> getGoals() const;

    /**
     * @brief Calculates current home team score.
     *
     * Own goals from away team are counted for home team.
     *
     * @return Current home score
     */
    int getHomeScore() const;

    /**
     * @brief Calculates current away team score.
     *
     * Own goals from home team are counted for away team.
     *
     * @return Current away score
     */
    int getAwayScore() const;

    /**
     * @brief Adds a goal event to match history.
     *
     * Appends the goal, updates score views through signal emission, and
     * preserves complete event metadata for logs.
     *
     * @param number Scorer jersey number
     * @param player Scorer display name
     * @param timeStamp Formatted match timestamp when goal occurred
     * @param team Team identifier associated with this event
     * @param ownGoal True if the event is an own goal
     * @return void
     */
    void addGoal(int number, QString player, const QString &timeStamp, const QString &team, bool ownGoal);

    /**
     * @brief Removes the most recently added goal event.
     *
     * Has no effect if history is empty.
     *
     * @return void
     */
    void removeLastGoal();

    /**
     * @brief Clears all goal events and resets match score state.
     *
     * @return void
     */
    void resetGame();

signals:
    /**
     * @brief Emitted whenever goal history changes.
     *
     * Triggered after add, remove, or reset operations.
     *
     * @return void
     */
    void goalsUpdated();
};

#endif // SCORE_MEMORY_H
