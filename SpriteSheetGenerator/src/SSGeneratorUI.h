// ========================================================================= //
// Copyright (c) 2020 Agustin Jesus Durand Diaz.                             //
// This code is licensed under the MIT license.                              //
// SSGeneratorUI.h                                                           //
// ========================================================================= //

#ifndef SS_GENERATOR_UI_H
#define SS_GENERATOR_UI_H

#include <QtWidgets/QMainWindow>
#include "ui_SSGeneratorUI.h"

class QLabel;

class SSGeneratorUI : public QMainWindow
{
    Q_OBJECT
public:
    SSGeneratorUI( QWidget* parent = nullptr );
    ~SSGeneratorUI();

private Q_SLOTS:
    void btnFolderSlot();
    void btnGenerateSlot();

private:
    void showAtlas( const int index );

private:
    Ui::SSGeneratorUI* m_ui;
    std::vector< QString > m_filenames;
    std::vector< QString > m_spriteSheets;
    QLabel* m_atlasLabel;
    int m_currentSS;
};

#endif // SS_GENERATOR_UI_H
