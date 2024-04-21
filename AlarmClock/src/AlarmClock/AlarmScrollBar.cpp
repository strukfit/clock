#include "AlarmClock/AlarmScrollBar.h"

AlarmScrollBar::AlarmScrollBar(QWidget* parent) :
	QScrollBar(Qt::Orientation::Vertical, parent)
{
	setDefaultStyle();
}

bool AlarmScrollBar::event(QEvent* event)
{ 
	if (event->type() == QEvent::Enter)
	{
		this->setStyleSheet(R"(
			QScrollBar::vertical {
				background-color: #2C2C2C;
				width: 12px;
				border-radius: 5px;
				margin: 0;
			}
		
			QScrollBar::sub-line:vertical {
				background: none;
				height: 15px;
				subcontrol-position: top;
				subcontrol-origin: margin;
			}
		
			QScrollBar::add-line:vertical {
				background: none;
				height: 15px;
				subcontrol-position: bottom;
				subcontrol-origin: margin;
			}
		
			QScrollBar::handle::vertical {
				background-color: #9F9F9F;
				border-radius: 2px;
				margin: 12px 3px 12px 4px;
			}
		
			QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
				background: none;
			}

			QScrollBar::up-arrow:vertical {
				image: url(Resources/arrow-up-grey.png);
				height: 15px;
				width: 12px;
			}

			QScrollBar::down-arrow:vertical {
				image: url(Resources/arrow-down-grey.png);
				height: 15px;
				width: 12px;
			}
		)");

		return true;
	}

	if (event->type() == QEvent::Leave)
	{
		setDefaultStyle();

		return true;
	}

	return QWidget::event(event);
}

void AlarmScrollBar::setDefaultStyle()
{
	this->setStyleSheet(R"(
		QScrollBar::vertical {
			background-color: transparent;
			width: 12px;
			border-radius: 5px;
			margin: 0;
		}
		
		QScrollBar::sub-line:vertical {
			background: none;
			height: 15px;
			subcontrol-position: top;
			subcontrol-origin: margin;
		}
		
		QScrollBar::add-line:vertical {
			background: none;
			height: 15px;
			subcontrol-position: bottom;
			subcontrol-origin: margin;
		}
		
		QScrollBar::handle::vertical {
			background-color: #9F9F9F;
			border-radius: 1px;
			margin: 12px 2px 12px 8px;
		}
		
		QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
			background: none;
		}

		QScrollBar::up-arrow:vertical {
			background: none;
		}

		QScrollBar::down-arrow:vertical {
			background: none;
		}
	)");
}
