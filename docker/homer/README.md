# Homer

**[English](#english)** | **[Русский](#russian)**

---

<a id="english"></a>

## English

Homer is a tiny static web server that takes the contents of `config.yml` and renders it as a web page. It works very nicely as a start page for all the services in your smart home — like this:

![Homer start page](https://github.com/ageev/SmartHome/raw/master/Pictures/start_page.jpg)

I created an A record on my own domain pointing to Homer, then added the site to [NPM](https://github.com/ageev/SmartHome/tree/master/docker/nginx%20proxy%20manager). Now I don't have to remember internal IPs — I just type `<my_domain.com>` in the browser and see all the links.

I also installed a [theme for Homer](https://github.com/walkxcode/homer-theme). My app icons live in [tools.zip](https://github.com/ageev/SmartHome/raw/master/docker/homer/tools.zip).

### docker-compose.yml

```yaml
---
version: "3.9"
services:
  homer:
    image: b4bz/homer
    container_name: homer
    environment:
      - PUID=1029   # change me!
      - PGID=100    # change me!
      - TZ=Europe/Zurich
      - PORT=8080
    volumes:
      - /volume1/docker/homer:/www/assets
    ports:
      - 8080:8080
    restart: unless-stopped
    network_mode: "bridge"
```

### config.yml

> **Tip:** download all icons from [homer-icons](https://github.com/NX211/homer-icons/tree/master) and drop them in the `icons` folder for later use.

```yaml
---
# Homepage configuration
# See https://fontawesome.com/icons for icon options

title: "Start page"
subtitle: "Our home"
logo: "logo.png"

header: false
footer: false

# Theme
stylesheet:
  - "assets/custom.css"

columns: "3"  # any factor of 12: 1, 2, 3, 4, 6, 12
theme: default
colors:
  light:
    highlight-primary: "#fff5f2"
    highlight-secondary: "#fff5f2"
    highlight-hover: "#bebebe"
    background: "#12152B"
    card-background: "rgba(255, 245, 242, 0.8)"
    text: "#ffffff"
    text-header: "#fafafa"
    text-title: "#000000"
    text-subtitle: "#111111"
    card-shadow: rgba(0, 0, 0, 0.5)
    link: "#3273dc"
    link-hover: "#2e4053"
    background-image: "../assets/wallpaper-light.jpeg"
  dark:
    highlight-primary: "#181C3A"
    highlight-secondary: "#181C3A"
    highlight-hover: "#1F2347"
    background: "#12152B"
    card-background: "rgba(24, 28, 58, 0.8)"
    text: "#eaeaea"
    text-header: "#7C71DD"
    text-title: "#fafafa"
    text-subtitle: "#8B8D9C"
    card-shadow: rgba(0, 0, 0, 0.5)
    link: "#c1c1c1"
    link-hover: "#fafafa"
    background-image: "../assets/wallpaper.jpeg"

# Optional navbar
links:
  - name: "DSM"
    icon: "far fa-edit"
    url: "https://dsm"
    target: "_blank"
  - name: "VirtualDSM"
    icon: "far fa-edit"
    url: "http://dsm"
    target: "_blank"

# Services
# First-level array entries represent groups.
# Place the icons in "assets/tools/".
services:
  - name: "Video"
    icon: "fas fa-download"
    items:
      - name: "Plex Media Server"
        logo: "assets/tools/plex.png"
        subtitle: "Media server with movies and TV"
        tag: "movies"
        url: "https://<URL>/plex"
        target: "_blank"
      - name: "Radarr"
        logo: "assets/tools/radarr.png"
        subtitle: "Movie library manager"
        tag: "torrents"
        url: "https://<URL>/radarr"
        target: "_blank"
# ...and so on

  - name: "Utilities"
    icon: "fas fa-tools"
    items:
      - name: "Bitwarden"
        logo: "assets/tools/bitwarden.png"
        subtitle: "Password manager"
        tag: "passwords"
        tagstyle: "is-primary"
        url: "https://<URL>"
        target: "_blank"
```

---

<a id="russian"></a>

## Русский

Homer — это простой веб-сервер, который берёт содержимое `config.yml` и превращает его в веб-страницу. Очень удобен в виде стартовой страницы для всех сервисов вашего умного дома. Вот такой:

![Стартовая страница Homer](https://github.com/ageev/SmartHome/raw/master/Pictures/start_page.jpg)

Я создал DNS A-запись для своего домена, указывающую на Homer, и добавил сайт в [NPM](https://github.com/ageev/SmartHome/tree/master/docker/nginx%20proxy%20manager). Теперь не нужно запоминать адреса внутренней сети — просто вбиваю в браузер `<my_domain.com>` и вижу все ссылки.

Ещё установил [тему для Homer](https://github.com/walkxcode/homer-theme). Иконки приложений лежат в [tools.zip](https://github.com/ageev/SmartHome/raw/master/docker/homer/tools.zip).

### docker-compose.yml

```yaml
---
version: "3.9"
services:
  homer:
    image: b4bz/homer
    container_name: homer
    environment:
      - PUID=1029   # ваш!
      - PGID=100    # ваш!
      - TZ=Europe/Zurich
      - PORT=8080
    volumes:
      - /volume1/docker/homer:/www/assets
    ports:
      - 8080:8080
    restart: unless-stopped
    network_mode: "bridge"
```

### config.yml

> **Совет:** скачайте все иконки [отсюда](https://github.com/NX211/homer-icons/tree/master) и бросьте в папку `icons` для последующего использования.

```yaml
---
# Конфигурация стартовой страницы
# Иконки: https://fontawesome.com/icons

title: "Стартовая страница"
subtitle: "Наш дом"
logo: "logo.png"

header: false
footer: false

# Тема
stylesheet:
  - "assets/custom.css"

columns: "3"  # любой делитель 12: 1, 2, 3, 4, 6, 12
theme: default
colors:
  light:
    highlight-primary: "#fff5f2"
    highlight-secondary: "#fff5f2"
    highlight-hover: "#bebebe"
    background: "#12152B"
    card-background: "rgba(255, 245, 242, 0.8)"
    text: "#ffffff"
    text-header: "#fafafa"
    text-title: "#000000"
    text-subtitle: "#111111"
    card-shadow: rgba(0, 0, 0, 0.5)
    link: "#3273dc"
    link-hover: "#2e4053"
    background-image: "../assets/wallpaper-light.jpeg"
  dark:
    highlight-primary: "#181C3A"
    highlight-secondary: "#181C3A"
    highlight-hover: "#1F2347"
    background: "#12152B"
    card-background: "rgba(24, 28, 58, 0.8)"
    text: "#eaeaea"
    text-header: "#7C71DD"
    text-title: "#fafafa"
    text-subtitle: "#8B8D9C"
    card-shadow: rgba(0, 0, 0, 0.5)
    link: "#c1c1c1"
    link-hover: "#fafafa"
    background-image: "../assets/wallpaper.jpeg"

# Опциональный navbar
links:
  - name: "DSM"
    icon: "far fa-edit"
    url: "https://dsm"
    target: "_blank"
  - name: "VirtualDSM"
    icon: "far fa-edit"
    url: "http://dsm"
    target: "_blank"

# Сервисы
# Первый уровень массива — группы.
# Иконки кладите в "assets/tools/".
services:
  - name: "Видео"
    icon: "fas fa-download"
    items:
      - name: "Plex Media Server"
        logo: "assets/tools/plex.png"
        subtitle: "Медиа-сервер с фильмами и сериалами"
        tag: "movies"
        url: "https://<URL>/plex"
        target: "_blank"
      - name: "Radarr"
        logo: "assets/tools/radarr.png"
        subtitle: "Управление медиатекой фильмов"
        tag: "torrents"
        url: "https://<URL>/radarr"
        target: "_blank"
# ...и так далее

  - name: "Утилиты"
    icon: "fas fa-tools"
    items:
      - name: "Bitwarden"
        logo: "assets/tools/bitwarden.png"
        subtitle: "Менеджер паролей"
        tag: "passwords"
        tagstyle: "is-primary"
        url: "https://<URL>"
        target: "_blank"
```
