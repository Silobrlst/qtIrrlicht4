#ifndef ATTRIBUTETREEDELEGATE_H
#define ATTRIBUTETREEDELEGATE_H

#include <QStyledItemDelegate>
#include <qlineedit.h>
#include <qpainter.h>
#include <QStylePainter>

class AttributeTreeDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    AttributeTreeDelegate(QWidget *parent = 0) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override{
        QStyledItemDelegate::paint(painter, option, index);
        painter->drawRect(option.rect);
    }

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override{
        if (index.column() == 1) {
            QLineEdit editor;
            editor.setText(index.data().toString());
            return editor.sizeHint();
        } else {
            return QStyledItemDelegate::sizeHint(option, index);
        }
    }

    //запустить виджет для редактирования
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override{
        if (index.column() == 1) {
            QLineEdit *editor = new QLineEdit(parent);
            connect(editor, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()));
            return editor;
        }if(index.column() == 0){
            return 0;
        } else {
            return QStyledItemDelegate::createEditor(parent, option, index);
        }
    }

    //занести в виджет редактирования данные, чтобы отредактировать их
    void setEditorData(QWidget *editorIn, const QModelIndex &index) const override{
        if (index.column() == 1) {
            QString text = qvariant_cast<QString>(index.data());
            QLineEdit *editor = qobject_cast<QLineEdit *>(editorIn);
            editor->setText(text);
        } if(index.column() == 0){
        }else {
            QStyledItemDelegate::setEditorData(editorIn, index);
        }
    }

    //сохранить отредактированные данные в модель
    void setModelData(QWidget *editorIn, QAbstractItemModel *model, const QModelIndex &index) const override{
        if (index.column() == 1) {
            QLineEdit *editor = qobject_cast<QLineEdit *>(editorIn);
            model->setData(index, QVariant::fromValue(editor->text()));
        } if(index.column() == 0){

        }else {
            QStyledItemDelegate::setModelData(editorIn, model, index);
        }
    }

private slots:
    void commitAndCloseEditor(){
        QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
        emit commitData(editor);
        emit closeEditor(editor);
    }
};

#endif // ATTRIBUTETREEDELEGATE_H
