#include "delegatewithoutfocus.h"

DelegateWithoutFocus::DelegateWithoutFocus(QObject *parent)
  :QStyledItemDelegate(parent)
{
}

void DelegateWithoutFocus::paint(QPainter *painter,
                                 const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
  QStyleOptionViewItem opt = option;
  opt.state &= ~QStyle::State_HasFocus;
  QStyledItemDelegate::paint(painter,opt,index);
}
