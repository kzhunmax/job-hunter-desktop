#include "Toast.h"

#include <QHBoxLayout>
#include <QGraphicsOpacityEffect>
#include <QPainter>
#include <QTimer>

Toast::Toast(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);

    auto *layout = new QVBoxLayout(this);
    label = new QLabel(this);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("color: white; font-weight: bold; font-size: 14px;");
    layout->addWidget(label);

    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, [this]() {
        animation->setDirection(QAbstractAnimation::Backward);
        animation->start();
    });

    auto *opacityEffect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(opacityEffect);

    animation = new QPropertyAnimation(opacityEffect, "opacity");
    animation->setDuration(300);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);

    connect(animation, &QPropertyAnimation::finished, this, [this]() {
        if (animation->direction() == QAbstractAnimation::Backward) {
            hide();
        }
    });

    hide();
}

void Toast::showMessage(const QString &message, Type type) {
    label->setText(message);

    QString color;
    switch(type) {
        case Success: color = "#10b981"; break; // Green
        case Error:   color = "#ef4444"; break; // Red
        case Info:    color = "#3b82f6"; break; // Blue
    }

    setStyleSheet(QString(".Toast { background-color: %1; border-radius: 8px; }").arg(color));
    adjustSize();

    if (parentWidget()) {
        int x = (parentWidget()->width() - width()) / 2;
        int y = parentWidget()->height() - 80;
        move(parentWidget()->mapToGlobal(QPoint(x, y)));
    }

    show();
    animation->setDirection(QAbstractAnimation::Forward);
    animation->start();
    timer->start(3000); // 3 seconds
}

void Toast::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor(styleSheet().split("background-color: ").last().split(";").first()));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 8, 8);
}