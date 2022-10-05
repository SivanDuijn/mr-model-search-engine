# converts normalizationStats.json to two CSVs, one without and one with preprocessed stats
import sys
import json

f = open("normalizationStats.json")
stats = json.load(f)
f = open("files.json")
files = json.load(f)

unprocessedCSV = "file,nVertices,nFaces,position,aabbSize,angleX,angleY,angleZ,totalAngle,totalFlip\n"
processedCSV = "file,nVertices,nFaces,position,aabbSize,angleX,angleY,angleZ,totalAngle,totalFlip\n"

def addStatsToCSV(CSV, stats, file):
    CSV += file + "," + \
        str(stats["nVertices"]) + "," + \
        str(stats["nFaces"]) + "," + \
        str(stats["position"]) + "," + \
        str(stats["aabbSize"]) + "," + \
        str(stats["angleX"]) + "," + \
        str(stats["angleY"]) + "," + \
        str(stats["angleZ"]) + "," + \
        str(stats["totalAngle"]) + "," + \
        str(stats["totalFlip"]) + "\n"
    return CSV

for c in files:
    for file in files[c]:
        unpStats = stats[file]
        unprocessedCSV  = addStatsToCSV(unprocessedCSV, stats[file], file)
        ws = file.split(".")
        processedCSV  = addStatsToCSV(processedCSV, stats[ws[0] + "_processed." + ws[1]], ws[0] + "_processed." + ws[1])

output = open("normalizationStatsUnprocessed.csv", "w")
output.write(unprocessedCSV)
output.close()
output = open("normalizationStatsProcessed.csv", "w")
output.write(processedCSV)
output.close()