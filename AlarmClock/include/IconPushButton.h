#pragma once

#include <QPushButton>

class IconPushButton : public QPushButton
{
public:
	IconPushButton(QWidget* parent = nullptr, QString text = "", QString textColor = "#000", QString textPressedColor = "#000", QString iconPath = "", QString pressedIconPath = "", QString backgroundColor = "#000", QString hoverColor = "#000", QString pressedColor = "#000", int borderRadius = 4, QString inactiveIconPath = "");

	void setInactiveIcon(bool flag = true);

protected:
	//void enterEvent(QEnterEvent* event) override;
	//void leaveEvent(QEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

private:
	QString iconPath;
	QString pressedIconPath;
	QString inactiveIconPath;
};