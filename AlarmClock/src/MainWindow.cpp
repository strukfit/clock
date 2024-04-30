#include "MainWindow.h"

MainWindow::MainWindow(QWidget* parent) :
	QMainWindow(parent), 
	ui(new Ui::MainWindowClass),
	menuExpanded(false)
{
	ui->setupUI(this);

	overlayWidget = new QWidget(this);
	overlayWidget->setGeometry(geometry());
	overlayWidget->setStyleSheet("background-color: rgba(0, 0, 0, 0.75);");
	overlayWidget->hide();

	dbManager = new DatabaseManager(this);

	connect(dbManager, &DatabaseManager::dataReceived, this, &MainWindow::setAlarm);

	dbManager->open();

	dbManager->tableInit();

	dbManager->selectAll();

	AlarmClockWidget::lastId = dbManager->getLastId();

	setSideMenuConections();

	connect(this, &MainWindow::childWindowShowed, [&] {
		overlayWidget->resize(size());
		overlayWidget->show();
	});

	timer = new QTimer(this);

	setAlarmClockConnections();
	connect(timer, &QTimer::timeout, this, &MainWindow::checkAlarm);
	setTimerConnections();

	auto t = new TimerWidget(this, 0, QTime(0, 0, 30), "timer");
	auto t2 = new TimerWidget(this, 0, QTime(0, 1, 0), "timer");
	t->hide();
	t2->hide();

	//ui->timerListLayout->addWidget(t);
	//ui->timerListLayout->addWidget(t2);

	
	int total = QTime(0, 0, 0).msecsTo(t->getTime());
	QTime endTime = QTime::currentTime().addSecs(total);
	QSlider* s = new QSlider(Qt::Horizontal, this);
	s->setMinimum(0);
	s->setMaximum(total);
	auto p = new CircularProgressBar(this, total);
	auto l = new QLabel();
	
	//connect(s, &QSlider::valueChanged, [=]() { p->upd((qreal)s->value() / s->maximum()); l->setText(QString::number(s->value())); });

	ui->timerListLayout->addWidget(p);
	//ui->timerListLayout->addWidget(s);
	//ui->timerListLayout->addWidget(l);

	//auto b = new QPushButton(this);

	//connect(timer, &QTimer::timeout, this, [=] {
	//	int remainingSeconds = QTime::currentTime().secsTo(endTime);
	//	int progress = qMax(0, total - remainingSeconds);
	//	//p->upd((qreal)progress / total);
	//	//p->setProcess((qreal)progress / total);
	//});

	/*connect(timer, &QTimer::timeout, this, [=] {
		p->upd(0.0);
		for (double i = 0.0; i <= total; i+=0.0001)
		{
			p->upd((qreal)i / total);
		}
	});*/

	//connect(b, &QPushButton::clicked, [=] {
	//	//p->upd(qreal(0));
	//	/*for (int i = 100; i > 0; i--)
	//	{
	//		p->upd((qreal)i / 100);
	//	}*/
	//});
	
	//ui->timerListLayout->addWidget(b);

	timer->start(1000);	
}

MainWindow::~MainWindow()
{
	delete ui;

	delete overlayWidget;

	dbManager->close();

	delete dbManager;
}

void MainWindow::increaseMenu()
{
	ui->functionSelectorWidget->setStyleSheet(R"(
		background-color: #202020; 
		border-top-right-radius: 4px;
		border-bottom-right-radius: 4px;
		border-top: 1px solid #434343;
		border-right: 1px solid #434343;
		border-bottom: 1px solid #434343;
	)");
	//qlineargradient(x1:0, y1:0, x2:1, y2:0.274, stop:0 #202020, stop:1 #434343)
	ui->alarmClockButton->setText("    Alarm clock");
	ui->timerButton->setText("    Timer");
	ui->stopwatchButton->setText("    Stopwatch");
	ui->worldClockButton->setText("    World clock");

	ui->functionSelectorWidget->setFixedWidth(280);

	menuExpanded = true;
}

void MainWindow::decreaseMenu()
{
	ui->functionSelectorWidget->setStyleSheet("background-color: #202020; border: 0; border-radius: 0;");

	ui->alarmClockButton->setText("");
	ui->timerButton->setText("");
	ui->stopwatchButton->setText("");
	ui->worldClockButton->setText("");

	ui->functionSelectorWidget->setFixedWidth(48);
	
	menuExpanded = false;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	emit closed();
	QMainWindow::closeEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
	ui->functionSelectorWidget->setFixedHeight(this->height());
	QMainWindow::resizeEvent(event);
}

void MainWindow::setSideMenuConections()
{
	connect(ui->menuButton, &QPushButton::clicked, [&] {
		if (menuExpanded)
		{
			decreaseMenu();
		}
		else
		{
			increaseMenu();
		}
		});

	connect(ui->alarmClockButton, &QPushButton::toggled, [&](bool checked) {
		if (checked)
			ui->alarmsWidget->show();
		else
			ui->alarmsWidget->hide();
		decreaseMenu();
		});

	connect(ui->alarmClockButton, &QPushButton::clicked, this, &MainWindow::decreaseMenu);

	connect(ui->timerButton, &QPushButton::toggled, [&](bool checked) {
		if (checked)
			ui->timerWidget->show();
		else
			ui->timerWidget->hide();
		decreaseMenu();
		});

	connect(ui->timerButton, &QPushButton::clicked, this, &MainWindow::decreaseMenu);

	connect(ui->stopwatchButton, &QPushButton::toggled, [&](bool checked) {
		if (checked)
			ui->stopwatchWidget->show();
		else
			ui->stopwatchWidget->hide();
		decreaseMenu();
		});

	connect(ui->stopwatchButton, &QPushButton::clicked, this, &MainWindow::decreaseMenu);

	connect(ui->worldClockButton, &QPushButton::toggled, [&](bool checked) {
		if (checked)
			ui->worldClockWidget->show();
		else
			ui->worldClockWidget->hide();
		decreaseMenu();
		});

	connect(ui->worldClockButton, &QPushButton::clicked, this, &MainWindow::decreaseMenu);
}

void MainWindow::setAlarmClockConnections()
{
	connect(this, &MainWindow::alarmClockAdded, dbManager, &DatabaseManager::insertData);
	connect(this, &MainWindow::alarmClockUpdated, dbManager, &DatabaseManager::updateData);
	connect(this, &MainWindow::alarmClockDeleted, dbManager, &DatabaseManager::deleteData);

	connect(ui->addAlarmButton, &QPushButton::clicked, this, &MainWindow::openAddAlarmWindow);

	connect(ui->deleteAlarmsButton, &QPushButton::clicked, [&] {
		ui->deleteAlarmsButton->hide();
		ui->confirmButton->show();

		for (int i = 0; i < ui->alarmsListLayout->count(); i++)
		{
			AlarmClockWidget* alarm = qobject_cast<AlarmClockWidget*>(ui->alarmsListLayout->itemAt(i)->widget());
			alarm->deleteMode(true);
		}

	});

	connect(ui->confirmButton, &QPushButton::clicked, [&] {
		ui->confirmButton->hide();
		ui->deleteAlarmsButton->show();

		for (int i = 0; i < ui->alarmsListLayout->count(); i++)
		{
			AlarmClockWidget* alarm = qobject_cast<AlarmClockWidget*>(ui->alarmsListLayout->itemAt(i)->widget());
			alarm->deleteMode(false);
		}
	});

	auto updateTimer = [&]() {
		QTime currentTime = QTime::currentTime();

		for (int i = 0; i < ui->alarmsListLayout->count(); i++)
		{
			AlarmClockWidget* alarm = qobject_cast<AlarmClockWidget*>(ui->alarmsListLayout->itemAt(i)->widget());
			QTime remainingTime = QTime(0, 0).addSecs(currentTime.secsTo(alarm->getAlarmTime()) + 60);
			QString remainingTimeString = "in ";

			if (remainingTime.hour() == 0 && remainingTime.minute() == 0)
			{
				remainingTimeString += "1 day";
			}
			else if (remainingTime.hour() == 0)
			{
				if (remainingTime.minute() == 1)
					remainingTimeString += "1 minute";
				else
					remainingTimeString += QString("%1 minutes").arg(remainingTime.minute());
			}
			else if (remainingTime.minute() == 0)
			{
				if (remainingTime.hour() == 1)
					remainingTimeString += "1 hour";
				else
					remainingTimeString += QString("%1 hours").arg(remainingTime.hour());
			}
			else
			{
				remainingTimeString += QString("%1 h, %2 min.").arg(remainingTime.hour()).arg(remainingTime.minute());
			}

			alarm->setRemainingTime(remainingTimeString);
		}
	};

	updateTimer();

	connect(this->timer, &QTimer::timeout, this, updateTimer);
}

void MainWindow::setTimerConnections()
{
	//connect(ui->timerAddButton, &QPushButton::clicked, this, );

	connect(ui->deleteTimerButton, &QPushButton::clicked, [&] {
		ui->deleteTimerButton->hide();
		ui->timerConfirmButton->show();

		// Delete mode on
		/*for (int i = 0; i < ui->alarmsListLayout->count(); i++)
		{
			AlarmClockWidget* alarm = qobject_cast<AlarmClockWidget*>(ui->alarmsListLayout->itemAt(i)->widget());
			alarm->deleteMode(true);
		}*/

	});

	connect(ui->timerConfirmButton, &QPushButton::clicked, [&] {
		ui->timerConfirmButton->hide();
		ui->deleteTimerButton->show();

		// Delete mode off
		/*for (int i = 0; i < ui->alarmsListLayout->count(); i++)
		{
			AlarmClockWidget* alarm = qobject_cast<AlarmClockWidget*>(ui->alarmsListLayout->itemAt(i)->widget());
			alarm->deleteMode(false);
		}*/
	});
}

void MainWindow::checkAlarm()
{
	QTime currentTime = QTime::currentTime();

	for (int i = 0; i < ui->alarmsListLayout->count(); ++i)
	{
		AlarmClockWidget* alarm = dynamic_cast<AlarmClockWidget*>(ui->alarmsListLayout->itemAt(i)->widget());
		if (alarm)
		{
			QTime alarmTime = alarm->getAlarmTime();

			if (currentTime >= alarmTime &&
				currentTime < alarmTime.addSecs(2) &&
				alarm->isActive() &&
				!alarm->isNotifficationOpen())
			{
				// Alarm went off
				auto notificationWindow = new AlarmNotificationWindow(nullptr, alarm);

				connect(notificationWindow, &AlarmNotificationWindow::alarmSnoozed, this->dbManager, &DatabaseManager::updateData);

				connect(this, &MainWindow::closed, notificationWindow, &QDialog::close);

				QRect scr = QGuiApplication::primaryScreen()->geometry();

				notificationWindow->show();
				notificationWindow->activateWindow();
				notificationWindow->raise();


				notificationWindow->move((scr.right() - notificationWindow->frameSize().width()), (scr.bottom() - notificationWindow->frameSize().height()));
			}
		}
	}
}

void MainWindow::openAddAlarmWindow()
{
	emit childWindowShowed();

	AddAlarmWindow* addAlarmWindow = new AddAlarmWindow(this);
	addAlarmWindow->setModal(true);

	addAlarmWindow->setFocus();

	connect(addAlarmWindow, &AddAlarmWindow::setAlarm, this, [&](const int& id, const QString& name, const QTime& time) {
		AlarmClockWidget* alarm = setAlarm(id, name, time);
		alarm->setActive(true);
		emit alarmClockAdded(id, name, time);
	});

	connect(addAlarmWindow, &QDialog::finished, [&]() {
		overlayWidget->hide();
	});

	addAlarmWindow->exec();
}

void MainWindow::openEditAlarmWindow(AlarmClockWidget* alarm)
{
	emit childWindowShowed();

	EditAlarmWindow* editAlarmWindow = new EditAlarmWindow(this, alarm);
	editAlarmWindow->setModal(true);

	editAlarmWindow->setFocus();

	connect(editAlarmWindow, &EditAlarmWindow::updateAlarm, this, &MainWindow::updateAlarm);
	connect(editAlarmWindow, &EditAlarmWindow::deleteAlarm, this, &MainWindow::deleteAlarm);

	connect(editAlarmWindow, &QDialog::finished, [&]() {
		overlayWidget->hide();
	});

	editAlarmWindow->exec();
}

AlarmClockWidget* MainWindow::setAlarm(const int& id, const QString& name, const QTime& time)
{
	AlarmClockWidget* alarm = new AlarmClockWidget(this, id, time, name);
	ui->alarmsListLayout->addWidget(alarm);

	QObject::connect(alarm, &AlarmClockWidget::clicked, this, &MainWindow::openEditAlarmWindow);
	QObject::connect(alarm, &AlarmClockWidget::deleteButtonClicked, this, &MainWindow::deleteAlarm);

	QTimer::singleShot(1000, this, &MainWindow::checkAlarm);

	return alarm;
}

void MainWindow::updateAlarm(AlarmClockWidget* alarm, const QString& name, const QTime& time)
{
	alarm->setName(name);
	alarm->setAlarmTime(time);
	alarm->updateUI();

	emit alarmClockUpdated(alarm->getId(), name, time);
}

void MainWindow::deleteAlarm(AlarmClockWidget* alarm)
{
	emit alarmClockDeleted(alarm->getId());

	/*QSettings settings(settingsFile, QSettings::IniFormat);
	int defaultNameCounter = settings.value("defaultNameCounter", 0).toInt();
	settings.setValue("defaultNameCounter", --defaultNameCounter);*/

	alarm->deleteLater();
}
