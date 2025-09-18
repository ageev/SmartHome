# Homer
Гомер - это простой вебсервер, который берёт содержимое ```config.yml``` и преобразует его в вебстраницу.
Гомер очень удобен в виде стартовой страницы для всех сервисов вашего умного дома. 
Вот такой:

![Стартовая страница Homer](https://github.com/ageev/SmartHome/raw/master/Pictures/start_page.jpg)

Я создал DNS А запись для собственного домена, указывающую на Гомера, и добавил сайт в [NPM.](https://github.com/ageev/SmartHome/tree/master/docker/nginx%20proxy%20manager)
Теперь мне не нужно запоминать адреса внутренней сети. Я просто вбиваю в браузер <my_domain.com> и вижу все ссылки.
Еще я установил [тему на Гомера](https://github.com/walkxcode/homer-theme). Мои иконки разных приложений в файле [tools.zip](https://github.com/ageev/SmartHome/raw/master/docker/homer/tools.zip)

# docker-compose.yml
```yml
---
version: "3.9"
networks:
  homer:
    image: b4bz/homer
    container_name: homer
    environment:
      - PUID=1029 #change me!
      - PGID=100 #change me!
      - TZ=Europe/Zurich
      - PORT=8080
    volumes:
      - /volume1/docker/homer:/www/assets
    ports:
      - 8080:8080
    restart: unless-stopped
    network_mode: "bridge"
```

## config.yml

*СОВЕТ!* - скачайте все иконки [отсюда](https://github.com/NX211/homer-icons/tree/master) и бросьте в папку icons для последующего использования.

```yml
---
# Homepage configuration
# See https://fontawesome.com/icons for icons options

title: "Стартовая страница"
subtitle: "Наш дом"
logo: "logo.png"

header: false
footer: false

# theme

stylesheet:
  - "assets/custom.css"

columns: "3" # You can change this to any number that is a factor of 12: (1, 2, 3, 4, 6, 12)
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
    background-image: "../assets/wallpaper-light.jpeg" # Change wallpaper.jpeg to the name of your own custom wallpaper!
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
# links: [] # Allows for navbar (dark mode, layout, and search) without any links
links:
  - name: "DSM"
    icon: "far fa-edit"
    url: "https://dsm"
    target: "_blank" # optional html a tag target attribute
  - name: "VirtualDSM"
    icon: "far fa-edit"
    url: "http://dsm"
    target: "_blank" # optional html a tag target attribute    

# Services
# First level array represent a group.
# Leave only a "items" key if not using group (group name, icon & tagstyle are optional, section separation will not be displayed).
# Place the icons in "assets/tools/"
services:
  - name: "Видео"
    icon: "fas fa-download"
    items:
      - name: "Plex Media Server"
        logo: "assets/tools/plex.png"
        subtitle: "Медиа сервер с фильмами и сериалами"
        tag: "movies"
        url: "https://<URL>/plex"
        target: "_blank"
      - name: "Radarr"
        logo: "assets/tools/radarr.png"
        subtitle: "Управление медиатекой фильмов"
        tag: "torrents"
        url: "https://<URL>/radarr"
        target: "_blank"
# ...and others
        
  - name: "Утилиты"
    icon: "fas fa-tools"
    items:
      - name: "Bitwarden"
        logo: "assets/tools/bitwarden.png"
        subtitle: "Менеджер паролей"
        tag: "passwords"
        tagstyle: "is-primary" # This will display the is-primary color! Try changing it to; is-link, is-info, is-success, is-warning or is-danger!
        url: "https://<URL>"
        target: "_blank" 
```
