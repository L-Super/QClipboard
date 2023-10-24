/**
 * Created by LMR on 2023/10/24.
*/

#pragma once

#include <QWidget>



class Clipboard: public QWidget
{
Q_OBJECT

public:
	explicit Clipboard(QWidget* parent = nullptr);
	~Clipboard() override;

private:
	QClipboard *clipboard;
};


#endif //CLIPBOARD_H
