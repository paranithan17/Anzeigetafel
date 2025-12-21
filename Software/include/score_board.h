/**
 * BTE5058a Einstieg in  OOP, Mini-project Scoreboard
 * @file  scoreboard.cpp
 * @class scoreboard
 * @caption
 * This code opens the second window which shows the score, time
 * and player who scored the goal to audience.
 * If there is enough time before submission, then is also possible
 * the show embelem of the Teams aswell.
 *
 * @author Paranithan Paramalingam. BFH-Ti
 * @version V1.0, 29.05.2025
 *
 * @note This code has been created with help of chatgpt
 */
#ifndef SCORE_BOARD_H
#define SCORE_BOARD_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QListWidget>
#include <QKeyEvent>
#include <QWindowStateChangeEvent>
#include <QShortcut>
#include <QTimer>
#include <QDir>
#include <QPixmap>
#include <QProcess>
#include <QFileInfo>



#include "score_memory.h"
#include "timer.h"

class Score_board : public QWidget
{
    Q_OBJECT

public:
    // Keep this in sync with controll_window::MatchState
    enum class MatchState {
        PreGame = 0,
        FirstHalf,
        HalfTime,
        SecondHalf,
        PostGame
    };


private:
    MatchState m_state = MatchState::PreGame;
    score_memory* Score;
    timer* gameTime;

    QLabel* scoreLabel;
    QLabel* timeLabel;
    QListWidget* scorerListTeam1;
    QListWidget* scorerListTeam2;

    /**********************************/
    /**
     * Extra features:
     * 1) Adding emblems of both teams
     */
    // Elements to add emblems
    QLabel* emblemTeam1;
    QLabel* emblemTeam2;

    void adjustEmblemSize();
    /**********************************/

    /*
     * --- Slideshow system ---
     */
    QTimer *slideshowTimer = nullptr;
    QLabel *slideshowLabel = nullptr;

    QStringList slideshowFiles;
    int slideshowIndex = 0;


    bool pptConversionEnabled = false;
    bool isLinuxPlatform() const;
    QStringList convertPowerPoints(const QDir &dir);
    QString cacheDirForDeck(const QDir &baseDir, const QFileInfo &pptFile) const;
    bool cacheIsUpToDate(const QFileInfo &pptFile, const QDir &cacheDir) const;
    QStringList collectSlides(const QString &folderPath);
    QStringList collectImages(const QDir &dir);





    QString preGamePath  = "C:/Users/paran/Desktop/Anzeigetafel/slides/PreGame";
    QString halfTimePath = "C:/Users/paran/Desktop/Anzeigetafel/slides/HalfTime";
    QString postGamePath = "C:/Users/paran/Desktop/Anzeigetafel/slides/PostGame";


    // helpers
    void startSlideshow(const QString &folderPath);
    void stopSlideshow();
    void showNextSlide();
    /*********************************/




    void setupLayout();
    void applyStyle();
    void adjustFontSize();

private slots:
    void updateScore();
    void extracted(QList<Goal> &goals);
    void updateGoals();
    void updateTime(const QString& time);




protected:
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
   // void changeEvent(QEvent* event) override;


public:
    Score_board(score_memory* scoreMemory, timer* gameTime, QWidget *parent = nullptr);

/**********************************/
public slots:
    // Add embelem
    void updateEmblem(const QString& team, const QString& filePath);
/**********************************/


    // New: react to state machine changes
    void setMatchState(int state);
    /**********************************/
private:
    // Helper to adjust the visual mode based on m_state
    void updateViewForState();
};
#endif // SCORE_BOARD_H
