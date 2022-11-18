import json
import seaborn as sn
import pandas as pd
import matplotlib.pyplot as plt
import sys

f = open("../frontend/model-viewer/public/PSBDatabase/confusion_matrix.json")
confusion = json.load(f)

df_cm = pd.DataFrame(confusion["matrix"], confusion["class_names"],
        confusion["class_names"])
plt.figure(figsize = (10,7))
sn.heatmap(df_cm, annot=True, cmap="Blues", fmt='.3g')
plt.show()