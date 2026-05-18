#pragma once

#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QIcon> // Include QIcon here

class NfFolderActionDelegate : public QStyledItemDelegate 
{
    Q_OBJECT
public:
    explicit NfFolderActionDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;

signals:
    void importRequested(const QString& folderPath);

private:
    QRect getButtonRect(const QStyleOptionViewItem& option) const;
    
    const int m_btnWidth = 24;
    const int m_btnPadding = 6;

    // Cache the icon here so it's loaded into memory exactly once
    QIcon m_plusIcon;
};
