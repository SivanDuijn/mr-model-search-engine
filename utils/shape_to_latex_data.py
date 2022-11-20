import json
import sys
import matplotlib.pyplot as plt

f = open("../frontend/model-viewer/public/PSBDatabase/feature_descriptors.json")
data = json.load(f)

f = open ("../frontend/model-viewer/public/PSBDatabase/files.json")
files_per_class = json.load(f)

plots = [[],[]]
i = 0
for c in files_per_class:
  if c == "Debug":
    continue
  if i < (len(files_per_class)-1) / 2:
    plots[0].append(c)
  else:
    plots[1].append(c)
  print(c)
  i += 1

# print(files_per_class)
x = [0,1,2,3,4,5,6,7,8,9]
descriptors = ["A3", "D1", "D2", "D3", "D4"]

for p in plots:
  fig, axs = plt.subplots(len(p), len(descriptors))
  i = 0 
  for d in descriptors:
    j = 0
    for c in p:
      files = files_per_class[c]
      for file in files: 
        f = file.split(".")[0];
        fn = f + "_processed.off"
        s = data[fn][d]
        axs[j][i].plot(x, s)
        axs[j][i].tick_params(axis='both', which='both', left=False, right=False, bottom=False,top=False,labelbottom=False,labelleft=False)
        # axs[j][i].tick_params(axis='y', which='both', bottom=False,top=False,labelbottom=False)
      j += 1
    i += 1
  fig.show()
plt.show()

# models = ["21_processed.off", "201_processed.off"]

# out = ""

# for model in models:
#   out += model + ":\n"
#   for descr in descriptors:
#     s = data[model][descr]
#     out += descr + ": "
#     i = 0
#     for v in s:
#       out += "(" + str(i) + "," + "{:.5f}".format(v) + ") "
#       i += 1
#     out += "\n"

# print(out)
