# interface-Nodered

## Presentation du projet

Le but de ce projet est de mettre en place un systeme qui permet de mesurer l'affluence du fablab, d'avoir une meilleur vision de l'utilisation des machines, et aussi permettre un accès plus controllé aux machines

## Le choix technologique

Pour ce projet, nous partons d'une solution développée en Node-Red pour plusieurs raisons :
 - Node-red est installé par défault sur une distribution raspbian et donc on peut avoir le centre du système qui tourne sur un raspberry pi
 - Le bocal n'a pas des ressources énormes à consacrer à ce développement, le système node-red parrait une technologie rapide à prendre en main pour permettre aux membres du fablab de contribuer à ce developpement
 - il permet d'utiliser toutes les technologies qui devront permettre de mettre en place cette interface (SQlite pour la BDD, MQTT pour la communication entre modules, et une interface web simple)

## Architecture systeme

![borne-schema](https://user-images.githubusercontent.com/10775713/116858218-53befb00-abfe-11eb-9f73-2e6fd7e51082.png)
