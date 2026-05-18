#include "NfFolderActionDelegate.h"
#include <QPainter>
#include <QMouseEvent>
#include <QFileSystemModel>

NfFolderActionDelegate::NfFolderActionDelegate(QObject* parent) 
    : QStyledItemDelegate(parent)
    , m_plusIcon(":/icons/plus.svg")
{}

QRect NfFolderActionDelegate::getButtonRect(const QStyleOptionViewItem& option) const 
{
    int x = option.rect.right() - m_btnWidth - m_btnPadding;
    int y = option.rect.top() + (option.rect.height() - m_btnWidth) / 2;
    return QRect(x, y, m_btnWidth, m_btnWidth);
}

void NfFolderActionDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const 
{
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        // Prevent the text from overlapping the button placement area on selection                                                                                                                     
        if (opt.state & QStyle::State_Selected) {
                opt.rect.setRight(opt.rect.right() - (m_btnWidth + m_btnPadding * 2));
        }

        // Draw the standard file system icon and folder label first                                                                                                                                   
        QStyledItemDelegate::paint(painter, opt, index);

        // Contextual Action: Render the cached asset icon only when the row is selected                                                                                                               
        if (option.state & QStyle::State_Selected) {
                painter->save();
                painter->setRenderHint(QPainter::Antialiasing);

                QRect btnRect = getButtonRect(option);

                // Draw background base for the icon
                painter->setPen(Qt::NoPen);
                painter->setBrush(option.palette.accent());
                painter->drawRoundedRect(btnRect, 4, 4);

                // Add slight padding so the icon graphic sits comfortably inside the button boundaries
                QRect iconRect = btnRect.adjusted(4, 4, -4, -4); 
                
                // Use the pre-cached member icon. This operation is now computationally trivial.
                m_plusIcon.paint(painter, iconRect, Qt::AlignCenter, QIcon::Normal, QIcon::On);

                painter->restore();
        }
}

bool NfFolderActionDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) 
{
    if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease) {
        if (option.state & QStyle::State_Selected) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton && getButtonRect(option).contains(mouseEvent->pos())) {
                if (event->type() == QEvent::MouseButtonRelease) {
                    auto fsModel = qobject_cast<const QFileSystemModel*>(model);
                    if (fsModel) {
                        emit importRequested(fsModel->filePath(index));
                    }
                }
                return true; 
            }
        }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}
