# SmartHouse
## torrent watcher
get torrent files from qBittorent and send updates to telegram chat

## secrets.py
очень удобно хранить все пароли/токены в одном файле. Тогда можно добавить его в .gitignore и ваши секреты не утекут в сеть когда запустишь git push.
Структура файла проста. Это обычный питон словарь:
```python
# Description
SoftName = {
  "key" : "value",
  }
```

Чтобы им пользоваться нужно добавить в Python фаил это:
```python
import sys
from pathlib import Path
here = Path(__file__).parent.absolute()
repository_root = (here / ".." ).resolve()   # перейти на папку выше. Когда файл secrets.py лежит в корне
sys.path.insert(0, str(repository_root))
```

потом импортировать нужный словарик
```python
from secrets import SoftName
```
и в нужном месте просто вызвать..
```python
key = SoftName['key']
```
