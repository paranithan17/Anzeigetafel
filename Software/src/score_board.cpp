/**
 * @file score_board.cpp
 * @brief Implementation of the Scoreboard display window.
 *
 * Provides live score and match information display with support for fullscreen,
 * emblems, and dynamic layout scaling. Integrates match state changes and
 * synchronizes with timer and score memory components.
 *
 * @author Paranithan Paramalingam (BFH-Ti)
 * @version 2.1, 2025-12-25
 */
#include "score_board.h"

Score_board::Score_board(score_memory *score, timer *gameTime, QWidget *parent)
    : QWidget(parent), Score(score), gameTime(gameTime)

{
    setupSlidePaths();
    setupLayout();
    applyStyle();

    setFixedSize(512, 320);
    adjustFontSize();

    // showFullScreen();
    setWindowFlags(Qt::FramelessWindowHint);

    slideshowLabel = new QLabel(this);
    slideshowLabel->setAlignment(Qt::AlignCenter);
    slideshowLabel->setStyleSheet("background-color: black;");
    slideshowLabel->setVisible(false);
    slideshowLabel->raise();
    slideshowLabel->setGeometry(this->rect());

    slideshowTimer = new QTimer(this);
    connect(slideshowTimer, &QTimer::timeout, this, &Score_board::showNextSlide);
    connect(Score, &score_memory::goalsUpdated, this, &Score_board::updateGoals);
    connect(Score, &score_memory::goalsUpdated, this, &Score_board::updateScore);
    connect(gameTime, &timer::timeUpdated, this, &Score_board::updateTime);

    updateScore();
    updateGoals();

    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_F11), this);
    connect(shortcut, &QShortcut::activated, [=]()
            {
        if (!isFullScreen()) {
            setWindowFlags(Qt::FramelessWindowHint);
            showFullScreen();
        } else {
            setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
            showNormal();
        } });

    m_state = MatchState::PreGame;
    updateViewForState();
}

void Score_board::setupLayout()
{
    QVBoxLayout *mainlayout = new QVBoxLayout(this);
    mainlayout->setAlignment(Qt::AlignCenter);

    // Score label
    scoreLabel = new QLabel("0 : 0", this);
    scoreLabel->setAlignment(Qt::AlignCenter);

    emblemTeam1 = new QLabel(this);
    emblemTeam2 = new QLabel(this);

    emblemTeam1->setFixedSize(500, 500);
    emblemTeam2->setFixedSize(500, 500);
    emblemTeam1->setScaledContents(true);
    emblemTeam2->setScaledContents(true);
    emblemTeam1->setAlignment(Qt::AlignCenter);
    emblemTeam2->setAlignment(Qt::AlignCenter);

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addSpacing(10);
    topLayout->addWidget(emblemTeam1);
    topLayout->addStretch(10);
    topLayout->addWidget(scoreLabel);
    topLayout->addStretch(10);
    topLayout->addWidget(emblemTeam2);
    topLayout->addSpacing(10);

    timeLabel = new QLabel("00:00", this);
    timeLabel->setAlignment(Qt::AlignCenter);

    QHBoxLayout *scorerLayout = new QHBoxLayout;
    scorerListTeam1 = new QListWidget(this);
    scorerListTeam1->setFocusPolicy(Qt::NoFocus);
    scorerListTeam2 = new QListWidget(this);
    scorerListTeam2->setFocusPolicy(Qt::NoFocus);
    scorerLayout->addSpacing(10);
    scorerLayout->addWidget(scorerListTeam1);
    scorerLayout->addStretch();
    scorerLayout->addSpacing(60);
    scorerLayout->addWidget(scorerListTeam2);
    scorerLayout->addSpacing(10);

    mainlayout->addSpacing(-5);
    mainlayout->addLayout(topLayout);
    mainlayout->addSpacing(-20);
    mainlayout->addWidget(timeLabel);
    mainlayout->addLayout(scorerLayout);
    setLayout(mainlayout);
}

void Score_board::applyStyle()
{
    this->setStyleSheet("background-color: black;");

    scoreLabel->setStyleSheet("color: white;");
    timeLabel->setStyleSheet("color: white;");
    scorerListTeam1->setStyleSheet("color: white; background-color: black; border: none;");
    scorerListTeam2->setStyleSheet("color: white; background-color: black; border: none;");
}

void Score_board::updateScore()
{
    unsigned home = Score->getHomeScore();
    unsigned away = Score->getAwayScore();
    scoreLabel->setText(QString("%1 : %2").arg(home).arg(away));
}

void Score_board::extracted(QList<Goal> &goals)
{
    for (const Goal &g : goals)
    {
        if (g.player.trimmed().isEmpty())
        {
            continue;
        }

        QString text;

        if (g.ownGoal)
        {
            text = QString("OG - %1  %2'").arg(g.player, g.timeStamp);
        }
        else
        {
            text = QString("%1 - %2  %3'")
                       .arg(g.playerNumber)
                       .arg(g.player, g.timeStamp);
        }

        QListWidgetItem *item = new QListWidgetItem(text);

        const QString targetTeam = g.ownGoal
                                       ? (g.team == "Home" ? QStringLiteral("Away") : QStringLiteral("Home"))
                                       : g.team;

        if (targetTeam == "Home")
        {
            item->setTextAlignment(Qt::AlignCenter);
            scorerListTeam1->addItem(item);
        }
        else if (targetTeam == "Away")
        {
            item->setTextAlignment(Qt::AlignCenter);
            scorerListTeam2->addItem(item);
        }
    }
}

void Score_board::updateGoals()
{
    scorerListTeam1->clear();
    scorerListTeam2->clear();

    QList<Goal> goals = Score->getGoals();
    extracted(goals);
}

void Score_board::updateTime(const QString &time)
{
    timeLabel->setText(time);
}

void Score_board::setMatchState(int state)
{
    MatchState newState = m_state;

    switch (state)
    {
    case 0:
        newState = MatchState::PreGame;
        break;
    case 1:
        newState = MatchState::FirstHalf;
        break;
    case 2:
        newState = MatchState::HalfTime;
        break;
    case 3:
        newState = MatchState::SecondHalf;
        break;
    case 4:
        newState = MatchState::PostGame;
        break;
    default:
        return;
    }

    if (newState == m_state)
        return;

    m_state = newState;
    updateViewForState();
}

void Score_board::resizeEvent(QResizeEvent *event)
{
    adjustFontSize();

    if (slideshowLabel)
    {
        slideshowLabel->setGeometry(this->rect());

        QPixmap pix = slideshowLabel->pixmap(Qt::ReturnByValue);
        if (!pix.isNull())
        {
            slideshowLabel->setPixmap(
                pix.scaled(size(),
                           Qt::KeepAspectRatio,
                           Qt::SmoothTransformation));
        }
    }

    QWidget::resizeEvent(event);
}

void Score_board::adjustFontSize()
{
    // Dynamic font sizes for fullscreen mode
    if (isFullScreen())
    {
        int w = width();
        int h = height();
        int fontSize = std::min(w, h) / 5;

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
    // Font sizes for fixed 512x320 LED wall
    else
    {
        QFont scoreFont;
        scoreFont.setPixelSize(110);
        scoreFont.setBold(true);
        scoreLabel->setFont(scoreFont);

        QFont timeFont;
        timeFont.setPixelSize(60);
        timeFont.setBold(true);
        timeLabel->setFont(timeFont);

        QFont goalFont;
        goalFont.setPixelSize(14);
        goalFont.setBold(false);
        scorerListTeam1->setFont(goalFont);
        scorerListTeam2->setFont(goalFont);

        emblemTeam1->setFixedSize(44 * 2, 44 * 2);
        emblemTeam2->setFixedSize(44 * 2, 44 * 2);
    }
}

void Score_board::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        if (isFullScreen())
        {
            showNormal();
        }
    }
}

void Score_board::updateEmblem(const QString &team, const QString &filePath)
{
    if (team == "Home")
    {
        emblemTeam1->setPixmap(QPixmap(filePath));
    }
    else if (team == "Away")
    {
        emblemTeam2->setPixmap(QPixmap(filePath));
    }
}

void Score_board::adjustEmblemSize()
{
    int w = width();
    int h = height();
    int emblemSize = std::min(w, h) / 3;

    emblemTeam1->setFixedSize(emblemSize, emblemSize);
    emblemTeam2->setFixedSize(emblemSize, emblemSize);
}

void Score_board::updateViewForState()
{
    bool gameMode =
        (m_state == MatchState::FirstHalf ||
         m_state == MatchState::SecondHalf);

    if (gameMode)
    {
        stopSlideshow();

        scoreLabel->show();
        timeLabel->show();
        scorerListTeam1->show();
        scorerListTeam2->show();
        emblemTeam1->show();
        emblemTeam2->show();
    }
    else
    {
        scoreLabel->hide();
        timeLabel->hide();
        scorerListTeam1->hide();
        scorerListTeam2->hide();
        emblemTeam1->hide();
        emblemTeam2->hide();

        if (m_state == MatchState::PreGame)
            startSlideshow(preGamePath);
        else if (m_state == MatchState::HalfTime)
            startSlideshow(halfTimePath);
        else if (m_state == MatchState::PostGame)
            startSlideshow(postGamePath);
    }
}

void Score_board::startSlideshow(const QString &folderPath)
{
    stopSlideshow();

    QDir folder(folderPath);
    if (!folder.exists())
    {
        qWarning() << "Slideshow folder does not exist:" << folderPath;
        return;
    }

    slideshowFiles = collectSlides(folderPath);

    if (slideshowFiles.isEmpty())
    {
        qWarning() << "No slides found in slideshow folder:" << folderPath;
        return;
    }

    slideshowIndex = 0;
    slideshowLabel->setVisible(true);

    showNextSlide();
    slideshowTimer->start(4000);
}

void Score_board::stopSlideshow()
{
    slideshowTimer->stop();
    slideshowLabel->setVisible(false);
    slideshowFiles.clear();
}

void Score_board::showNextSlide()
{
    if (slideshowFiles.isEmpty())
        return;

    const QString fullPath = slideshowFiles.at(slideshowIndex);
    slideshowIndex = (slideshowIndex + 1) % slideshowFiles.size();

    QPixmap pix(fullPath);
    if (!pix.isNull())
    {
        slideshowLabel->setPixmap(
            pix.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    else
    {
        qWarning() << "Failed to load slide:" << fullPath;
    }
}

QStringList Score_board::collectSlides(const QString &folderPath)
{
    qDebug() << "collectSlides called with:" << folderPath;

    QDir dir(folderPath);
    if (!dir.exists())
    {
        qWarning() << "Slideshow folder does not exist:" << folderPath;
        return {};
    }

    QStringList slides;
    slides << collectImages(dir);
    slides.removeDuplicates();
    slides.sort(Qt::CaseInsensitive);

    return slides;
}

QStringList Score_board::collectImages(const QDir &dir)
{
    QStringList result;

    QFileInfoList files = dir.entryInfoList(
        {"*.png", "*.jpg", "*.jpeg", "*.bmp"},
        QDir::Files | QDir::Readable,
        QDir::Name);

    for (const QFileInfo &fi : files)
    {
        result << fi.absoluteFilePath();
    }

    return result;
}

void Score_board::setControlWindow(QWidget *window)
{
    controlWindow = window;
}

void Score_board::toggleControlWindow()
{
    if (controlWindow)
    {
        if (controlWindow->isVisible())
        {
            controlWindow->hide();
        }
        else
        {
            controlWindow->show();
            controlWindow->raise();
            controlWindow->activateWindow();
        }
    }
}

void Score_board::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    toggleControlWindow();
}

QString Score_board::findSlideBasePath()
{
    QStringList possiblePaths = {
        QDir::homePath() + "/Anzeigetafel/slides",
        QDir::homePath() + "/Desktop/Anzeigetafel/slides",
        QCoreApplication::applicationDirPath() + "/slides",
        QCoreApplication::applicationDirPath() + "/../slides",
        QCoreApplication::applicationDirPath() + "/../../slides"};

    for (const QString &path : possiblePaths)
    {
        QDir dir(path);

        if (dir.exists("PreGame") &&
            dir.exists("HalfTime") &&
            dir.exists("PostGame"))
        {
            return dir.absolutePath();
        }
    }

    qWarning() << "No valid slide folder found!";
    return QString();
}

void Score_board::setupSlidePaths()
{
    baseSlidePath = findSlideBasePath();

    preGamePath = baseSlidePath + "/PreGame";
    halfTimePath = baseSlidePath + "/HalfTime";
    postGamePath = baseSlidePath + "/PostGame";

    qDebug() << "PreGame path:" << preGamePath;
    qDebug() << "HalfTime path:" << halfTimePath;
    qDebug() << "PostGame path:" << postGamePath;
}