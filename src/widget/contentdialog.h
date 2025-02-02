/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright © 2015-2019 by The qTox Project Contributors
 * Copyright © 2024-2025 The TokTok team.
 */

#pragma once

#include "src/core/conferenceid.h"
#include "src/core/toxpk.h"
#include "src/model/dialogs/idialogs.h"
#include "src/model/status.h"
#include "src/widget/genericchatitemlayout.h"
#include "src/widget/tool/activatedialog.h"

#include <memory>

template <typename K, typename V>
class QHash;

class ContentLayout;
class Core;
class Friend;
class FriendChatroom;
class FriendListLayout;
class FriendWidget;
class GenericChatForm;
class GenericChatroomWidget;
class Conference;
class ConferenceRoom;
class ConferenceWidget;
class QCloseEvent;
class QSplitter;
class QScrollArea;
class Settings;
class Style;
class IMessageBoxManager;
class FriendList;
class ConferenceList;
class Profile;

class ContentDialog : public ActivateDialog, public IDialogs
{
    Q_OBJECT
public:
    ContentDialog(const Core& core, Settings& settings, Style& style,
                  IMessageBoxManager& messageBoxManager, FriendList& friendList,
                  ConferenceList& conferenceList, Profile& profile, QWidget* parent = nullptr);
    ~ContentDialog() override;

    FriendWidget* addFriend(std::shared_ptr<FriendChatroom> chatroom, GenericChatForm* form);
    ConferenceWidget* addConference(std::shared_ptr<ConferenceRoom> chatroom, GenericChatForm* form);
    void removeFriend(const ToxPk& friendPk) override;
    void removeConference(const ConferenceId& conferenceId) override;
    int chatroomCount() const override;
    void ensureSplitterVisible();
    void updateTitleAndStatusIcon();

    void cycleChats(bool forward, bool inverse = true);
    void onVideoShow(QSize size);
    void onVideoHide();

    void addFriendWidget(FriendWidget* widget, Status::Status status);
    bool isActiveWidget(GenericChatroomWidget* widget);

    bool hasChat(const ChatId& chatId) const override;
    bool isChatActive(const ChatId& chatId) const override;

    void focusChat(const ChatId& chatId);
    void updateFriendStatus(const ToxPk& friendPk, Status::Status status);
    void updateChatStatusLight(const ChatId& chatId);

    void setStatusMessage(const ToxPk& friendPk, const QString& message);

signals:
    void friendDialogShown(const Friend* f);
    void conferenceDialogShown(Conference* c);
    void addFriendDialog(Friend* frnd, ContentDialog* contentDialog);
    void addConferenceDialog(Conference* conference, ContentDialog* contentDialog);
    void activated();
    void willClose();
    void connectFriendWidget(FriendWidget& friendWidget);

public slots:
    void reorderLayouts(bool newConferenceOnTop);
    void previousChat();
    void nextChat();
    void setUsername(const QString& newName);
    void reloadTheme() override;

protected:
    bool event(QEvent* event) final;
    void dragEnterEvent(QDragEnterEvent* event) final;
    void dropEvent(QDropEvent* event) final;
    void changeEvent(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void moveEvent(QMoveEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

public slots:
    void activate(GenericChatroomWidget* widget);

private slots:
    void updateFriendWidget(const ToxPk& friendPk, QString alias);
    void onConferencePositionChanged(bool top);

private:
    void closeIfEmpty();
    void closeEvent(QCloseEvent* event) override;

    void retranslateUi();
    void saveDialogGeometry();
    void saveSplitterState();
    QLayout* nextLayout(QLayout* layout, bool forward) const;
    int getCurrentLayout(QLayout*& layout);
    void focusCommon(const ChatId& id,
                     QHash<std::reference_wrapper<const ChatId>, GenericChatroomWidget*> list);

private:
    QList<QLayout*> layouts;
    QSplitter* splitter;
    QScrollArea* friendScroll;
    FriendListLayout* friendLayout;
    GenericChatItemLayout conferenceLayout;
    ContentLayout* contentLayout;
    GenericChatroomWidget* activeChatroomWidget;
    QSize videoSurfaceSize;
    int videoCount;

    QHash<std::reference_wrapper<const ChatId>, GenericChatroomWidget*> chatWidgets;
    QHash<std::reference_wrapper<const ChatId>, GenericChatForm*> chatForms;

    QString username;
    Settings& settings;
    Style& style;
    IMessageBoxManager& messageBoxManager;
    FriendList& friendList;
    ConferenceList& conferenceList;
    Profile& profile;
};
