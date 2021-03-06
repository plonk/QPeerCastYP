/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "actions.h"
#include "application.h"
#include "mainwindow.h"
#include "channellistwidget.h"
#include "channel.h"
#include "commandaction.h"
#include "process.h"
#include "settings.h"
#include "utils.h"

Actions::Actions(MainWindow *mainWindow)
    : QObject(mainWindow), m_mainWindow(mainWindow), m_userActions(0)
{
    // ファイル
    m_quitAction = new QAction(QIcon(":/images/exit.png"), tr("終了(&Q)"), this);
    m_quitAction->setShortcut(QKeySequence(tr("Ctrl+Q")));
    m_quitAction->setMenuRole(QAction::QuitRole);
    connect(m_quitAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(quit()));

    // イエローページ
    m_updateYellowPageAction = new QAction(QIcon(":/images/reload.png"), tr("イエローページを更新(&R)"), this);
    m_updateYellowPageAction->setShortcut(QKeySequence("Ctrl+R"));
    connect(m_updateYellowPageAction, SIGNAL(changed()), this, SLOT(updateYellowPageActionChanged()));
    connect(m_updateYellowPageAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(updateYellowPage()));

    m_toggleAutoUpdateAction = new QAction(QIcon(), tr("自動更新を有効に(&A)"), this);
    m_toggleAutoUpdateAction->setCheckable(true);
    connect(m_toggleAutoUpdateAction, SIGNAL(toggled(bool)), m_mainWindow, SLOT(setAutoUpdateEnabled(bool)));

    m_updateYellowPageToolBarAction = new QAction(m_updateYellowPageAction->icon(),
                                                  m_updateYellowPageAction->text(), this);
    m_updateYellowPageToolBarAction->setIconText(tr("更新"));
#ifndef Q_WS_MAC
    m_updateYellowPageToolBarAction->setMenu(new QMenu(m_mainWindow));
    m_updateYellowPageToolBarAction->menu()->addAction(m_toggleAutoUpdateAction);
#endif
    connect(m_updateYellowPageToolBarAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(updateYellowPage()));

    m_playChannelAction = new QAction(QIcon(":/images/play.png"), tr("チャンネルを再生(&P)"), this);
    m_playChannelAction->setIconText(tr("再生"));
    connect(m_playChannelAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(playChannel()));

    m_addToFavoritesAction = new QAction(QIcon(":/images/favorite.png"), tr("お気に入りに追加(&A)"), this);
    m_addToFavoritesAction->setIconText(tr("お気に入り"));
    m_addToFavoritesAction->setShortcut(QKeySequence("Ctrl+D"));
    connect(m_addToFavoritesAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(addToFavorites()));

    m_unfavoriteAction = new QAction(QIcon(":/images/unfavorite.png"), tr("お気に入り解除(&U)"), this);
    m_unfavoriteAction->setIconText(tr("お気に入り解除"));
    connect(m_unfavoriteAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(unfavorite()));

    m_addToNGAction = new QAction(QIcon(":/images/stopped.png"), tr("NGに追加(&N)"), this);
    m_addToNGAction->setIconText(tr("NG"));
    m_addToNGAction->setShortcut(QKeySequence("Ctrl+Shift+D"));
    connect(m_addToNGAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(addToNG()));

    m_openContactUrlAction = new QAction(QIcon(":/images/browser.png"), tr("コンタクトURLを開く(&W)"), this);
    m_openContactUrlAction->setIconText(tr("コンタクト"));
    m_openContactUrlAction->setShortcut(QKeySequence("Ctrl+Return"));
    connect(m_openContactUrlAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(openContactUrl()));

    m_openStatsUrlAction = new QAction(QIcon(":/images/graph.png"), tr("統計URLを開く(&T)"), this);
    m_openStatsUrlAction->setIconText(tr("統計"));
    m_mainWindow->addAction(m_openStatsUrlAction);
    connect(m_openStatsUrlAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(openStatsUrl()));
    
    m_openContactUrlWith2chBrowserAction = new QAction(tr("コンタクトURLを&2ちゃんねるブラウザで開く"), this);
    m_openContactUrlWith2chBrowserAction->setShortcut(QKeySequence("Shift+Return"));
    // connect(m_openContactUrlWith2chBrowserAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(openContactUrlWith2chBrowser()));

    m_copyChannelInfoAction = new QAction(QIcon(":/images/copy.png"), tr("チャンネル情報をコピー(&I)"), this);
    m_copyChannelInfoAction->setShortcut(QKeySequence("Ctrl+C,Ctrl+C"));
    connect(m_copyChannelInfoAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(copyChannelInfo()));

    m_copyStreamUrlAction = new QAction(QIcon(":/images/copy.png"), tr("ストリームURLをコピー(&S)"), this);
    m_copyStreamUrlAction->setShortcut(QKeySequence("Ctrl+C,Ctrl+S"));
    connect(m_copyStreamUrlAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(copyStreamUrl()));

    m_copyContactUrlAction = new QAction(QIcon(":/images/copy.png"), tr("コンタクトURLをコピー(&C)"), this);
    m_copyContactUrlAction->setShortcut(QKeySequence("Ctrl+C,Ctrl+T"));
    connect(m_copyContactUrlAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(copyContactUrl()));

    m_findChannelAction = new QAction(QIcon(":/images/find.png"), tr("チャンネルを検索(&F)"), this);
    m_findChannelAction->setIconText(tr("検索"));
    m_findChannelAction->setShortcut(QKeySequence("Ctrl+F"));
    connect(m_findChannelAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(findChannel()));

    // 設定
    m_showMenuBarAction = new QAction(tr("メニューバーを表示(&M)"), this);
    m_showMenuBarAction->setCheckable(true);
    m_showMenuBarAction->setShortcut(QKeySequence("Ctrl+M"));
    connect(m_showMenuBarAction, SIGNAL(toggled(bool)), m_mainWindow, SLOT(setMenuBarVisible(bool)));

    m_showToolBarAction = new QAction(tr("ツールバーを表示(&T)"), this);
    m_showToolBarAction->setCheckable(true);
    connect(m_showToolBarAction, SIGNAL(toggled(bool)), m_mainWindow, SLOT(setToolBarVisible(bool)));

    m_showStatusBarAction = new QAction(tr("ステータスバーを表示(&A)"), this);
    m_showStatusBarAction->setCheckable(true);
    connect(m_showStatusBarAction, SIGNAL(toggled(bool)), m_mainWindow, SLOT(setStatusBarVisible(bool)));

    m_showTabBarAction = new QAction(tr("タブバーを表示(&B)"), this);
    m_showTabBarAction->setShortcut(QKeySequence("Ctrl+T"));
    m_showTabBarAction->setCheckable(true);
    connect(m_showTabBarAction, SIGNAL(toggled(bool)), m_mainWindow, SLOT(setTabBarVisible(bool)));

    m_showSettingsAction = new QAction(QIcon(":/images/configure.png"), tr("%1 を設定(&C)...").arg(QApplication::applicationName()), this);
    m_showSettingsAction->setIconText(tr("設定"));
    m_showSettingsAction->setMenuRole(QAction::PreferencesRole);
    connect(m_showSettingsAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(showSettings()));

    m_showFavoritesAction = new QAction(QIcon(":/images/organize_favorites.png"), tr("お気に入りを整理(&O)..."), this);
    m_showFavoritesAction->setIconText(tr("お気に入りを整理"));
    connect(m_showFavoritesAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(showFavorites()));

    // ヘルプ
    m_aboutQPeerCastYPAction = new QAction(tr("Q&PeerCastYP について"), this);
    m_aboutQPeerCastYPAction->setIcon(QApplication::windowIcon());
    m_aboutQPeerCastYPAction->setMenuRole(QAction::AboutRole);
    connect(m_aboutQPeerCastYPAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(aboutQPeerCastYP()));

    m_aboutQtAction = new QAction(tr("&Qt について"), this);
    connect(m_aboutQtAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(aboutQt()));
}

Actions::~Actions()
{
}

QMenu *Actions::fileMenu(QWidget *parent) const
{
    QMenu *menu = new QMenu(tr("ファイル(&F)"), parent);
    menu->addAction(m_quitAction);
    return menu;
}

QMenu *Actions::yellowPageMenu(QWidget *parent) const
{
    QMenu *menu = new QMenu(tr("イエローページ(&Y)"), parent);
    menu->addAction(m_updateYellowPageAction);
    menu->addAction(m_toggleAutoUpdateAction);
    menu->addSeparator();
    menu->addAction(m_findChannelAction);
    return menu;
}

QMenu *Actions::channelMenu(QWidget *parent) const
{
    QMenu *menu = new QMenu(tr("チャンネル(&C)"), parent);
    menu->addAction(m_playChannelAction);
    menu->addAction(m_openContactUrlAction);
    menu->addSeparator();
    menu->addAction(m_addToFavoritesAction);
    menu->addAction(m_unfavoriteAction);
    menu->addAction(m_addToNGAction);
    menu->addSeparator();
    menu->addAction(m_showFavoritesAction);
    menu->addSeparator();
    menu->addAction(m_copyChannelInfoAction);
    menu->addAction(m_copyStreamUrlAction);
    menu->addAction(m_copyContactUrlAction);
    return menu;
}

QMenu *Actions::channelContextMenu(QWidget *parent) const
{
    QMenu *menu = new QMenu("チャンネルコンテキストメニュー", parent);
    if (m_mainWindow->menuBar()->isHidden() && m_mainWindow->toolBar()->isHidden()) {
        menu->addAction(m_updateYellowPageAction);
        menu->addAction(m_toggleAutoUpdateAction);
        menu->addSeparator();
    }
    menu->addAction(m_playChannelAction);
    menu->addAction(m_openContactUrlAction);
    menu->addAction(m_openStatsUrlAction);
    menu->addSeparator();
    menu->addAction(m_addToFavoritesAction);
    menu->addAction(m_unfavoriteAction);
    menu->addAction(m_addToNGAction);
    menu->addSeparator();
    menu->addAction(m_copyChannelInfoAction);
    menu->addAction(m_copyStreamUrlAction);
    menu->addAction(m_copyContactUrlAction);
    menu->addSeparator();
    menu->addAction(m_findChannelAction);
#ifndef Q_WS_MAC
    if (m_mainWindow->menuBar()->isHidden()) {
        menu->addSeparator();
        menu->addMenu(settingsMenu(menu));
        menu->addSeparator();
        menu->addAction(m_quitAction);
    }
#endif
    return menu;
}

QMenu *Actions::settingsMenu(QWidget *parent) const
{
    QMenu *menu = new QMenu(tr("設定(&S)"), parent);
#ifndef Q_WS_MAC
    menu->addAction(m_showMenuBarAction);
#endif
    menu->addAction(m_showToolBarAction);
    menu->addAction(m_showStatusBarAction);
    menu->addAction(m_showTabBarAction);
#ifndef Q_WS_MAC
    menu->addSeparator();
#endif
    menu->addAction(m_showSettingsAction);
    return menu;
}

QMenu *Actions::helpMenu(QWidget *parent) const
{
    QMenu *menu = new QMenu(tr("ヘルプ(&H)"), parent);
    menu->addAction(m_aboutQPeerCastYPAction);
    menu->addAction(m_aboutQtAction);
    return menu;
}

void Actions::playChannel(Channel *channel)
{
    if (!channel or !channel->isPlayable()) {
        m_mainWindow->showErrorMessage(tr("このチャンネルは再生できません。"));
        return;
    }
    Settings *s = qApp->settings();
    int size = s->beginReadArray("Player/Items");
    for (int i = 0; i < size; ++i) {
        s->setArrayIndex(i);
        if (channel->type().contains(QRegExp(s->value("Types").toString(), Qt::CaseInsensitive))) {
            QString program = s->value("Program").toString();
            QStringList args = Utils::shellwords(s->value("Args").toString());
            s->endArray();
            startProcess(program, args, channel);
            return;
        }
    }
    s->endArray();
    m_mainWindow->showErrorMessage(
            tr("%1 用のプレイヤーが設定されていません。").arg(channel->type().toUpper()));
    return;
}

void Actions::setClipboardText(const QString &text)
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(text, QClipboard::Selection);
    clipboard->setText(text, QClipboard::Clipboard);
}

void Actions::openUrl(const QUrl &url)
{
    if (qApp->settings()->value("Program/UseCommonWebBrowser", true).toBool()) {
        QDesktopServices::openUrl(url);
    } else {
        QString browser = qApp->settings()->value("Program/WebBrowser").toString();
        startProcess(browser, QStringList(url.toString()));
    }
}

void Actions::startProcess(const QString &program, const QStringList &args, Channel *channel)
{
    if (!Process::start(program, args, channel))
        QMessageBox::warning(QApplication::activeWindow(),
                tr("エラー"), tr("プログラムの実行に失敗しました。"));
}

QAction *Actions::quitAction() const
{
    return m_quitAction;
}

QAction *Actions::updateYellowPageAction() const
{
    return m_updateYellowPageAction;
}

void Actions::updateYellowPageActionChanged()
{
    bool enabled = m_updateYellowPageAction->isEnabled();
    m_updateYellowPageToolBarAction->setEnabled(enabled);
    m_toggleAutoUpdateAction->setEnabled(enabled);
    if (!enabled)
        m_mainWindow->setAutoUpdateEnabled(false);
}

QAction *Actions::updateYellowPageToolBarAction() const
{
    return m_updateYellowPageToolBarAction;
}

QAction *Actions::toggleAutoUpdateAction() const
{
    return m_toggleAutoUpdateAction;
}

QAction *Actions::playChannelAction() const
{
    return m_playChannelAction;
}

QAction *Actions::addToFavoritesAction() const
{
    return m_addToFavoritesAction;
}

QAction *Actions::unfavoriteAction() const
{
    return m_unfavoriteAction;
}

QAction *Actions::addToNGAction() const
{
    return m_addToNGAction;
}

QAction *Actions::openContactUrlAction() const
{
    return m_openContactUrlAction;
}

QAction *Actions::openStatsUrlAction() const
{
    return m_openStatsUrlAction;
}

QAction *Actions::openContactUrlWith2chBrowserAction() const
{
    return m_openContactUrlWith2chBrowserAction;
}

QAction *Actions::copyStreamUrlAction() const
{
    return m_copyStreamUrlAction;
}

QAction *Actions::copyContactUrlAction() const
{
    return m_copyContactUrlAction;
}

QAction *Actions::copyChannelInfoAction() const
{
    return m_copyChannelInfoAction;
}

QAction *Actions::findChannelAction() const
{
    return m_findChannelAction;
}

QAction *Actions::showStatusBarAction() const
{
    return m_showStatusBarAction;
}

QAction *Actions::showMenuBarAction() const
{
    return m_showMenuBarAction;
}

QAction *Actions::showToolBarAction() const
{
    return m_showToolBarAction;
}

QAction *Actions::showTabBarAction() const
{
    return m_showTabBarAction;
}

QAction *Actions::showSettingsAction() const
{
    return m_showSettingsAction;
}

QAction *Actions::showFavoritesAction() const
{
    return m_showFavoritesAction;
}

QAction *Actions::aboutQPeerCastYPAction() const
{
    return m_aboutQPeerCastYPAction;
}

QAction *Actions::aboutQtAction() const
{
    return m_aboutQtAction;
}

