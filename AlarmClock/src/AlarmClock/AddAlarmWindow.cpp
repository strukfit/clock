#include "AlarmClock/AddAlarmWindow.h"

AddAlarmWindow::AddAlarmWindow(QWidget* parent) :
	QDialog(parent), 
	ui(new Ui::SetAlarmWindowClass)
{
	ui->setupAddAlarmWindowUI(this);

	setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint | Qt::FramelessWindowHint);

	this->setAttribute(Qt::WA_DeleteOnClose);

	connect(ui->setAlarmButton, &QPushButton::clicked, this, [&] {
		int h = ui->hoursSpinBox->value();
		int m = ui->minutesSpinBox->value();

		QString name = ui->nameLineEdit->text();

		emit setAlarm(AlarmClockWidget::lastId + 1, name, QTime(h, m));

		close();
	});

	connect(ui->cancelButton, &QPushButton::clicked, this, [&] {
		close();
	});
}

AddAlarmWindow::~AddAlarmWindow()
{
	delete ui;
}

void AddAlarmWindow::setFocus()
{
	ui->hoursSpinBox->setFocus();
}
