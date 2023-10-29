/**
 * Created by LMR on 2023/10/27.
*/

#ifndef ITEM_H
#define ITEM_H

#include <QWidget>
class QListWidgetItem;

QT_BEGIN_NAMESPACE
namespace Ui
{
class Item;
}
QT_END_NAMESPACE

class Item: public QWidget
{
Q_OBJECT

public:
	explicit Item(QWidget* parent = nullptr);
	explicit Item(const QString& text, QWidget* parent = nullptr);

	~Item() override;

	void SetData(const QVariant& data);
	void SetListWidgetItem(QListWidgetItem* listWidgetItem);
	QListWidgetItem* GetListWidgetItem();

	void SetText(const QString& text);
	QString GetText();

protected:
	void DeleteButtonClicked();

signals:
	void itemClickedSignal(const QString& text);
	void deleteButtonClickedSignal(QListWidgetItem*);

private:
	Ui::Item* ui;
	QListWidgetItem* listItem;
};


#endif //ITEM_H
