#ifndef TOAST_H
#define TOAST_H

#include <QLabel>
#include <QPropertyAnimation>

class Toast : public QWidget {
    Q_OBJECT;
public:
    enum Type { Success, Error, Info };
    explicit Toast(QWidget* parent = nullptr);
    void showMessage(const QString &message, Type type);
private:
    QLabel *label;
    QTimer *timer;
    QPropertyAnimation *animation;
    void paintEvent(QPaintEvent* event) override;
};


#endif //TOAST_H