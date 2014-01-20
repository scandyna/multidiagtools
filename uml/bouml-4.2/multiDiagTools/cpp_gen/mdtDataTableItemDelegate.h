#ifndef _MDTDATATABLEDELEGATE_H
#define _MDTDATATABLEDELEGATE_H


class mdtDataTableItemDelegate : public QStyledItemDelegate {
  public:
    mdtDataTableItemDelegate(const QObject & parent);

    ~mdtDataTableItemDelegate();

    virtual void paint(const QPainter & painter, const QStyleOptionViewItem & option, const QModelIndex & index);

    virtual QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index);

};
#endif
