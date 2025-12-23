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
 * \version V1.1, 07.11.2025 - Default fullscreen by launching the program
 * @note This code has been created with help of chatgpt
 *
 */
#include "score_board.h"

Score_board::Score_board(score_memory* score, timer* gameTime, QWidget* parent)
    : QWidget(parent), Score(score), gameTime(gameTime)

{  
    setupLayout();
    applyStyle();  // Apply background and text color

    adjustFontSize();    // Force native LED resolution for 2x2 P2.5 Panels
    setFixedSize(256, 128);   // Force native LED resolution for 2x2 P2.5 Panels

    //showFullScreen();     // Fullscreen when there is no issue with the resolution
    setWindowFlags(Qt::FramelessWindowHint);     // Disable window scaling artifacts


    /**
 * -- Slide Show --
 */

    slideshowLabel = new QLabel(this);
    slideshowLabel->setAlignment(Qt::AlignCenter);
    slideshowLabel->setStyleSheet("background-color: black;");
    slideshowLabel->setVisible(false);      // hidden during FirstHalf/SecondHalf
    slideshowLabel->raise();                // always on top when visible

    slideshowLabel->setGeometry(this->rect()); // NEW: make it cover the whole window initially

    slideshowTimer = new QTimer(this);
    connect(slideshowTimer, &QTimer::timeout,
            this, &Score_board::showNextSlide);
    /**************************************/


    // Connect signals
    connect(Score, &score_memory::goalsUpdated, this, &Score_board::updateGoals);
    connect(Score, &score_memory::goalsUpdated, this, &Score_board::updateScore);
    connect(gameTime, &timer::timeUpdated, this, &Score_board::updateTime);

    // Display Initialisation
    updateScore();
    updateGoals();

    // As backup to exit and re-enter the fullscreen mode
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

    m_state = MatchState::PreGame;  // default state on startup
    updateViewForState();           // ensure initial view matches state

    pptConversionEnabled = true; // Conversion on PPT from PPTX (Windows -> Linux)
    qDebug() << "PPT conversion enabled:" << pptConversionEnabled;



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


void Score_board::setMatchState(int state)
{
    MatchState newState = m_state;

    switch (state) {
    case 0: newState = MatchState::PreGame;    break;
    case 1: newState = MatchState::FirstHalf;  break;
    case 2: newState = MatchState::HalfTime;   break;
    case 3: newState = MatchState::SecondHalf; break;
    case 4: newState = MatchState::PostGame;   break;
    default:
        // invalid value – ignore
        return;
    }

    if (newState == m_state)
        return; // nothing to do

    m_state = newState;
    updateViewForState();
}


void Score_board::resizeEvent(QResizeEvent *event)
{
    /**
     * @brief Will be recalled when the size of the window has be changed
     */
    //adjustFontSize(); // commented out for the 2x2 LED Wall

    // Make slideshow label always cover the full window
    if (slideshowLabel) {
        slideshowLabel->setGeometry(this->rect());

        // If a slide is currently visible, rescale it to the new size
        QPixmap pix = slideshowLabel->pixmap(Qt::ReturnByValue);
        if (!pix.isNull()) {
            slideshowLabel->setPixmap(
                pix.scaled(size(),
                           Qt::KeepAspectRatio,
                           Qt::SmoothTransformation)
                );
        }
    }

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
    /*
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
    */


    // Fixed sizes for 256x128 LED wall (tune later if needed)
    QFont scoreFont;
    scoreFont.setPixelSize(72);
    scoreFont.setBold(true);
    scoreLabel->setFont(scoreFont);

    QFont timeFont;
    timeFont.setPixelSize(28);
    timeFont.setBold(true);
    timeLabel->setFont(timeFont);

    QFont goalFont;
    goalFont.setPixelSize(16);
    goalFont.setBold(true);
    scorerListTeam1->setFont(goalFont);
    scorerListTeam2->setFont(goalFont);

    // If you keep emblems, they must be small on 256x128
    emblemTeam1->setFixedSize(32, 32);
    emblemTeam2->setFixedSize(32, 32);


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


void Score_board::updateViewForState()
{
    bool gameMode =
        (m_state == MatchState::FirstHalf ||
         m_state == MatchState::SecondHalf);

    if (gameMode) {
        // Stop slideshow
        stopSlideshow();

        // Show match info
        scoreLabel->show();
        timeLabel->show();
        scorerListTeam1->show();
        scorerListTeam2->show();
        emblemTeam1->show();
        emblemTeam2->show();

    } else {
        // Hide scoreboard UI → slideshow takes over
        scoreLabel->hide();
        timeLabel->hide();
        scorerListTeam1->hide();
        scorerListTeam2->hide();
        emblemTeam1->hide();
        emblemTeam2->hide();

        // Start slideshow for selected state
        if (m_state == MatchState::PreGame)
            startSlideshow(preGamePath);
        else if (m_state == MatchState::HalfTime)
            startSlideshow(halfTimePath);
        else if (m_state == MatchState::PostGame)
            startSlideshow(postGamePath);
    }
}





/**********************************/
void Score_board::startSlideshow(const QString &folderPath)
{
    stopSlideshow();   // stop any running slideshow

    QDir folder(folderPath);
    if (!folder.exists()) {
        qWarning() << "Slideshow folder does not exist:" << folderPath;
        return;
    }

    // Load files (images + PPT/PPTX rendered PNGs)
    slideshowFiles = collectSlides(folderPath);

    if (slideshowFiles.isEmpty()) {
        qWarning() << "No slides found in slideshow folder:" << folderPath;
        return;
    }

    slideshowIndex = 0;
    slideshowLabel->setVisible(true);

    // Show first slide immediately
    showNextSlide();

    // Cycle every 4 seconds (adjustable)
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
    if (!pix.isNull()) {
        slideshowLabel->setPixmap(
            pix.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)
            );
    } else {
        qWarning() << "Failed to load slide:" << fullPath;
    }
}



QStringList Score_board::convertPowerPoints(const QDir &dir)
{
    qDebug() << "convertPowerPoints() called for:" << dir.absolutePath();
    qDebug() << "pptConversionEnabled =" << pptConversionEnabled;

    if (!pptConversionEnabled) {
        return {};
    }

    QStringList result;

    QFileInfoList ppts = dir.entryInfoList(
        {"*.ppt", "*.pptx"},
        QDir::Files | QDir::Readable,
        QDir::Name);

    if (ppts.isEmpty()) {
        return {};
    }

    for (const QFileInfo &pptFile : ppts) {
        const QString pptPath = pptFile.absoluteFilePath();

        // Cache directory per deck
        const QString cachePath = cacheDirForDeck(dir, pptFile);
        QDir cacheDir(cachePath);

        // Create cache dir if missing
        if (!cacheDir.exists()) {
            dir.mkpath(".ppt_cache/" + pptFile.completeBaseName());
        }

        // Only convert if cache is missing/outdated
        if (!cacheIsUpToDate(pptFile, cacheDir)) {
            qDebug() << "Converting PPTX (cache miss/outdated):" << pptPath;


            qDebug() << "Running LibreOffice for:" << pptPath;
            qDebug() << "Output dir:" << cacheDir.absolutePath();

            QProcess proc;
            proc.setProgram("libreoffice");
            proc.setArguments({
                "--headless",
                "--convert-to", "png",
                "--outdir", cacheDir.absolutePath(),
                pptPath
            });

            proc.start();

            if (!proc.waitForFinished(600000)) {
                qWarning() << "LibreOffice conversion timed out for:" << pptPath;
                proc.kill();
                proc.waitForFinished();
                continue;
            }

            if (proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0) {
                qWarning() << "LibreOffice conversion failed for:" << pptPath
                           << "exitCode:" << proc.exitCode()
                           << "stderr:" << proc.readAllStandardError()
                           << "stdout:" << proc.readAllStandardOutput();
                continue;
            }
        } else {
            qDebug() << "Using cached PPTX render:" << pptPath;
        }

        // Collect PNGs from cache folder
        QFileInfoList pngs = cacheDir.entryInfoList(
            {"*.png"},
            QDir::Files | QDir::Readable,
            QDir::Name);

        for (const QFileInfo &png : pngs) {
            result << png.absoluteFilePath();
        }
    }

    result.removeDuplicates();
    result.sort();
    return result;
}



QString Score_board::cacheDirForDeck(const QDir &baseDir, const QFileInfo &pptFile) const
{
    // <slidesFolder>/.ppt_cache/<deckBaseName>/
    return baseDir.absoluteFilePath(".ppt_cache/" + pptFile.completeBaseName());
}

bool Score_board::cacheIsUpToDate(const QFileInfo &pptFile, const QDir &cacheDir) const
{
    if (!cacheDir.exists())
        return false;

    QFileInfoList pngs = cacheDir.entryInfoList(
        {"*.png"},
        QDir::Files | QDir::Readable,
        QDir::Name);

    if (pngs.isEmpty())
        return false;

    // If any PNG is older than the PPT file, reconvert
    const QDateTime pptTime = pptFile.lastModified();
    for (const QFileInfo &png : pngs) {
        if (png.lastModified() < pptTime)
            return false;
    }
    return true;
}

QStringList Score_board::collectSlides(const QString &folderPath)
{
    qDebug() << "collectSlides called with:" << folderPath;

    QDir dir(folderPath);
    if (!dir.exists()) {
        qWarning() << "Slideshow folder does not exist:" << folderPath;
        return {};
    }

    QStringList slides;

    // 1) Images in the base folder
    slides << collectImages(dir);

    // 2) PPT/PPTX rendered PNGs (cached)
    slides << convertPowerPoints(dir);

    // 3) Deterministic order
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
        QDir::Name
        );

    for (const QFileInfo &fi : files) {
        result << fi.absoluteFilePath();
    }

    return result;
}


bool Score_board::isLinuxPlatform() const
{
#ifdef Q_OS_LINUX
    return true;
#else
    return false;
#endif
}


