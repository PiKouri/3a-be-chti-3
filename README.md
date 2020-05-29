

# 3IR BE CHTI - Assembleur
## 3MIC-GRP E2
## Binôme : 
  * BENASSAI Marino  
  * HOK Jean-Rémy  
  
## Objectif 1 : Programmer la DFT en simulation sur un tableau de données imposées

- [x] Jeu de tests validé

Après que notre programme valide tous les tests du jeu fourni, nous considérons que notre DFT fonctionne correctement pour les étapes suivantes.

## Objectif 2 : Faire tourner la DFT "en réel" et gérer le score des 6 joueurs

### Mise à jour des scores

On augmente le score du joueur i à chaque fois que l'on détecte son tir sur l'une des cibles, donc à chaque fois que `occurences[i]` est à 3. 

### Vérifier nos résultats

Pour le vérifier, placer un point d'arrêt à l'instruction `score[i]++` du code c, et observer dans la ***watch window*** les tableaux `occurences` et `score`. Lorsque le point d'arrêt est atteint, vérifier que la case du tableau `score` qui vient d'être incrémenté correspond à la case d'`occurences` qui est à la valeur 3. 

Dans le **logic analyser**, afficher les variables `occurence[0]` à `occurence[5]`. Laisser le programme tourner pendant quelque temps. On peut alors observer des pics, chaque pic correspondant à la détection de l'un des tirs. Pour `Duree_Ech_ticks = 0x52` (dans le code c, c'est la valeur 82 = 0x52 dans l'instruction `Init_TimingADC_ActiveADC_ff( ADC1, 82 );`) , on vérifie que les graphes ont la même forme que dans le ficher GFSSP72.pdf.

![Occurences obtenues](https://github.com/PiKouri/3a-be-chti-3/blob/master/Screenshots/Occurences.png)

