#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTableWidget>
#include <QListWidget>
#include <QComboBox>
#include <QLabel>
#include <QKeyEvent>
#include <QContextMenuEvent>
#include <QMenu>
#include <QMessageBox>
#include <QFileDialog>
#include <QHeaderView>
#include <QPixmap>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Система учета кадров");
    resize(1000, 650);
    setupUi();
}

void MainWindow::setupUi() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(12);

    QHBoxLayout *topLayout = new QHBoxLayout();
    m_sizeEdit = new QLineEdit("5", this);
    m_sizeEdit->setFixedWidth(60);
    QPushButton *genBtn = new QPushButton("Сгенерировать случайный массив", this);

    topLayout->addWidget(new QLabel("Размер массива:"));
    topLayout->addWidget(m_sizeEdit);
    topLayout->addWidget(genBtn);
    topLayout->addStretch();
    mainLayout->addLayout(topLayout);

    QFormLayout *formLayout = new QFormLayout();
    m_nameEdit = new QLineEdit(this);
    m_positionEdit = new QLineEdit(this);
    m_salaryEdit = new QLineEdit(this);
    m_iconBtn = new QPushButton("Выбрать изображение...", this);

    formLayout->addRow("ФИО сотрудника:", m_nameEdit);
    formLayout->addRow("Должность:", m_positionEdit);
    formLayout->addRow("Оклад (qint16):", m_salaryEdit);
    formLayout->addRow("Фотография:", m_iconBtn);
    mainLayout->addLayout(formLayout);

    QHBoxLayout *crudButtonsLayout = new QHBoxLayout();
    QPushButton *addTableBtn = new QPushButton("➕ В таблицу", this);
    QPushButton *delTableBtn = new QPushButton("❌ Из таблицы", this);
    QPushButton *addListBtn  = new QPushButton("➕ В список", this);
    QPushButton *delListBtn  = new QPushButton("❌ Из списка", this);

    crudButtonsLayout->addWidget(addTableBtn);
    crudButtonsLayout->addWidget(delTableBtn);
    crudButtonsLayout->addWidget(addListBtn);
    crudButtonsLayout->addWidget(delListBtn);
    mainLayout->addLayout(crudButtonsLayout);

    m_filterCombo = new QComboBox(this);
    m_filterCombo->addItems({"Показать всех сотрудников", "Зарплата строго выше 50 000 руб."});
    mainLayout->addWidget(m_filterCombo);

    QHBoxLayout *displayLayout = new QHBoxLayout();
    m_tableWidget = new QTableWidget(this);
    m_tableWidget->setColumnCount(4);
    m_tableWidget->setHorizontalHeaderLabels({"Фото", "ФИО", "Должность", "Зарплата"});
    m_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_listWidget = new QListWidget(this);
    displayLayout->addWidget(m_tableWidget, 2);
    displayLayout->addWidget(m_listWidget, 1);
    mainLayout->addLayout(displayLayout);

    QHBoxLayout *sortLayout = new QHBoxLayout();
    QPushButton *sortStdBtn = new QPushButton("Стандартная сортировка (А-Я)", this);
    QPushButton *sortAscBtn = new QPushButton("Кастомная по окладу (▲)", this);
    QPushButton *sortDescBtn = new QPushButton("Кастомная по окладу (▼)", this);

    sortLayout->addWidget(sortStdBtn);
    sortLayout->addWidget(sortAscBtn);
    sortLayout->addWidget(sortDescBtn);
    mainLayout->addLayout(sortLayout);

    connect(genBtn, &QPushButton::clicked, this, &MainWindow::onGeneratePressed);
    connect(m_iconBtn, &QPushButton::clicked, this, &MainWindow::onSelectIconPressed);
    connect(addTableBtn, &QPushButton::clicked, this, &MainWindow::onAddTablePressed);
    connect(delTableBtn, &QPushButton::clicked, this, &MainWindow::onDeleteTablePressed);
    connect(addListBtn, &QPushButton::clicked, this, &MainWindow::onAddListPressed);
    connect(delListBtn, &QPushButton::clicked, this, &MainWindow::onDeleteListPressed);
    connect(m_filterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onFilterChanged);
    connect(sortStdBtn, &QPushButton::clicked, this, &MainWindow::onSortStandardPressed);
    connect(sortAscBtn, &QPushButton::clicked, this, &MainWindow::onSortCustomAscPressed);
    connect(sortDescBtn, &QPushButton::clicked, this, &MainWindow::onSortCustomDescPressed);

    centralWidget->setFocusPolicy(Qt::StrongFocus);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Delete) {
        if (m_tableWidget->hasFocus()) onDeleteTablePressed();
        else if (m_listWidget->hasFocus()) onDeleteListPressed();
    } else if (event->key() == Qt::Key_Escape) {
        close();
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event) {
    QMenu menu(this);
    QAction *clearAction = menu.addAction("Очистить все контейнеры данных");
    connect(clearAction, &QAction::triggered, this, &MainWindow::onClearAllTriggered);
    menu.exec(event->globalPos());
}

void MainWindow::onSelectIconPressed() {
    m_currentIconPath = QFileDialog::getOpenFileName(this, "Выбрать аватар", "", "Изображения (*.png *.jpg)");
    if (!m_currentIconPath.isEmpty()) {
        m_iconBtn->setText("Фотография успешно прикреплена");
    }
}

Employee MainWindow::getEmployeeFromInput() const {
    Employee emp;
    emp.name = m_nameEdit->text().trimmed();
    emp.position = m_positionEdit->text().trimmed();
    emp.salary = static_cast<qint16>(m_salaryEdit->text().toInt());
    emp.iconPath = m_currentIconPath;
    return emp;
}

void MainWindow::onAddTablePressed() {
    Employee emp = getEmployeeFromInput();
    if (emp.name.isEmpty()) return;

    int row = m_tableWidget->rowCount();
    m_tableWidget->insertRow(row);
    insertRowIntoTable(row, emp);
}

void MainWindow::insertRowIntoTable(int row, const Employee& emp) {
    QLabel *imgLabel = new QLabel(this);
    if (!emp.iconPath.isEmpty()) {
        imgLabel->setPixmap(QPixmap(emp.iconPath).scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        imgLabel->setText("[Нет]");
    }
    imgLabel->setAlignment(Qt::AlignCenter);

    m_tableWidget->setCellWidget(row, 0, imgLabel);
    m_tableWidget->setItem(row, 1, new QTableWidgetItem(emp.name));
    m_tableWidget->setItem(row, 2, new QTableWidgetItem(emp.position));
    m_tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(emp.salary)));
}

void MainWindow::onAddListPressed() {
    Employee emp = getEmployeeFromInput();
    if (emp.name.isEmpty()) return;

    QListWidgetItem *item = new QListWidgetItem();
    item->setText(QString("%1 | %2 | %3 ₽").arg(emp.name, emp.position).arg(emp.salary));
    item->setData(Qt::UserRole, emp.salary);
    if (!emp.iconPath.isEmpty()) {
        item->setIcon(QIcon(emp.iconPath));
    }
    m_listWidget->addItem(item);
}

void MainWindow::onGeneratePressed() {
    int count = m_sizeEdit->text().toInt();
    if (count <= 0) return;

    m_tableWidget->setRowCount(0);
    QStringList names = {"Алексей", "Дмитрий", "Ольга", "Мария", "Николай"};
    QStringList posts = {"Разработчик", "QA-Инженер", "Team Lead", "HR", "DevOps"};

    for (int i = 0; i < count; ++i) {
        Employee emp{ names[rand() % 5] + QString(" %1").arg(i+1), posts[rand() % 5], static_cast<qint16>(30000 + rand() % 70000), m_currentIconPath };
        int row = m_tableWidget->rowCount();
        m_tableWidget->insertRow(row);
        insertRowIntoTable(row, emp);
    }
}

void MainWindow::onDeleteTablePressed() {
    int row = m_tableWidget->currentRow();
    if (row >= 0) m_tableWidget->removeRow(row);
}

void MainWindow::onDeleteListPressed() {
    int row = m_listWidget->currentRow();
    if (row >= 0) delete m_listWidget->takeItem(row);
}

void MainWindow::onFilterChanged(int index) {
    for (int i = 0; i < m_tableWidget->rowCount(); ++i) {
        bool hide = (index == 1) && (m_tableWidget->item(i, 3)->text().toInt() <= 50000);
        m_tableWidget->setRowHidden(i, hide);
    }
}

void MainWindow::onSortStandardPressed() {
    m_listWidget->sortItems(Qt::AscendingOrder);
}

void MainWindow::onSortCustomAscPressed() {
    struct Compare {
        bool operator()(QListWidgetItem* a, QListWidgetItem* b) {
            return a->data(Qt::UserRole).toInt() < b->data(Qt::UserRole).toInt();
        }
    };
    QList<QListWidgetItem*> items;
    while(m_listWidget->count() > 0) items.append(m_listWidget->takeItem(0));
    std::sort(items.begin(), items.end(), Compare());
    for(auto* item : items) m_listWidget->addItem(item);
}

void MainWindow::onSortCustomDescPressed() {
    struct CompareDesc {
        bool operator()(QListWidgetItem* a, QListWidgetItem* b) {
            return a->data(Qt::UserRole).toInt() > b->data(Qt::UserRole).toInt();
        }
    };
    QList<QListWidgetItem*> items;
    while(m_listWidget->count() > 0) items.append(m_listWidget->takeItem(0));
    std::sort(items.begin(), items.end(), CompareDesc());
    for(auto* item : items) m_listWidget->addItem(item);
}

void MainWindow::onClearAllTriggered() {
    m_tableWidget->setRowCount(0);
    m_listWidget->clear();
}
