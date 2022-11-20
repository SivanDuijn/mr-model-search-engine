import json
import seaborn as sn
import pandas as pd
import matplotlib.pyplot as plt
import sys

f = open("../frontend/model-viewer/public/PSBDatabase/confusion_matrix.json")
confusion = json.load(f)
# confusion["matrix"] = map(lambda l : map(lambda x : x / 200, l), confusion["matrix"])

out = ""
i = 0
for l in confusion["matrix"]:
    out += confusion["class_names"][i]
    for v in l:
        out += " & {:.3g}".format(v / 200)
    out += "\\\\\n\\hline\n"
    i += 1

print(out)

df_cm = pd.DataFrame(confusion["matrix"], confusion["class_names"],
        confusion["class_names"])
plt.figure(figsize = (10,7))
sn.heatmap(df_cm, annot=True, cmap="Blues", fmt='g')
plt.show()