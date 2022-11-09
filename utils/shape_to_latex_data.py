import json
import sys

f = open("../frontend/model-viewer/public/PSBDatabase/feature_descriptors.json")
data = json.load(f)

models = ["21_processed.off", "201_processed.off"]
descriptors = ["A3", "D1", "D2", "D3", "D4"]

out = ""

for model in models:
  out += model + ":\n"
  for descr in descriptors:
    s = data[model][descr]
    out += descr + ": "
    i = 0
    for v in s:
      out += "(" + str(i) + "," + "{:.5f}".format(v) + ") "
      i += 1
    out += "\n"

print(out)
