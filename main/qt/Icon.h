#ifndef ICON_H_SVUFN7OR
#define ICON_H_SVUFN7OR

#include <QIcon>


class Icon: public QIcon
{
public:
	Icon(const QString &name, bool showUnknownIcon = true);
	virtual ~Icon();

private:
	void scanFiles(const QString &name);
	static const int sm_size[7];
};


#endif /* end of include guard: ICON_H_SVUFN7OR */

