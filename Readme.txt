#########################################################
	STM32 Deploy MEthode
#########################################################

CubeMX génère le code pour l'IDE Keil V5 dans le dossier MDK-ARM
Les sources sont cependant stockées dans le dossier Src, en dehors de MDK-ARM

Le projet se trouve dans le dossier : STM32/val. On y trouve les dossiers Drivers/Inc contenant les lib et header du µP STM32F303, ainsi
que le projet pour l'IDE Keil V5 et les Src du projet 

#########################################################
	27/04/2019 TODO List
#########################################################

- Correction des courses des servomoteurs horizontal et vertical
- Ajout commandes photo par raspberry et intégration totale du code sur Slave
- Correction du fonctionnement du pointer lumineux
- Etalonnage des valeurs d'encoder pour les rotations d'angle
- Refonte du système de déplacement G X: Y: A:
