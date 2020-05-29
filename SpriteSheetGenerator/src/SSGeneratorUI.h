#ifndef SS_GENERATOR_UI_H
#define SS_GENERATOR_UI_H

#include <QtWidgets/QMainWindow>
#include "ui_SSGeneratorUI.h"

class SSGeneratorUI : public QMainWindow
{
    Q_OBJECT
public:
    SSGeneratorUI( QWidget* parent = 0 );
    ~SSGeneratorUI();

public Q_SLOTS:
    void btnFolderSlot();
    void btnGenerateSlot();

private:
    Ui::SSGeneratorUI* m_ui;
};

#endif // SS_GENERATOR_UI_H