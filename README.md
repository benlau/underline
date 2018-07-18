It is still under development


Example:

```C++

QList<int> output1 = _::map(QList<QString>() << "1" << "2" << "3", [](auto item) { return item.toInt();});

QVector<int> output2 = _::map(QVector<QString>() << "1" << "2" << "3", [](auto item) { return item.toInt();});

QVector<int> output3 = _::map(QVector<QString>() << "1" << "2" << "3", [](auto item, int index) { return item.toInt();});

```
