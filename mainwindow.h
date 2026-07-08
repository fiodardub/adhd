#pragma once

#include <QMainWindow>
#include <QVector>
#include "struct.h"

class QLineEdit;
class QPushButton;
class QTableWidget;
class QListWidget;
class QComboBox;
class QKeyEvent;
class QContextMenuEvent;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void onGeneratePressed();
    void onSelectIconPressed();
    void onAddTablePressed();
    void onAddListPressed();
    void onDeleteTablePressed();
    void onDeleteListPressed();
    void onFilterChanged(int index);
    void onSortStandardPressed();
    void onSortCustomAscPressed();
    void onSortCustomDescPressed();
    void onClearAllTriggered();

private:
    void setupUi();
    Employee getEmployeeFromInput() const;
    void insertRowIntoTable(int row, const Employee& emp);

    QVector<Employee> m_employees;
    QString m_currentIconPath;

    QLineEdit *m_sizeEdit;
    QLineEdit *m_nameEdit;
    QLineEdit *m_positionEdit;
    QLineEdit *m_salaryEdit;

    QPushButton *m_iconBtn;
    QTableWidget *m_tableWidget;
    QListWidget *m_listWidget;
    QComboBox *m_filterCombo;
};
