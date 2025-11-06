/**
 * BTE5058a Einstieg in  OOP, Mini-project Scoreboard
 * @file  scoreboard.cpp
 * @class scoreboard
 * @brief
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
#include "score_board.h"

Score_board::Score_board(score_memory* score, timer* gameTime, QWidget* parent)
    : QWidget(parent), Score(score), gameTime(gameTime)

{
    setupLayout();
    applyStyle();  // Apply background and text color
    // Fullscreen mode, no title bar

    // Connect signals
    connect(Score, &score_memory::goalsUpdated, this, &Score_board::updateGoals);
    connect(Score, &score_memory::goalsUpdated, this, &Score_board::updateScore);
    connect(gameTime, &timer::timeUpdated, this, &Score_board::updateTime);

    // Display Initialisation
    updateScore();
    updateGoals();

    // In constructor or setupLayout
    QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_F11), this);
    connect(shortcut, &QShortcut::activated, [=]() {
        if (!isFullScreen()) {
            setWindowFlags(Qt::FramelessWindowHint);
            showFullScreen();
        } else {
            setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
            showNormal();
        }
    });


}


void Score_board::setupLayout()
{
    QVBoxLayout* mainlayout = new QVBoxLayout(this);
    mainlayout->setAlignment(Qt::AlignCenter);

    /*   QVBoxLayout* scorelayout = new QVBoxLayout(this);
        scorelayout->setAlignment(Qt::AlignCenter);
*/
    scoreLabel = new QLabel("0 : 0", this);
    scoreLabel->setAlignment(Qt::AlignCenter);


    /**********************************/
    emblemTeam1 = new QLabel(this);
    emblemTeam2 = new QLabel(this);

    emblemTeam1->setFixedSize(500, 500);
    emblemTeam2->setFixedSize(500, 500);
    emblemTeam1->setScaledContents(true);
    emblemTeam2->setScaledContents(true);
    emblemTeam1->setAlignment(Qt::AlignCenter);
    emblemTeam2->setAlignment(Qt::AlignCenter);

    QHBoxLayout* topLayout = new QHBoxLayout;
    topLayout->addSpacing(100);
    topLayout->addWidget(emblemTeam1);
    topLayout->addStretch(10);
    topLayout->addWidget(scoreLabel);
    topLayout->addStretch(10);
    topLayout->addWidget(emblemTeam2);
    topLayout->addSpacing(100);


    /**********************************/
    timeLabel = new QLabel("00:00", this);
    timeLabel->setAlignment(Qt::AlignCenter);

    QHBoxLayout* scorerLayout = new QHBoxLayout;
    scorerListTeam1 = new QListWidget(this);
    scorerListTeam1->setFocusPolicy(Qt::NoFocus);
    scorerListTeam2 = new QListWidget(this);
    scorerListTeam2->setFocusPolicy(Qt::NoFocus);
    scorerLayout->addSpacing(100);
    scorerLayout->addWidget(scorerListTeam1);
    //scorerLayout->addStretch();
    scorerLayout->addSpacing(600);
    scorerLayout->addWidget(scorerListTeam2);
    scorerLayout->addSpacing(100);



    mainlayout->addLayout(topLayout);
    mainlayout->addWidget(timeLabel);
    mainlayout->addLayout(scorerLayout);
    setLayout(mainlayout);
}



void Score_board::applyStyle()
{
    /**
     * @brief Desing paranmeters to set the design of the scoreboard
     */
    this->setStyleSheet("background-color: black;");  // <-- Background color here

    scoreLabel->setStyleSheet("color: white;");
    timeLabel->setStyleSheet("color: white;");
    scorerListTeam1->setStyleSheet("color: white; background-color: black; border: none;");
    scorerListTeam2->setStyleSheet("color: white; background-color: black; border: none;");
}


void Score_board::updateScore()
{
    /**
     * @brief Layout for represent the score
     */
    unsigned home = Score->getHomeScore();
    unsigned away = Score->getAwayScore();
    scoreLabel->setText(QString("%1 : %2").arg(home).arg(away));
}

void Score_board::extracted(QList<Goal> &goals) {
    /**
     * @brief Design of the scorerlist, which also remarks when it is a owngaol
     *
     * @note this part of the code was coded with help from chatgpt.
     */
    for (const Goal &g : goals) {
        QString text;

        // If it's an own goal, show "OG" instead of player number
        if (g.ownGoal) {
            text = QString("OG - %1  %2'").arg(g.player, g.timeStamp);
        } else {
            text = QString("%1 - %2  %3'")
            .arg(g.playerNumber)
                .arg(g.player, g.timeStamp);
        }

        // Create the item
        QListWidgetItem* item = new QListWidgetItem(text);

        // Add to the correct team list
        if (g.team == "Home") {
            item->setTextAlignment(Qt::AlignCenter);  // Right align for Home team
            scorerListTeam1->addItem(item);
        } else if (g.team == "Away") {
            item->setTextAlignment(Qt::AlignCenter);  // Left align for Away team
            scorerListTeam2->addItem(item);
        }
    }
}


void Score_board::updateGoals() {

    /**
     * @brief Updates the numbers of the score when one was added or removed
     *
     * @note this part of the code was coded with help from chatgpt.
     */
    scorerListTeam1->clear();
    scorerListTeam2->clear();

    QList<Goal> goals = Score->getGoals();
    extracted(goals);
}

void Score_board::updateTime(const QString& time)
{
    timeLabel->setText(time);
}




void Score_board::resizeEvent(QResizeEvent *event)
{
    /**
     * @brief Will be  recalled when the size of the window has be changed
     */
    adjustFontSize();
    QWidget::resizeEvent(event);
}

void Score_board::adjustFontSize()
{
    /**
     * @brief Sets the parameters for resize the window, so that all datas can be displayed in
     * a go size.
     *
     * @note this part of the code was coded with help from chatgpt.
     */
    int w = width();
    int h = height();
    int fontSize = std::min(w, h) / 5;  // Adjust scaling as needed

    QFont font;
    font.setPointSize(fontSize);
    font.setBold(true);

    scoreLabel->setFont(font);

    QFont timeFont;
    timeFont.setPointSize(fontSize / 2);
    timeFont.setBold(true);
    timeLabel->setFont(timeFont);

    QFont goalFont;
    goalFont.setPointSize(fontSize / 6);
    scorerListTeam1->setFont(goalFont);
    scorerListTeam2->setFont(goalFont);

    adjustEmblemSize();
}

void Score_board::keyPressEvent(QKeyEvent *event)
{
    /**
     * @brief Lets the score board displaying in full size without white windwow bars.
     */
    if (event->key() == Qt::Key_Escape) {
        if (isFullScreen()) {
            showNormal();  // Exit fullscreen
        }
    }
}


/**********************************/
// Add emblem
void Score_board::updateEmblem(const QString& team, const QString& filePath)
{
    /**
     * @brief Represents the emblem of the teams.
     */
    if (team == "Home"){
        emblemTeam1->setPixmap(QPixmap(filePath));
    } else if (team == "Away"){
        emblemTeam2->setPixmap(QPixmap(filePath));

    }
}

void Score_board::adjustEmblemSize()
{
    /**
     * @brief Resizes the embelem as well.
     */

    int w = width();
    int h = height();

    // Adjust size as a fraction of window size
    int emblemSize = std::min(w, h) / 3;  // Adjust 5 as a scaling factor if needed

    emblemTeam1->setFixedSize(emblemSize, emblemSize);
    emblemTeam2->setFixedSize(emblemSize, emblemSize);

}
/**********************************/



